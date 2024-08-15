// https://curl.se/libcurl/c/curl_easy_setopt.html
// https://help.openai.com/en/articles/7042661-chatgpt-api-transition-guide
// https://ai.stackexchange.com/questions/39837/meaning-of-roles-in-the-api-of-gpt-4-chatgpt-system-user-assistant
// https://stackoverflow.com/questions/3840582/still-reachable-leak-detected-by-valgrind

#include "main.h"
#include "construct_json.h"
#include "send_request.h"
#include "parse_options.h"
#include <json-c/json.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
// https://www.thegeekstuff.com/2010/10/linux-error-codes/
#include <errno.h>
#include <assert.h>

int main(int argc, char **argv)
{
	struct main_state *ms = alloc_main_state();
	enum argument_detection *args_detect = NULL;


	if (ms == NULL) {
		errno = ENOMEM;
		perror("Could not allocate state");
		goto cleanup;
	}

	ms->api_key = getenv("OPENAI_API_KEY");

	if (ms->api_key == NULL) {
		errno = ENOKEY;
		perror("No API key found. Please get an API key from OpenAI and set the environment variable OPENAI_API_KEY");
		goto cleanup;
	}

	args_detect = parse_options(argc, argv, ms);

	if (args_detect == NULL && isatty(STDIN_FILENO)) { goto cleanup; }

do {

	if(isatty(STDIN_FILENO) && args_detect[repl] != repl) {

		if (errno == EINVAL) {
			goto cleanup;
		}

	} else if (!isatty(STDIN_FILENO) || args_detect[repl] == repl) {

		if (ms->root == NULL) {
			ms->root = new_chatgpt();
		}

		signed char c = '\0';
		char *s = NULL;


		if (args_detect != NULL && args_detect[repl] == repl) { printf("> "); }


		for (int i = 1;; i++) {
			c = getchar();

			if (c == EOF || feof(stdin) || c == '\0') { break; }
			if (args_detect != NULL && args_detect[repl] == repl && c == '\n') { break; }

			s = realloc(s, i + 1);
			s[i - 1] = c;
			s[i] = '\0';
		}

		if (s != NULL) {
			if (args_detect != NULL && args_detect[repl] == repl && !strcmp(s, "exit")) { free(s); goto cleanup; }
			add_text_prompt(ms->root, "user", s);
			free(s);
		} else {
//			errno = EINVAL;
//			perror("You passed an empty string through stdin");
			errno = 0;
			goto cleanup;
		}
	}

	char *result_string = send_request("https://api.openai.com/v1/chat/completions",
					ms->api_key,
					json_object_to_json_string(ms->root));

	if (result_string == NULL) {
		errno = ENETUNREACH;
		perror("Curl status was not OK or it returned NULL");
		goto cleanup;
	}

	json_object *result_json = json_tokener_parse(result_string);

	if (json_object_object_get(result_json, "error") != NULL) {
		errno = EBADR;
		fprintf(stderr, "%s\n", json_object_to_json_string_ext(result_json, JSON_C_TO_STRING_PRETTY));
		free(result_string);
		json_object_put(result_json);
		goto cleanup;
	}

	json_object *assist_message = json_object_object_get(json_object_array_get_idx(json_object_object_get(result_json, "choices"), 0), "message");

	json_object *text = json_object_object_get(assist_message, "content");

	add_text_prompt(ms->root, "assistant", json_object_get_string(text));

	if (ms->json_file != NULL) {
		json_object_to_file(ms->json_file, ms->root);
	}

	puts(json_object_get_string(json_object_object_get(assist_message, "content")));

	free(result_string);
	result_string = NULL;

	json_object_put(result_json);
	result_json = NULL;

} while (args_detect != NULL && args_detect[repl] == repl);

cleanup:

	free(args_detect);
	free_main_state(ms);

	return errno;
}
