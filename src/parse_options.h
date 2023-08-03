#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

struct main_state {
	char *user_prompt;
	char *system_prompt;
	char *json_file;
};

void *alloc_main_state(void);
void free_main_state(struct main_state *restrict ms);
void parse_options(const int argc, char **restrict argv, struct main_state *restrict ms);
