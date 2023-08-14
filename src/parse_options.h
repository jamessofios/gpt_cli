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

// Check the value of errno after calling this function. The value should be zero unless an error occurs.
void parse_options(const int argc, char **restrict argv, struct main_state *restrict ms);
