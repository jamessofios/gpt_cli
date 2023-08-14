#include "parse_options.h"

void parse_options(const int argc, char **restrict argv, struct main_state *restrict ms) {

	static struct option long_options[] = {
		{"user_prompt", required_argument, NULL, 'u'},
		{"system_prompt", required_argument, NULL, 's'},
		{"json_file", required_argument, NULL, 'j'},
		{"temperature", required_argument, NULL, 't'},
		{"model", required_argument, NULL, 'm'},
		//the last entry in the array must be all zeros for getopt_long to know where the array ends
		{NULL, 0, NULL, '\0'}
	};

	// Specifing signed char because clang on termux defaults to unsigned char
	signed char ch = '\0';

	float temp = -1.0;
	char *model = NULL;

	while ((ch = getopt_long(argc, argv, "s:u:j:t:m:", long_options, NULL)) != -1) {

		switch (ch) {
			case 's':
				ms->system_prompt = calloc(1, strlen(optarg) + 1);

				if (ms->system_prompt == NULL) {
					break;
				}

				memcpy(ms->system_prompt, optarg, strlen(optarg));
				break;

			case 'u':
				ms->user_prompt = calloc(1, strlen(optarg) + 1);
				if (ms->user_prompt == NULL) {
					break;
				}

				memcpy(ms->user_prompt, optarg, strlen(optarg));
				break;

			case 'j':
				ms->json_file = calloc(1, strlen(optarg) + 1);
				if (ms->json_file == NULL) {
					break;
				}

				memcpy(ms->json_file, optarg, strlen(optarg));

				// try to read json from an existing file
				ms->root = json_object_from_file(ms->json_file);

				// if the file does not exists,
				// then make a new json that can later be saved

				break;
			case 't':
				temp = atof(optarg);
				break;
			case 'm':
				model = calloc(1, strlen(optarg) + 1);
				if (model == NULL) {
					break;
				}

				memcpy(model, optarg, strlen(optarg));

				break;
			default:
				errno = EINVAL;
				perror("Please provide a valid argument");
				printf("Usage: %s [-m --model] [-t --temperature] [-s --system_prompt] [-u --user_prompt] [-j --json_file]\n", argv[0]);
				break;
		}
	}

	if (ms->root == NULL) { ms->root = new_chatgpt(); }
	if (model != NULL) { set_model(ms->root, model); }

	// check if temp is valid. Otherwise it will default to 1
	if (temp >= 0.0 && temp <= 2.0 ) { set_temp(ms->root, temp); }
	if (ms->system_prompt != NULL) { add_text_prompt(ms->root, "system", ms->system_prompt); }

	if (ms->user_prompt != NULL) {
		add_text_prompt(ms->root, "user", ms->user_prompt);
	} else {
		errno = EINVAL;
		perror("Please provide a valid argument");
		printf("Usage: %s [-m --model] [-t --temperature] [-s --system_prompt] [-u --user_prompt] [-j --json_file]\n", argv[0]);
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

	json_object_put(ms->root);
	ms->root = NULL;

	free(ms);
	ms = NULL;
}
