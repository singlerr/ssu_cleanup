#include "tree.h"
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include "queue.h"
#define MED_TERMINATED "├─"
#define MED_TERMINATED_TYPE 0
#define MED_NONTERMINATED "│"
#define MED_NONTERMINATED_TYPE 1
#define LAST "└─"
#define LAST_TYPE 2
#define BLANK "   "
#define EXTRA_LEN 30
#define OUTSIDE_DIR 0x1
#define EXECUTE_OK 0
#define PREFIX_SIZE 10

// context holds its internal queue that retains entries with same depth level
// for tree printing, directory entries have higher pritority than non-directory files
struct context{
	int level;
	struct queue_t* queue;
	char prefix[4096];
};

// from the root of directory, all parent directories and its sub entries are connected with linked list.
// scan_dirs will search all entries and create big tree
struct pathinfo{
	char path[PATH_MAX];
	char name[PATH_MAX];
	int type;
	int symbol_type;
	int child_count;
	struct pathinfo** childs;
	struct pathinfo* prev;
};

// scan_dir retrieves . and .. entry.
// we do not care them, so we have to ignore them
int ignore_rel(const struct dirent* entry){
	return strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0;
}


int execute(char* path, int s, int p, int sp);

// checks a path is valid.
// a path must:
// 1. have existing directory
// 2. be inside of home directory
// 3. be directory
// if it violates any of rules above, flags will set to true
// flags:
// 1. flag_not_exist
// 2. flag_not_dir
// 3. flag_outside_home
char* validate_dir_(char* path, char* home_dir, int* flag_not_exist, int* flag_not_dir, int* flag_outside_home){
	char* final_path;
	int path_len = strlen(path);
	// replace ~ to a user's home directory since realpath does not handle
	if(path[0] == '~'){
		char* temp = (char*) malloc(sizeof(char) * (path_len - 1 + 1));
		memset(temp, 0, sizeof(temp));

		for(int i = 1; i < path_len;i++){
			temp[i - 1] = path[i];
		}

		final_path = strcat(home_dir, temp);
		free(temp);
	// replace $HOME to a user's home directory
	}else if(path_len >= 4 && start_with(path, "$HOME")){

		char* temp = (char*) malloc(sizeof(char) * (path_len - 4 + 1));
		memset(temp, 0, sizeof(temp));

		for(int i = 4; i < path_len; i++){
			temp[i-4] = path[i];
		}

		final_path = strcat(home_dir,temp);
		free(temp);
	}else{
		final_path = path;
	}
	
	char* output = (char*) malloc(sizeof(char) * PATH_MAX);
	memset(output, 0, sizeof(output));
	
	// convert relative path to absolute path
	char* t_ = realpath(final_path, output);
	if(t_ == NULL){
		// null return value of realpath means target dir does not exist
		*flag_not_exist = true;
		*flag_not_dir = false;
		*flag_outside_home = false;
		return NULL;
	}

	struct stat s_buf;
	if(stat(t_, &s_buf) != -1){
		// check rule of 2 - a path must be directory
		if(! S_ISDIR(s_buf.st_mode)){
			*flag_not_exist = false;
			*flag_not_dir = true;
			*flag_outside_home = false;
			return NULL;
		}
	}else{
		// -1 means target directory does not exist
		*flag_not_exist = true;
		*flag_not_dir = false;
		*flag_outside_home = false;
	}

	// checks a path is outside the home directory
	if(! start_with(t_, home_dir)){
		*flag_not_exist = false;
		*flag_not_dir = false;
		*flag_outside_home = true;
		return NULL;
	}

	return output;
}


