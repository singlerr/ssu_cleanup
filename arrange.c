#include "arrange.h"
#include <libgen.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>
#include <time.h>
#include <getopt.h>
#include "queue.h"
#define MAX_EXT 255
#define HOME_SYMBOL "~"
#define HOME_VAR "$HOME"

// entry represents one entry of directory
// holds its extension or null in case of directory
struct entry{
	char path[PATH_MAX];
	char inputpath[PATH_MAX];
	char extension[MAX_EXT];
};

// group of entries with same extension
struct ext_group{
	char extension[MAX_EXT];
	int count;
	struct entry** entries;
};

// join path variables in the sense of path separator
void join_path(char* parent, char* name, char* output){
	if(parent[strlen(parent) -1] == '/' || name[0] == '/')
        	sprintf(output, "%s" "%s", parent,name);
	else
		sprintf(output, "%s" PATH_SEP "%s", parent, name);
}

int execute_(char* path, char* inputpath, char* dest, int time_modified, char** exclude_dirs, int exclude_dirs_count, char** include_extensions, int include_extensions_count);

// fetch multiple args coming after options
char** parse_args(int argc, char** argv, int* out_size){
	int size = 0;
	
	int offset = optind -1;
	for(int i = offset; i < argc && *argv[i] != '-'; i++, size++){
		
	}

	if(size < 1)
		return NULL;

	char** args = (char**) malloc(sizeof(char*) * size);

	for(int i = 0; i < size; i++){
		char* o_arg = argv[offset + i];
		char* arg = (char*) malloc(sizeof(char) * (strlen(o_arg) + 1));
		memset(arg, 0, sizeof(arg));
		strcpy(arg, o_arg);
		args[i] = arg;
	}

	if(out_size != NULL)
		*out_size = size;

	return args;
}

