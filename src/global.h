#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <json-c/json.h>

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#ifdef _WIN32
    #include <io.h>
    #define IS_TERMINAL(fptr) _isatty(_fileno(fptr))
#else
    extern int fileno(FILE*);
    /* Must forward declare fileno() here with extern because
     * it cannot be found when compiling as -std=c99 under POSIX
     */
    #include <unistd.h>
    #define IS_TERMINAL(fptr) isatty(fileno(fptr))
#endif

int is_terminal(FILE* fptr);

struct main_state {
	char *user_prompt;
	char *system_prompt;
	char *json_file;
	const char *api_key;
	json_object *root;
};

//
//
//struct chat_message {
//	char *role;
//	char *message;
//};
//
//struct main_program_state {
//	const char *restrict api_key;
//	char *restrict model;
//	struct chat_message *chat_messages;
//	float temperature;
//	bool stream;
//};