// called from main.c and core executor of the command
// as mentioned in main.c, argc and args do not match the actual inputs - "tree" string excluded
int tree_command(int argc, char** args){
	// tree does not accept empty args!
	if(! (argc > 0)){
		//print usage
		tree_command_print_usage(true);
		printf("\n");
		return -1;
	}

	// first arg must be path, not options which do not start with - or not be empty
	char* path = args[0];
	// strncmp
	if(start_with(path, "-")){
		tree_command_print_usage(true);
		printf("\n");
		return -1;
	}

	// flag for -s, which prints size of file or dir
	int print_size = 0;
	// flag for -p, which prints permission of file or dir
	int print_permission = 0;
	// flag for -sp, which both prints permission and size of file or dir
	int print_size_permission = 0;
	
	char* short_opt = "sp";
	int opt;
	opterr = 0;
	
	// retrieve optional flags
	while((opt = getopt(argc, args, short_opt)) != -1){
		switch(opt){
			case 's':
				print_size = true;
				break;
			case 'p':
				print_permission = true;
				break;
			case '?':
				tree_command_print_usage(true);
				printf("\n");
				return -1;
		}
	}
	
	
	print_size_permission = print_size && print_permission;
	// this means -sp has typed.
	// since print_size and print_permission has higher priority than print_size_permission,
	// these must be set to false or print_size will always be applied!
	if(print_size_permission){
		print_size = false;
		print_permission = false;
	}
	
	// main processing function - scan directories and print
	int r = execute(path, print_size, print_permission, print_size_permission);
	
	// execute returns integers with its conditions
	// EXECUTE_OK -> all tasks have been completed without any errors
	// OUTSIDE_DIR -> input path is outside the home directory
	// other -> other errors occurred
	switch(r){
		case EXECUTE_OK:
			break;
		case OUTSIDE_DIR:
			printf("%s is outside the home directory\n", path);
			break;
		default:
			tree_command_print_usage(true);
			printf("\n");
			break;
	}

	// set optind to 0 so that getopt can handle next commands
	optind = 0;
}

// free memory
void free_pathinfo(void* p){
	struct pathinfo* pi = (struct pathinfo*) p;
	free(pi);
}

// free memory
void free_context(void* p){
	struct context* c = (struct context*) p;
	free(c);
}

