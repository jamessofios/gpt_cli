#pragma once
#include "main.h"
#include "construct_json.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>

void *alloc_main_state(void);
void free_main_state(struct main_state *restrict ms);



static const char *help_text = "Usage: %s [-m --model] [-t --temperature] [-s --system_prompt] [-u --user_prompt] [-j --json_file] [-h --help] [-r --repl]\n";

enum argument_detection {
	arg_not_found = -1,
	user_prompt = 0,
	system_prompt = 1,
	json_file = 2,
	temperature = 3,
	model = 4,
	help = 5,
	repl = 6
};

// Check the value of errno after calling this function. The value should be zero unless an error occurs.
enum argument_detection *parse_options(const int argc, char **restrict argv, struct main_state *restrict ms);
