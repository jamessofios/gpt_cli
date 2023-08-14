// https://curl.se/libcurl/c/curl_easy_setopt.html
// https://help.openai.com/en/articles/7042661-chatgpt-api-transition-guide
// https://ai.stackexchange.com/questions/39837/meaning-of-roles-in-the-api-of-gpt-4-chatgpt-system-user-assistant
// https://stackoverflow.com/questions/3840582/still-reachable-leak-detected-by-valgrind

#include "construct_json.h"
#include "send_request.h"
#include <json-c/json.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
// https://www.thegeekstuff.com/2010/10/linux-error-codes/
#include <errno.h>

int main(int argc, char **argv)
{
	const char *api_key = getenv("OPENAI_API_KEY");

	if (api_key == NULL) {
		errno = ENOKEY;
		perror("No API key found. Please set the environment variable OPENAI_API_KEY.");
		goto cleanup;
	}

	json_object *root = NULL;
	char *save_file = NULL;

	if(isatty(0)) {

		int opt = 0;
		char *sys_prompt = NULL;
		char *user_prompt = NULL;

		while ((opt = getopt(argc, argv, "m:t:s:u:j:r")) != -1) {

			switch (opt) {
			case 'm':
				set_model(root, optarg);
				break;
			case 't':
				set_temp(root, atoi(optarg));
				break;
			case 's':
				sys_prompt = calloc(strlen(optarg) + 1, 1);
				if (sys_prompt == NULL) {
					errno = ENOMEM;
					perror("Could not allocate memory for sys_prompt");
					goto cleanup;
				}
				memcpy(sys_prompt, optarg, strlen(optarg));
				break;
			case 'u':
				user_prompt = calloc(strlen(optarg) + 1, 1);
				if (user_prompt == NULL) {
					errno = ENOMEM;
					perror("Could not allocate memory for user_prompt");
					goto cleanup;
				}
				memcpy(user_prompt, optarg, strlen(optarg));
				break;
			case 'r':
//				TODO: repl loop goes here
				puts("Frosty!");
				break;
			case 'j':
				save_file = calloc(strlen(optarg) + 1, 1);
				memcpy(save_file, optarg, (strlen(optarg) + 1) );
				root = json_object_from_file(save_file);

				if (root == NULL) { root = new_chatgpt(); }
				break;
			case '?':
				errno = EINVAL;
				perror("Please provide a valid argument");
				printf("Usage: %s [-m model] [-t temperature] [-s system-prompt] [-u user-prompt]\n", argv[0]);
				goto cleanup;
				break;
			default:
				errno = EINVAL;
				perror("Please provide a valid argument");
				printf("Usage: %s [-m model] [-t temperature] [-s system-prompt] [-u user-prompt]\n", argv[0]);
				goto cleanup;
				break;
			}
		}

		if (root == NULL) {
			root = new_chatgpt();
		}

		if (sys_prompt != NULL && user_prompt != NULL) {

			add_text_prompt(root, "system", sys_prompt);
			free(sys_prompt);

			add_text_prompt(root, "user", user_prompt);
			free(user_prompt);

		} else if (user_prompt != NULL) {

			add_text_prompt(root, "user", user_prompt);
			free(user_prompt);

		} else {
			errno = EINVAL;
			perror("Please provide a valid argument");
			printf("Usage: %s [-m model] [-t temperature] [-s system-prompt] [-u user-prompt] [-j json-file]\n", argv[0]);
			free(sys_prompt);
			free(user_prompt);
			goto cleanup;
		}

	} else {

		if (root == NULL) {
			root = new_chatgpt();
		}

		char c = '\0';
		char *s = NULL;

		for (int i = 1;; i++) {
			c = getchar();
			if (c == EOF || feof(stdin)) {
				break;
			}
			s = realloc(s, i + 1);
			s[i - 1] = c;
			s[i] = '\0';
		}
		add_text_prompt(root, "user", s);
		free(s);
	}

	char *result_string = send_request("https://api.openai.com/v1/chat/completions",
					api_key,
					json_object_to_json_string(root));

	if (result_string == NULL) {
		errno = ENOMEM;
		perror("Curl status was not ok");
		goto cleanup;
	}

	json_object *result_json = json_tokener_parse(result_string);

//	print the replay as full json object
//	printf("%s\n",json_object_to_json_string_ext(result_json, JSON_C_TO_STRING_PRETTY));

	json_object *assist_message = json_object_object_get(json_object_array_get_idx(json_object_object_get(result_json, "choices"), 0), "message");

	json_object *text = json_object_object_get(assist_message, "content");

	add_text_prompt(root, "assistant", json_object_get_string(text));

	if (save_file != NULL) {
		json_object_to_file(save_file, root);
	}

	puts(json_object_get_string(json_object_object_get(assist_message, "content")));

	free(result_string);
	result_string = NULL;

	json_object_put(result_json);
	result_json = NULL;

	cleanup:

	json_object_put(root);
	root = NULL;

	free(save_file);
	save_file = NULL;

	return errno;
}
