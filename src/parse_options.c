#include "parse_options.h"

void parse_options(const int argc, char **restrict argv, struct main_state *restrict ms) {

	static struct option long_options[] = {
		{"user_prompt", required_argument, NULL, 'u'},
		{"system_prompt", required_argument, NULL, 's'},
		{"json_file", required_argument, NULL, 'j'},
		//the last entry in the array must be all zeros for getopt_long to know where the array ends
		{NULL, 0, NULL, '\0'}
	};

	char ch = '\0';

	while ((ch = getopt_long(argc, argv, "s:u:j:", long_options, NULL)) != -1) {

		switch (ch) {
			case 's':
				ms->system_prompt = calloc(1, strlen(optarg) + 1);
				memcpy(ms->system_prompt, optarg, strlen(optarg));
				break;
			case 'u':
				ms->user_prompt = calloc(1, strlen(optarg) + 1);
				memcpy(ms->user_prompt, optarg, strlen(optarg));
				break;
			case 'j':
				ms->json_file = calloc(1, strlen(optarg) + 1);
				memcpy(ms->json_file, optarg, strlen(optarg));
				break;
		}
	}
}

void *alloc_main_state(void)
{
	struct main_state *restrict ms = calloc(sizeof(*ms), 1);
	return ms;
}

void free_main_state(struct main_state *restrict ms)
{
	free(ms->user_prompt);
	ms->user_prompt = NULL;

	free(ms->system_prompt);
	ms->system_prompt = NULL;

	free(ms->json_file);
	ms->json_file = NULL;

	free(ms);
	ms = NULL;
}