// print usage of this command
// called by main.c
void tree_command_print_usage(int print_all){
	// print all usage
	if(print_all){
		printf(MAIN_TAB "> tree <DIR_PATH> [OPTION]...\n");
		printf(MAIN_TAB SUB_TAB "<none> : Display the directory structure recursively if <DIR_PATH> is a directory\n");
		printf(MAIN_TAB SUB_TAB "-s : Display the directory structure recursively if <DIR_PATH> is a directory, including the size of each file\n");
		printf(MAIN_TAB SUB_TAB "-p : Display the directory structure recursively if <DIR_PATH> is a directory, including the permissions of each directory and file");
	}

}
// ├─ -> 0
// │ -> 1
// └─ -> 2
// deep scan required -> 3
// deep-scan entries from the root
// we cannot ensure that count of sub entries will not exceed max stack size,
// so I chose queue rather than recursive calls
// after tasks, root will have childs if and only if it has sub entries
// and its entries expand to large tree that cover all entries in the directory
int scan_dirs(struct pathinfo* root){
	struct dirent **list;
	
	// fetch entries of current directory
	// alphasort allows us to retrieve results sorted by alphabetical order
	int c = scandir(root->path, &list, ignore_rel, alphasort);
	// childs wasn't initialized on parent stage
	// because scandir occurs here
	if(c <= 0){
		// childs do not exist
		return c;
	}else{
		
		// create queue
		struct queue_t* queue = create_queue();
		
		// ready for retrieving childs
		root->child_count = c;
		root->childs = (struct pathinfo**) malloc(sizeof(struct pathinfo*) * c);
 		
		// find child entries with depth level 1(right under the root)
		for(int i = 0; i < c; i++){
			struct dirent* current = list[i];
			
			char path[PATH_MAX];
			memset(path, 0, sizeof(path));

			// create path relative to its parent path
			sprintf(path, "%s/" "%s" "%s", root->path, current->d_name, current->d_type == DT_DIR ? "/" : "");
			
			struct pathinfo* p = (struct pathinfo*) malloc(sizeof(struct pathinfo));
			memset(p->name, 0, sizeof(p->name));
			memset(p->path, 0, sizeof(p->path));

			// reset pathinfo struct
			p->symbol_type = -1;
			p->type = current->d_type;
			p->childs = NULL;
			p->child_count = 0;

			// set name and path
			strcpy(p->path, path);
			strcpy(p->name, current->d_name);
	
			root->childs[i] = p;
			
			struct stat st_buf;
		
			// push to queue if directory
			if(stat(path, &st_buf) != -1 && S_ISDIR(st_buf.st_mode)){
				push(queue, p);
			}
			
			free(current);
			
		}
		
		// free memory
		free(list);

		// search more deeper entries
		// here happens going down to the deepest entries
		while(! is_empty(queue)){

			struct pathinfo* p = (struct pathinfo*) poll(queue);
			
			// if directory, search its sub entries and add to its child
			if(p->type == DT_DIR){
				struct dirent **p_list;
				int count = scandir(p->path, &p_list, ignore_rel, alphasort);
				p->child_count = count;
				if(count > 0){
					struct pathinfo** childs = (struct pathinfo**) malloc(sizeof(struct pathinfo*) * count);
					memset(childs, 0, sizeof(childs));
					
					p->childs = childs;
					for(int i = 0; i < count; i++){
						struct dirent* d = p_list[i];
						char pth[PATH_MAX];
						snprintf(pth, sizeof(pth), "%s" "%s" "%s" , p->path, d->d_name, d->d_type == DT_DIR ? "/" : "");
					
						struct pathinfo* sub_p = (struct pathinfo*) malloc(sizeof(struct pathinfo));
						memset(sub_p->path, 0, sizeof(sub_p->path));
						memset(sub_p->name, 0, sizeof(sub_p->name));
						sub_p->type = d->d_type;
						sub_p->symbol_type = -1;
		
						sub_p->childs = NULL;
						sub_p->child_count = 0;
						strcpy(sub_p->path, pth);
						strcpy(sub_p->name, d->d_name);
				
						childs[i] = sub_p;
						
						// also directory then push it to queue to search more deeper
						struct stat st_buf;
						if(stat(pth, &st_buf) != -1 && S_ISDIR(st_buf.st_mode)){
							push(queue, sub_p);
						}

						free(d);
					}
				}

				free(p_list);
			}else{
				// do nothing
			}
                        

		}

		free_queue(queue);
		free(queue);

	}

	
	return c;
}

