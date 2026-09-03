#pragma once
#include "global.h"
#include "construct_json.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>

void *alloc_main_state(void) __attribute_malloc__;
void free_main_state(struct main_state *restrict ms);

static const char * restrict help_text = "Usage: %s [-m --model] [-t --temperature] [-s --system_prompt] [-u --user_prompt] [-j --json_file] [-h --help] [-r --repl] [-e --stream]\n";

enum argument_detection {
	arg_not_found = 0,
	user_prompt = 1,
	system_prompt = 2,
	json_file = 3,
	temperature = 4,
	model = 5,
	help = 6,
	repl = 7,
	stream = 8
};

// Check the value of errno after calling this function. The value should be zero unless an error occurs.
enum argument_detection *parse_options(const int argc, char **restrict argv, struct main_state *restrict ms) __attribute_malloc__;
