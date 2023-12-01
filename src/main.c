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

	#ifdef DEBUG_ASSERTS
		assert(ms != NULL);
	#endif

	if (ms == NULL) {
		errno = ENOMEM;
		perror("Could not allocate state");
		goto cleanup;
	}

	ms->api_key = getenv("OPENAI_API_KEY");

	#ifdef DEBUG_ASSERTS
		assert(ms->api_key != NULL);
	#endif

	if (ms->api_key == NULL) {
		errno = ENOKEY;
		perror("No API key found. Please get an API key from OpenAI and set the environment variable OPENAI_API_KEY");
		goto cleanup;
	}

	if(isatty(0)) {

		parse_options(argc, argv, ms);
		if (errno == EINVAL) {
			goto cleanup;
		}
	} else {

		if (ms->root == NULL) {
			ms->root = new_chatgpt();
		}

		signed char c = '\0';
		char *s = NULL;

		for (int i = 1;; i++) {
			c = getchar();
			if (c == EOF || feof(stdin) || c == '\0') {
				break;
			}
			s = realloc(s, i + 1);
			s[i - 1] = c;
			s[i] = '\0';
		}

		#ifdef DEBUG_ASSERTS
			assert(s != NULL);
		#endif

		if (s != NULL) {
			add_text_prompt(ms->root, "user", s);
			free(s);
		} else {
			errno = EINVAL;
			perror("You passed an empty string through stdin");
			goto cleanup;
		}
	}

	char *result_string = send_request("https://api.openai.com/v1/chat/completions",
					ms->api_key,
					json_object_to_json_string(ms->root));

	#ifdef DEBUG_ASSERTS
		assert(result_string != NULL);
	#endif

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

cleanup:

	free_main_state(ms);

	return errno;
}
