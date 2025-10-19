#include<stdio.h>
#include <stdlib.h>
#include "main.h"

#define BUFFER_SIZE 4096
#define MAX_COMMAND_STACK 10

// implementing start_with function in main.h
// return whether string a start with string b
int start_with(char* a, char* b){
	int l = strlen(b);
	return strncmp(a,b,l) == 0;
}

// register tree command
#include "tree.h"
REG_COMMAND(tree.h, tree_command)
// register arrange command
// REG_COMMAND will expand to statements that declare extern functions
#include "arrange.h"
REG_COMMAND(arrange.h, arrange_command)

// print usage of help command
void help_command_print_usage(int flag){
	printf(MAIN_TAB "> help [COMMAND]\n");
}

// print usage of exit command
void exit_command_print_usage(int flag){
	printf(MAIN_TAB "> exit\n");
}
	
int main(int argc, char** argv){
	// start creating command struct array of 2
	// command struct has function pointer members which hold handle of executor and usage printer
	BEGIN_USE(2)
	USE_COMMAND(tree, tree_command),
	USE_COMMAND(arrange, arrange_command)
	END_USE

	// create buffer for reading inputs from stdin
	char* buffer = (char*) malloc(sizeof(char) * BUFFER_SIZE);
	// command stack holds arguments by separating user inputs with whitespace
	char** command_stack = (char**) malloc(sizeof(char*) * MAX_COMMAND_STACK);
	for(int i = 0; i < MAX_COMMAND_STACK; i++){
		command_stack[i] = (char*) malloc(sizeof(char) * BUFFER_SIZE);
	}

	while(1){
		// clear buffer
		for(int i = 0; i < MAX_COMMAND_STACK; i++){
			memset(command_stack[i], 0, sizeof(command_stack[i]));
		}
		
		// prompt, ready to read!
		printf(PREFIX "> ");
		
		int stack_index = 0; //getchar means at least one char entered
		int c_idx = 0;
		char c;
		
		// read inputs until ENTER key typed
		while((c = getchar()) != '\n'){
			// separate arguments by whitespace
			if(c == ' '){
				stack_index++;
				c_idx = 0;
				continue;
			}
			
			// store arguments
			command_stack[stack_index][c_idx++] = c;
		}
		
		// first element of stack is command label
		char* input_command_label = command_stack[0];
		
		// if command not available, empty command typed
		// then print usage
		if(! (strlen(input_command_label) > 0)){
			printf("Usage:\n");
			for(int i = 0; i < command_index; i++){
				commands[i].print_usage(true);
				printf("\n");
			}

			help_command_print_usage(true);
			exit_command_print_usage(true);
			continue;
		}

		// special commands - not associated with command struct
		// help and exit commands are internal command that a user cannot add, remove or update
		if(strcmp(input_command_label, "help") == 0){
			// print all usage if target command to print help is not available
			if(! (stack_index > 0)){
				printf("Usage:\n");
				for(int i = 0; i < command_index; i++){
					commands[i].print_usage(true);
					printf("\n");
				}

				help_command_print_usage(true);
				exit_command_print_usage(true);
			}else{
				// find target command struct to call usage printer function
				char* target_cmd = command_stack[1];
				int cmd_found = false;
				for(int i = 0; i < command_index; i++){
					if(strcmp(commands[i].name, target_cmd) == 0){
						commands[i].print_usage(true);
						printf("\n");
						cmd_found = true;
						break;
					}
					
					// help also handles itself
					if(strcmp(target_cmd, "help") == 0){
						help_command_print_usage(true);
						cmd_found = true;
						break;
					}else if(strcmp(target_cmd, "exit") == 0){
						exit_command_print_usage(true);
						cmd_found = true;
						break;
					}
				}

				if(! cmd_found){
					printf("Usage:\n");
					for(int i = 0; i < command_index; i++){
						commands[i].print_usage(true);
					}

					help_command_print_usage(true);
					exit_command_print_usage(true);
				}
			}
			continue;
		}else if(strcmp(input_command_label, "exit") == 0){
			exit(1);
			return 0;
		}
		
		// here finds handle of command, collects arguments without command label(which is useless for the command) and executes
		int cmd_found = false;
		for(int i = 0; i < command_index; i++){
			// find command with that naem
			if(strcmp(commands[i].name, input_command_label) == 0){
				int sub_command_size = stack_index;
				char** sub_command_args = (char**) malloc(sizeof(char*) * sub_command_size); 
				
				// create sub command arguments that all arguments a user typed without input_command_label
				for(int j = 0; j < sub_command_size; j++){
					// copy original command to target command args
					char* original_command = command_stack[j+1];
					int len = strlen(original_command) + 2;
					sub_command_args[j] = (char*) malloc(sizeof(char) * len);
					memset(sub_command_args[j], 0, sizeof(sub_command_args[j]));
					strcpy(sub_command_args[j], original_command);
				}
				cmd_found = true;
				// execute the command
				// all controls will be passed over to it
				commands[i].execute(sub_command_size, sub_command_args);
				break;
			}
		}
		
		// could not find any command matching with input_command_label
		// then print the usage
		if(! cmd_found){
			printf("Usage:\n");
			for(int i = 0; i < command_index; i++){
				commands[i].print_usage(true);
				printf("\n");
			}

			help_command_print_usage(true);
			exit_command_print_usage(true);
		}
	

	}
	// clear memory
	free(commands);
	free(buffer);
	
	for(int i = 0; i < MAX_COMMAND_STACK; i++){
		free(command_stack[i]);
	}
	free(command_stack);
	return 0;
}