// same with tree.c - validate dir(exists, directory, inside home directory)
char* validate_dir(char* path, char* home_dir, int* flag_not_exist, int* flag_not_dir, int* flag_outside_home){
	char* final_path;
	int path_len = strlen(path);
	if(path[0] == '~'){	
		char* temp = (char*) malloc(sizeof(char) * (path_len - 1 + 1));
		memset(temp, 0, sizeof(temp));
		
		for(int i = 1; i < path_len;i++){
			temp[i - 1] = path[i];
		}

		final_path = strcat(home_dir, temp);
		free(temp);

	}else if(path_len >= 4 && start_with(path, HOME_VAR)){
		
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

	char* t_ = realpath(final_path, output);
	if(t_ == NULL){
		*flag_not_exist = true;
		*flag_not_dir = false;
		*flag_outside_home = false;
		return NULL;
	}
	
	struct stat s_buf;
	if(stat(t_, &s_buf) != -1){
		if(! S_ISDIR(s_buf.st_mode)){
			*flag_not_exist = false;
			*flag_not_dir = true;
			*flag_outside_home = false;
			return NULL;
		}
	}else{
		*flag_not_exist = true;
		*flag_not_dir = false;
		*flag_outside_home = false;
	}

	if(! start_with(t_, home_dir)){
		*flag_not_exist = false;
		*flag_not_dir = false;
		*flag_outside_home = true;
		return NULL;
	}
	
	return output;
}

// execute
int arrange_command(int argc, char** argv){
	int flag_not_exist = false;
	int flag_not_dir = false;
	int flag_outside_home = false;
	opterr = 0;

	if(argc < 1){
		arrange_command_print_usage(true);
		printf("\n");
		return 0;
	}

	char* path = argv[0];

	if(start_with(path, "-")){
		arrange_command_print_usage(true);
		printf("\n");
		return 0;
	}
	
	struct passwd *pw = getpwuid(getuid());
	char input_path[PATH_MAX];
	memset(input_path, 0, sizeof(input_path));
	strcpy(input_path, path);

	char* t_path = validate_dir(path, pw->pw_dir, &flag_not_exist, &flag_not_dir, &flag_outside_home);

	if(t_path == NULL){
		if(flag_not_exist){
			printf("%s does not exist\n", path);
			return 0;
		}	

		if(flag_not_dir){
			printf("%s is not a directory\n", path);
			return 0;
		}

		if(flag_outside_home){
			printf("%s is outside the home directory\n", path);
			return 0;
		}
		return 0;
	}

	char short_opts[] = ":d:t:x:e:";
	int opt;

	char* dest = NULL;
	char** exclude_dirs = NULL;
	int exclude_dirs_count = 0;

	char** include_extensions = NULL;
	int include_extensions_count = 0;
	int time_modified = -1;
	
	// unlike tree command, these options must consume additional arguments
	// since optind points current position of argument, we need to eat up args until new option comes out
	while((opt = getopt(argc, argv, short_opts)) != -1){
		switch(opt){
			case 'd':
				dest = (char*) malloc(sizeof(char) * (strlen(optarg) + 1));
				memset(dest, 0, sizeof(dest));
				strcpy(dest, optarg);
				break;
			case 't':
				time_modified = atoi(optarg);
				break;
			case 'x':
				exclude_dirs = parse_args(argc, argv, &exclude_dirs_count);
				break;
			case 'e':
				include_extensions = parse_args(argc, argv, &include_extensions_count);
				break;
			case '?':
				printf("%c\n", optopt);
				break;
			default:
				break;
		}
	}

	// validate path followed by -d
	if(dest != NULL){
		// convert dest path to realpath
		char* temp = validate_dir(dest, pw->pw_dir, &flag_not_exist, &flag_not_dir, &flag_outside_home);
		if(temp == NULL){
			if(flag_not_exist){
				printf("%s does not exist\n", dest);
			}else if(flag_not_dir){
				printf("%s is not a directory\n", dest);
			}else if(flag_outside_home){
				printf("%s is outside the home directory\n", dest);
			}
			free(dest);
			return 0;
		}
		free(dest);
		dest = temp;
	}else{
		// without -d option, dest path defaults to current directory
		dest = (char*) malloc(sizeof(char) * PATH_MAX);
		char current_dir[PATH_MAX];
		char* result = realpath(".", current_dir);
		if(result == NULL){
			printf("Failed to fetch current path\n");
			return 0;
		}
		strcpy(dest, current_dir);
	}
	
	// validate directory name with input path followed by -x
	if(exclude_dirs != NULL){
		for(int i = 0; i < exclude_dirs_count; i++){
			char* d = exclude_dirs[i];
			char* new_d = (char*) malloc(sizeof(char) * PATH_MAX);
			memset(new_d, 0, sizeof(new_d));
			join_path(t_path, d, new_d);

			if(new_d == NULL){
				printf("Failed to fetch real path of %s\n", d);
				for(int k = 0; k < exclude_dirs_count; k++){
					free(exclude_dirs[i]);
					free(exclude_dirs);
				}
			
				if(dest != NULL){
					free(dest);
				}
				return 0;
			}
			exclude_dirs[i] = new_d;
		}
	}

	int r = execute_(t_path, input_path, dest,time_modified, exclude_dirs, exclude_dirs_count, include_extensions, include_extensions_count);
	optind = 0;	
	return 0;
}

void arrange_command_print_usage(int print_all){
	if(true){
		printf(MAIN_TAB "> arrange <DIR_PATH> [OPTION]...\n");
		printf(MAIN_TAB SUB_TAB "<none> : Display the directory structure recursively if <DIR_PATH> is a directory, including the size of each file\n");
		printf(MAIN_TAB SUB_TAB "-d <output_path> : Specify the output directory <output_path> where <DIR_PATH> will be arranged if <DIR_PATH> is a directory\n");
		printf(MAIN_TAB SUB_TAB "-x <exclude_path1, exclude_path2, ...> : Arrange the directory if <DIR_PATH> is a directory except for the files inside <exclude_path> directory\n");
		printf(MAIN_TAB SUB_TAB "-e <extension1, extension2, ...> : Arrange the directory with the specified extension <extension1, extension2, ...>");	
	}
}

// same with tree.c#ignore_rel, remove .,.. entry on scandir
int ignore_rel_(const struct dirent* entry){
	return strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0;
}


// get file extension from path
char* get_extension(char* path){
	char* ext = strrchr(path, '.');
	if(! ext){
		return NULL;
	}
	
	return ext + 1;
}

// allocate entry
struct entry* create_entry(char* path, char* inputpath, char* extension){
	
	struct entry* e = (struct entry*) malloc(sizeof(struct entry));
	memset(e->path, 0, sizeof(e->path));
	strcpy(e->path, path);
	memset(e->inputpath, 0, sizeof(e->inputpath));
	strcpy(e->inputpath, inputpath);

	if(extension != NULL)
		strcpy(e->extension, extension);
	
	return e;
}

// allocate group
struct ext_group* create_group(char* ext){
	struct ext_group* g = (struct ext_group*) malloc(sizeof(struct ext_group));
	g->count = 0;
	memset(g->extension, 0, sizeof(g->extension));
	strcpy(g->extension, ext);
	g->entries = NULL;

	return g;
}

// utilty function - extend array
void** extend_arr(void** original, int size, int new_size){
	if(size > new_size){
		return NULL;
	}

	void** new_arr = (void**) malloc(sizeof(void*) * new_size);
	for(int i = 0; i < size; i++){
		new_arr[i] = original[i];
	}

	for(int i = size; i < new_size; i++){
		new_arr[i] = NULL;
	}

	return new_arr;
}

// find extension group with provided extension name
struct ext_group* find_ext_match(struct ext_group** groups, int size, char* ext){
	for(int i = 0; i < size; i++){
		if(strcmp(groups[i]->extension, ext) == 0){
			return groups[i];
		}
	}

	return NULL;
}

// utility function - push new element to array or extend if needed
void** push_or_extend(void** arr, int* size, void* e){
	
	if(arr == NULL){
		*size = *size + 1;
		void** new_arr = (void**) malloc(sizeof(void*) * (*size));
		new_arr[*size - 1] = e;
		return new_arr; 
	}

	if(arr[*size - 1] != NULL){
		void** new_arr = extend_arr(arr, *size, *size + 1);
		new_arr[*size] = e;
		*size = *size + 1;
		return new_arr;
	}

	arr[*size] = e;
	return arr;
}


// checks whether a string in array
int str_contains(char** arr, int size, char* t){
	for(int i = 0; i < size; i++){
		if(strcmp(arr[i], t) == 0){
			return true;		
		}
	}

	return false;
}

// checks a string in arr starts with provided string
int str_begins(char** arr, int size, char* t){
	for(int i = 0; i < size; i++){
		if(start_with(t, arr[i])){
			return true;
		}
	}
	return false;
}


// deeply search entries inside directories
// if exclude_dirs_count is bigger than 0, it will check entry name everytime and exclude entries which path is inside exclude_dirs
// if include_extensions_count is bigger than 0, it will check entry extension everytime and exclude entries which extension is not in include_extensions
// if time_modified is bigger than 0, it will compare current time with last modified time of entry and exclude entries which time difference is bigger than time_modified
int scan_dirs_(char* path, char* inputpath, char** exclude_dirs, int exclude_dirs_count, char** include_extensions, int include_extensions_count, int time_modified, struct ext_group*** output_groups){
	int group_size = 0;

	// init array with 1
	// later extend it!
	struct ext_group** groups = NULL;

	struct dirent **d_list;
	
	struct queue_t* queue = create_queue();

	int count = scandir(path, &d_list, ignore_rel_, alphasort);
	if(count > 0){
		for(int i = 0; i < count; i++){
			struct dirent* d = d_list[i];
			char d_path[PATH_MAX];
			char in_path[PATH_MAX];
			
			// validate paths
			// input path - relative path to user input path
			// d_path - absolute path
			memset(d_path, 0, sizeof(d_path));
			memset(in_path, 0, sizeof(in_path));
			
			join_path(path, d->d_name, d_path);
		
			if(path[strlen(path)] == '/'){
				sprintf(in_path, "%s%s", inputpath, d->d_name);
			}else{
				sprintf(in_path, "%s/%s", inputpath, d->d_name);
			}
			
			struct entry* e = NULL;	
			struct stat s_b;
			
			if(stat(d_path, &s_b) == -1)
				continue;
			
			if(S_ISDIR(s_b.st_mode)){
				
				if(exclude_dirs_count > 0 && ! str_begins(exclude_dirs, exclude_dirs_count, d_path)){
					e = create_entry(d_path, in_path, NULL);
					push(queue, e);
				}else if(exclude_dirs_count <= 0){
					e = create_entry(d_path, in_path, NULL);
					push(queue, e);
				}
			}else{
				
				int time_flag = true;
				// filter with time_modified
				if(time_modified > 0){
					time_t current_time;
					time(&current_time);
					struct stat stat_buf;
					if(stat(d_path, &stat_buf) != -1){
						time_t file_modified_time = stat_buf.st_mtime;
						if(difftime(current_time, file_modified_time) > (double) time_modified){
							time_flag = false;
						}
					}
				}

				if(time_flag){
					char* ext = get_extension(d_path);
					if(ext != NULL){
						
						// filter with include_extensions
						int flag = include_extensions_count > 0 && str_contains(include_extensions, include_extensions_count, ext);
						flag |= include_extensions_count == 0;
						
						if(flag){
							// add new entry to extension group
							e = create_entry(d_path, in_path, ext);			
							struct ext_group* g = find_ext_match(groups, group_size, ext);
							
							// add entry to group
							if(g != NULL){
								g->entries = (struct entry**) push_or_extend(g->entries, &(g->count), e);
							}else{
								g = create_group(ext);
								g->entries = (struct entry**) push_or_extend(g->entries, &(g->count), e);
								groups = push_or_extend(groups, &group_size, g);
							}
						}

					
					}
				}

			}

			if(e != NULL){
				strcpy(e->inputpath, in_path);
			}
			
			free(d);
		}

		free(d_list);
	}

	// handle sub entries scanned
	while(! is_empty(queue)){
		struct entry* e = poll(queue);
		int sub_count = scandir(e->path, &d_list, ignore_rel_, alphasort);

		if(count > 0){
			for(int i = 0; i < sub_count; i++){
				struct dirent* d = d_list[i];
				char d_path[PATH_MAX];
				char in_path[PATH_MAX];

				memset(d_path, 0, sizeof(d_path));
				memset(in_path, 0, sizeof(in_path));

				join_path(e->path, d->d_name, d_path);

				sprintf(in_path, "%s/%s", e->inputpath, d->d_name);
				struct stat s_b;
				if(stat(d_path, &s_b) == -1)
					continue;
				struct entry* e_new = NULL;
				if(S_ISDIR(s_b.st_mode)){
					e_new = create_entry(d_path, in_path, NULL);
					push(queue, e_new);
				}else{
					int time_flag = true;
					if(time_modified > 0){
						time_t current_time;
						time(&current_time);
						struct stat stat_buf;
						if(stat(d_path, &stat_buf) != -1){
							time_t file_modified_time = stat_buf.st_mtime;
							//printf("%f\n", difftime(current_time, file_modified_time));
							if(difftime(current_time, file_modified_time) > (double) time_modified){
								time_flag = false;
							}
						}
					}
						
					if(time_flag){
				
						char* ext = get_extension(d_path);
						if(ext != NULL){
					
							int flag = include_extensions_count > 0 && str_contains(include_extensions, include_extensions_count, ext);
							flag |= include_extensions_count == 0;

							if(flag){

								e_new = create_entry(d_path, in_path, ext);
								struct ext_group* g = find_ext_match(groups, group_size, ext);
															
								if(g != NULL){
									g->entries = (struct entry**) push_or_extend(g->entries, &(g->count), e_new);
								}else{
									g = create_group(ext);
									g->entries = (struct entry**) push_or_extend(g->entries, &(g->count), e_new);
									groups = push_or_extend(groups, &group_size, g);
								}
							}
							
						}	
					}
				}

				if(e_new != NULL){
					strcpy(e_new->inputpath, in_path);
					
				}

				free(d);
			}

			free(d_list);
		}
	}

	*output_groups = groups;
	return group_size;
}

// fork new child process and execute new command
// used by diff, vi
int exec_cmd(char* file, char* const argv[]){
	int child_exit_status;
	pid_t pid;
	int status = 0;

	pid = fork();

	if(pid == 0){
		execv(file, argv);			
		return 0;
	}else if(pid < 0){
		return -1;
	}else{
		wait((int*) 0);
	/*
		pid_t w = waitpid(pid, &child_exit_status, WNOHANG);
		if(w == -1){
			return -1;
		}

		if(WIFEXITED(child_exit_status)){
			status = WEXITSTATUS(child_exit_status);
		}
	*/
	}

	return status;
}

// execute diff command in selection of resolve_conflicts
void diff(char* a, char* b){
	char** argv = (char**) malloc(sizeof(char*) * 4);
	argv[0] = "diff";
	argv[1] = (char*) malloc(sizeof(char) * (strlen(a) + 1));
	argv[2] = (char*) malloc(sizeof(char) * (strlen(b) + 1));
	argv[3] = NULL;

	memset(argv[1], 0, sizeof(argv[1]));
	memset(argv[2], 0, sizeof(argv[2]));

	strcpy(argv[1], a);
	strcpy(argv[2], b);
	
	exec_cmd("/bin/diff", argv);
}

// copy file from source to dest
int copy(char* source, char* dest){
	char buffer[4096];
	FILE* src_file;
	FILE* dest_file;
	src_file = fopen(source, "r");
	dest_file = fopen(dest, "wb");

	while(! feof(src_file)){
		size_t len = fread(buffer, 1, sizeof(buffer), src_file);
		if(len){
			fwrite(buffer, 1, len, dest_file);
		}
	}

	fclose(src_file);
	fclose(dest_file);
}

// execute vi command
void vi(char* path){
	char** argv = (char**) malloc(sizeof(char*) * 3);
	argv[0] = "vi";
	argv[1] = (char*) malloc(sizeof(char) * (strlen(path) + 1));
	argv[2] = NULL;
	memset(argv[1], 0, sizeof(argv[1]));

	strcpy(argv[1], path);
	
	exec_cmd("/bin/vi", argv);
}

// same with /bin/mkdir -p
// create directories recursively
int mkdir_p(char* path){
	char buffer[PATH_MAX];
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, path);
	for (char* p = strchr(buffer + 1, '/'); p; p = strchr(p + 1, '/')) {
        	*p = '\0';
        	if (mkdir(buffer, S_IRWXU) == -1) {
            		if (errno != EEXIST) {
                		*p = '/';
                		return -1;
            		}
        	}	
        	*p = '/';
    	}
	return 0;
}


