#include "parse_options.h"

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

enum argument_detection *parse_options(const int argc, char **restrict argv, struct main_state *restrict ms) {

	if (argc == 1) {
		errno = EINVAL;
		return NULL;
	}

	static struct option long_options[] = {
		{"user_prompt", required_argument, NULL, 'u'},
		{"system_prompt", required_argument, NULL, 's'},
		{"json_file", required_argument, NULL, 'j'},
		{"temperature", required_argument, NULL, 't'},
		{"model", required_argument, NULL, 'm'},
		{"help", no_argument, NULL, 'h'},
		{"repl", no_argument, NULL, 'r'},
		//the last entry in the array must be all zeros for getopt_long to know where the array ends
		{NULL, 0, NULL, '\0'}
	};

	enum argument_detection *args_detect = malloc(COUNT_OF(long_options) * sizeof(*args_detect));

	if (args_detect == NULL) { return NULL; }

	memset(args_detect, (signed int)-1, COUNT_OF(long_options));

	// Specifing signed char because clang on termux defaults to unsigned char
	signed char choice = '\0';

	float temp = -1.0;
	char *ai_model = NULL;

	while ((choice = getopt_long(argc, argv, "s:u:j:t:m:hr", long_options, NULL)) != -1) {

		switch (choice) {
			case 's':
				args_detect[system_prompt] = system_prompt;

				ms->system_prompt = calloc(1, strlen(optarg) + 1);

				if (ms->system_prompt == NULL) {
					break;
				}

				memcpy(ms->system_prompt, optarg, strlen(optarg));
				break;

			case 'u':
				args_detect[user_prompt] = user_prompt;

				ms->user_prompt = calloc(1, strlen(optarg) + 1);
				if (ms->user_prompt == NULL) {
					break;
				}

				memcpy(ms->user_prompt, optarg, strlen(optarg));
				break;

			case 'j':

				args_detect[json_file] = json_file;

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
				args_detect[temperature] = temperature;

				temp = atof(optarg);
				break;
			case 'm':
				args_detect[model] = model;

				ai_model = calloc(1, strlen(optarg) + 1);
				if (ai_model == NULL) {
					break;
				}

				memcpy(ai_model, optarg, strlen(optarg));

				break;
			case 'h':
				args_detect[help] = help;
				printf(help_text, argv[0]);
				break;
			case 'r':
				args_detect[repl] = repl;
				break;
			default:
				errno = EINVAL;
				perror("Please provide a valid argument");
				break;
		}
	}

	if (ms->root == NULL) { ms->root = new_chatgpt(); }
	if (ai_model != NULL) { set_model(ms->root, ai_model); free(ai_model); ai_model = NULL; }

	// check if temp is valid. Otherwise it will default to 1
	if (temp >= 0.0 && temp <= 2.0 ) { set_temp(ms->root, temp); }
	if (ms->system_prompt != NULL) { add_text_prompt(ms->root, "system", ms->system_prompt); }

	if (ms->user_prompt != NULL) {
		add_text_prompt(ms->root, "user", ms->user_prompt);
	} else {
		errno = EINVAL;
	}

	return args_detect;
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