// fetch file informations of size and permission
// stat provides various members and we only take st_size and st_mode
// meaning full string(non empty string) will only return if any of flags are true
void get_extras(struct pathinfo* p, int print_size, int print_permission, int print_size_permission, char* output){
	
	char* path = (char*) malloc(sizeof(char) * PATH_MAX);
	strcpy(path, p->path);
	
	//printf("%d %d %d\n", print_size, print_permission, print_size_permission);
	// get size only
	if(print_size){
		struct stat buffer;
		int ret = stat(path, &buffer);
		sprintf(output, "[%llu] ", (unsigned long long) buffer.st_size);
		free(path);
		return;
	}


	// get permission only
	if(print_permission){
		struct stat s;
		stat(path, &s);
		sprintf(output, "[%s%s%s%s%s%s%s%s%s%s] ",
				(S_ISDIR(s.st_mode)) ? "d" : "-",
				(s.st_mode & S_IRUSR) ? "r" : "-",
				(s.st_mode & S_IWUSR) ? "w" : "-",
				(s.st_mode & S_IXUSR) ? "x" : "-",
				(s.st_mode & S_IRGRP) ? "r" : "-",
				(s.st_mode & S_IWGRP) ? "w" : "-",
				(s.st_mode & S_IXGRP) ? "x" : "-",
				(s.st_mode & S_IROTH) ? "r" : "-",
				(s.st_mode & S_IWOTH) ? "w" : "-",
				(s.st_mode & S_IXOTH) ? "x" : "-"
			);
		free(path);
		return;
	}

	// get size and permission
	if(print_size_permission){
		struct stat s;
		stat(path, &s);

		 sprintf(output, "[%s%s%s%s%s%s%s%s%s%s %lld] ",
                                (S_ISDIR(s.st_mode)) ? "d" : "-",
                                (s.st_mode & S_IRUSR) ? "r" : "-",
                                (s.st_mode & S_IWUSR) ? "w" : "-",
                                (s.st_mode & S_IXUSR) ? "x" : "-",
                                (s.st_mode & S_IRGRP) ? "r" : "-",
                                (s.st_mode & S_IWGRP) ? "w" : "-",
                                (s.st_mode & S_IXGRP) ? "x" : "-",
                                (s.st_mode & S_IROTH) ? "r" : "-",
                                (s.st_mode & S_IWOTH) ? "w" : "-",
                                (s.st_mode & S_IXOTH) ? "x" : "-",
				(long long) s.st_size
			 );
		 free(path);
		 return;
	}
}

struct context* create_context(){
	struct context* c = (struct context*) malloc(sizeof(struct context));
	c->level = 0;
	c->queue = NULL;

	return c;
}