// deprecated - not used
int rmdir_r(char* path){
	char** argv = (char**) malloc(sizeof(char*) * 4);
	argv[0] = "rm";
	argv[1] = "-r";
	argv[2] = (char*) malloc(sizeof(char) * (strlen(path) + 1));
	argv[3] = NULL;
	strcpy(argv[2], path);

	return exec_cmd("/bin/rm", argv);
}

// called when file name clashes between two entries
// user can select:
// 0. select -> copy only one file
// 1. diff -> show diff between file
// 2. vi
// 3. do not select - remove file
struct entry* resolve_conflicts(struct ext_group* group, struct entry* a, struct entry* b){
	if(a == NULL){
		return b;
	}
	while(true){
		printf("1. %s\n", a->inputpath);
		printf("2. %s\n", b->inputpath);
		printf("\n");
		printf("choose an option:\n");
		printf("0. select [num]\n");
		printf("1. diff [num1] [num2]\n");
		printf("2. vi [num]\n");
		printf("3. do not select\n");

		printf(PREFIX "> ");
	
		char command[15];
		memset(command, 0, sizeof(command));

		scanf("%s", command);
		getchar();
		if(strcmp(command, "select") == 0){
			int n;
			scanf("%d", &n);
			getchar(); // remove newline from buffer
			if(n == 1){
				return a;	
			}else if(n == 2){
				return b;
			}		
		}else if(strcmp(command, "diff") == 0){
			int a_n;
			int b_n;
			
			scanf("%d %d", &a_n, &b_n);
			
			getchar(); // remove new line from buffer

			char* a_path = a_n == 1 ? a->path : b->path;
			char* b_path = b_n == 2 ? b->path : a->path;
			
			diff(a_path, b_path);
		}else if(strcmp(command, "vi") == 0){
			int n;
			scanf("%d", &n);
			
			getchar(); // remove new line from buffer

			if(n == 1){
				vi(a->path);
			}else if(n == 2){
				vi(b->path);
			}
		}else if(strcmp(command, "do") == 0){
			char rest[10];
			memset(rest,0,sizeof(rest));
			scanf("%[^\n]", rest);
			getchar(); // remove newline from buffer
			if(strcmp(rest, "not select") == 0)
				return NULL;	
		}
	}
}

