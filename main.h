#ifndef _MAIN_H_
#define _MAIN_H_

#include <string.h>
#include <linux/limits.h>
#include <errno.h>
#include <stdbool.h>

#define PATH_SEP "/"
#define PREFIX "20211430"
#define MAIN_TAB "\t"
#define SUB_TAB " "

typedef struct command
{
	char* name;
	int (*execute)(int, char**);
	void (*print_usage)(int);
} command;

#define BEGIN_USE(size) static int command_index = size; \
	command commands[size] = { \

#define REG_COMMAND(header_name, handler_name) extern int handler_name (int,char**); \
	extern void handler_name##_print_usage (int);

#define USE_COMMAND(command_label, handler_name) { #command_label , &handler_name, &handler_name##_print_usage }

#define END_USE };
// utility functions
// @param a string to be compared
// @param b string to compare
int start_with(char*, char*);

#endif