int execute(char* path, int print_size, int print_permission, int print_size_permission){
	int flag_not_exist = false;
	int flag_not_dir = false;
	int flag_outside_home = false;
	struct passwd *pw = getpwuid(getuid());
	
	// validate inputpath using validate_dir_
	char* abs_path = validate_dir_(path, pw->pw_dir, &flag_not_exist, &flag_not_dir, &flag_outside_home);
	
	if(flag_outside_home){
		return OUTSIDE_DIR;
	}

	if(flag_not_exist || flag_not_dir){
		return -1;
	}
		
	// create root struct with path
	struct pathinfo* current_path = (struct pathinfo*) malloc(sizeof(struct pathinfo));
	memset(current_path->path, 0, sizeof(current_path->path));
	memset(current_path->name, 0, sizeof(current_path->name));
	current_path->type = DT_DIR;
	strcpy(current_path->path, abs_path);
	strcpy(current_path->name, path);
	
	// search all entries
	int r = scan_dirs(current_path);	
	// errors - no 
	if(! current_path->childs){
		return errno;
	}

	if(r == -1){
		return errno;
	}
	
	// create context queue - holds queue list of context, in other words, directory
	struct queue_t* context_queue = create_queue();
	// create root context - root directory
	struct context* top_context = create_context();
	
	// create root entry queue - queue for entries in root directory
	struct queue_t* top_queue = create_queue();
	push(top_queue, current_path);
	
	top_context->queue = top_queue;
	top_context->level = 0;
	memset(top_context->prefix, 0, sizeof(top_context->prefix));
	
	push(context_queue, top_context);

	int dir_count = 0;
	int file_count = 0;
	int is_first = true;
	struct context* last_ctx = NULL;
	
	// everytime current entry is directory, creates new context which holds its child entries.
	// They will be processed after cascading sub directories of its parent
	// since for tree-like printing, one line should be printed from the start of search and end of search
	while(! is_empty(context_queue)){
		
		struct context* current_ctx = (struct context*) peek(context_queue);
		last_ctx = current_ctx;
		struct queue_t* current_queue = current_ctx->queue;
				
		// free empty queue
		if(is_empty(current_queue)){
			struct context* ctx = (struct context*) poll(context_queue);
			free_queue(current_queue);
			free(current_queue);
			free_context(ctx);
			
			ctx = NULL;
			current_queue = NULL;
			current_ctx = NULL;
			last_ctx = NULL;
			continue;
		}

		
		while(! is_empty(current_queue)){
			struct pathinfo* p = (struct pathinfo*) poll(current_queue);
						
			char *file_info = (char*) malloc(sizeof(char) * (EXTRA_LEN + 1));
			memset(file_info,0, sizeof(file_info));
			
			// get file information in case of flags
			get_extras(p, print_size, print_permission, print_size_permission, file_info);
			
			// handle directory - create new context 
			if( p->type == DT_DIR){
				struct context* new_ctx = create_context();
				struct queue_t* new_queue = create_queue();
				new_ctx->queue = new_queue;
				new_ctx->level = current_ctx->level + 1;
				memset(new_ctx->prefix, 0, sizeof(new_ctx->prefix));

				char prefix[PATH_MAX];
				memset(prefix, 0, sizeof(prefix));
				char curr_prefix[PATH_MAX];
				memset(curr_prefix, 0, sizeof(curr_prefix));
				
				switch(p->symbol_type){
					// non terminated symbol only has to be printed when context's is terminated type 
					case MED_TERMINATED_TYPE:
						strcpy(prefix, MED_NONTERMINATED);
						strcpy(curr_prefix, MED_TERMINATED);
						break;
					case MED_NONTERMINATED_TYPE:
						strcpy(curr_prefix, MED_NONTERMINATED);
						break;
					case LAST_TYPE:
						strcpy(curr_prefix, LAST);
						break;
					default:
						
						break;
				}

				
				char context_prefix[4096];
				memset(context_prefix, 0, sizeof(context_prefix));
				
				char* appendix;

				// root path - input path, has no branch symbols
				if(is_first){
					appendix = "";
					memset(prefix, 0, sizeof(prefix));
				}else{
					appendix = BLANK;
				}
				
				sprintf(context_prefix, "%s" "%s" "%s", current_ctx->prefix, prefix, appendix);		
				
				strcpy(new_ctx->prefix, context_prefix);
				pushFirst(context_queue, new_ctx); // push new context and print this directory:	
				// print directory - current line terminated
				
				if(is_first){
					memset(curr_prefix, 0, sizeof(curr_prefix));
				}
				if(! is_first)
					printf("%s%s %s%s%s\n", current_ctx->prefix, curr_prefix, file_info, p->name, is_first ? "" : "/");
				else
					printf("%s%s%s%s%s\n", current_ctx->prefix, curr_prefix, file_info, p->name, is_first ? "" : "/");
				is_first = false;
				// add its children to new context's queue
				
				int child_c = p->child_count;
				
				for(int i = 0; i < child_c; i++){
					
					struct pathinfo* child = p->childs[i];
					
					// sign last type
					if(i + 1 == p->child_count){
						child->symbol_type = LAST_TYPE;
					}else{
						child->symbol_type = MED_TERMINATED_TYPE;
					}

					push(new_queue, child);
				}
				
				if(child_c < 1){
					free_pathinfo(p);
				}

				free(file_info);

				dir_count++;
				// break nearest while loop to poll this context on the next loop
				break;
			}

			
			if(p->type != DT_DIR){
				char file_prefix[PREFIX_SIZE];	
				switch(p->symbol_type){
					case MED_TERMINATED_TYPE:
						strcpy(file_prefix, MED_TERMINATED);
						break;
					case MED_NONTERMINATED_TYPE:
						strcpy(file_prefix, MED_NONTERMINATED);
						break;
					case LAST_TYPE:
						strcpy(file_prefix, LAST);
						break;
					default:
						break;
				}
				printf("%s%s %s%s\n", current_ctx->prefix, file_prefix, file_info, p->name);
				free(file_info);
				free_pathinfo(p);
				file_count++;
				continue;
			}	
		}

	}
	//free(top_context);	
	//free(context_queue);

		
	printf("%d directories, %d files\n", dir_count, file_count); 
	return EXECUTE_OK;
}