// find entry by name
struct entry* find_by_name(struct entry** entries, int c, char* str){
	char pth[PATH_MAX];
	for(int i = 0; i < c; i++){
		if(entries[i] == NULL){
			continue;
		}
		memset(pth, 0, sizeof(pth));
		strcpy(pth, entries[i]->inputpath);
		char* base_name = basename(pth);
		if(strcmp(base_name, str) == 0){
			return entries[i];
		}
	}

	return NULL;
}

int execute_(char* path, char* inputpath, char* dest, int time_modified, char** exclude_dirs, int exclude_dirs_count, char** include_extensions, int include_extensions_count){	
	struct ext_group** groups;

	int group_count = scan_dirs_(path, inputpath, exclude_dirs, exclude_dirs_count, include_extensions, include_extensions_count, time_modified, &groups);

	char dest_path[PATH_MAX];
	
	memset(dest_path, 0, sizeof(dest_path));
	if(dest[strlen(dest) - 1] == '/' || inputpath[0] == '/')
		sprintf(dest_path, "%s%s_arranged/", dest, inputpath);
	else
		sprintf(dest_path, "%s/%s_arranged/", dest, inputpath);
	mkdir_p(dest_path);
		
	for(int i = 0; i < group_count; i++){
		struct ext_group* g = groups[i];

		char ext_path[PATH_MAX];
		memset(ext_path, 0, sizeof(ext_path));
		sprintf(ext_path, "%s%s", dest_path, g->extension);
		mkdir_p(ext_path);
			
		struct entry** copied_entries = (struct entry**) malloc(sizeof(struct entry*) * g->count);
		int processed_entries = 0;
		int ignored_entries = 0;		
		for(int j = 0; j < g->count; j++){
			copied_entries[i] = NULL;
		}

		for(int j = 0; j < g->count; j++){
			struct entry* e = g->entries[j];
								
			char path_temp[PATH_MAX];
			memset(path_temp, 0, sizeof(path_temp));
			strcpy(path_temp, e->path);
			
			char file_name[PATH_MAX];
			memset(file_name, 0, sizeof(file_name));
			strcpy(file_name, basename(path_temp));
			
			char dest_file_path[PATH_MAX];
			sprintf(dest_file_path, "%s/%s", ext_path, file_name);
			
			struct stat temp_stat_buf;
			if(stat(dest_file_path, &temp_stat_buf) != -1){
				// conflict resolution required
				struct entry* existing = find_by_name(copied_entries, processed_entries, file_name);
				int free_buf = false;
				if(existing == NULL){
					existing = create_entry(dest_file_path, dest_file_path, g->extension);
					free_buf = true;
				}
				struct entry* selected = resolve_conflicts(g, existing, e);
					
				// do not select, remove target file
				if(selected == NULL){
					unlink(dest_file_path);
					ignored_entries++;
					// skip entire
					break;
				}else{
					// select
					copy(selected->path, dest_file_path); 
				}

				if(free_buf){
					free(existing);
				}
			}else{
				copied_entries[processed_entries] = e;
				copy(e->path, dest_file_path);
				processed_entries++;
			}
		}

		if(processed_entries == ignored_entries){
			// delete ext directory if no file was copied
			rmdir(ext_path);
		}
		free(copied_entries);
	}

	printf("%s arranged\n", inputpath);

	return 0;

	
	
}
