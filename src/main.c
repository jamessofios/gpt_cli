#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <json-c/json.h>
#include "global.h"
#include "construct_json.h"
#include "parse_jsonl.h"
#include "send_request.h"
#include "parse_options.h"

int main(int argc, char **argv)
{
	struct main_state *restrict ms = alloc_main_state();
	enum argument_detection *args_detect = NULL;


	#ifdef DEBUG_ASSERTS
	assert(ms != NULL);
	#else
	if (ms == NULL) {
		errno = ENOMEM;
		perror("Could not allocate state");
		goto cleanup;
	}
	#endif

	ms->api_key = getenv("OPENAI_API_KEY");

	if (ms->api_key == NULL) {
		errno = 1;
		perror("No API key found. Please get an API key from OpenAI and set the environment variable OPENAI_API_KEY");
		goto cleanup;
	}

	args_detect = parse_options(argc, argv, ms);

	if (args_detect == NULL && is_terminal(stdin)) { goto cleanup; }

//	if (args_detect[repl] == repl && args_detect[user_prompt] == user_prompt) { args_detect[repl] = arg_not_found; }


do {

	if(is_terminal(stdin) && args_detect != NULL && args_detect[repl] != repl) {

		if (errno == EINVAL) {
			goto cleanup;
		}

	} else if (!is_terminal(stdin) || (args_detect != NULL && args_detect[repl] == repl)) {

		if (ms->root == NULL) {
			ms->root = new_chatgpt();
		}

		signed char c = '\0';
		char *s = NULL;


		if (args_detect != NULL && args_detect[repl] == repl) {
			printf("\033[1;36m%s>\033[0m ", json_object_get_string(json_object_object_get(ms->root, "model")));
		}


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
			if (ms->user_prompt == NULL) {
//				This would just cache the last user prompt in repl mode, but store the user prompt when unput is a pipe from stdin
				ms->user_prompt = s;
			} else {
				free(s);
			}
		} else {
//			errno = EINVAL;
//			perror("You passed an empty string through stdin");
			errno = 0;
			goto cleanup;
		}
	}

//	char *result_string = send_request("https://api.openai.com/v1/chat/completions",
//					ms->api_key,
//					json_object_to_json_string(ms->root), args_detect);

	char *result_string = send_request("http://192.168.1.3:11434/v1/chat/completions",
					ms->api_key,
					json_object_to_json_string(ms->root), args_detect);

	if (result_string == NULL) {
		errno = ENETUNREACH;
		perror("Curl status was not OK or it returned NULL");
		goto cleanup;
	}

	if (args_detect != NULL && args_detect[stream] == stream) {
		add_text_prompt(ms->root, "assistant", result_string);
		if (ms != NULL && ms->json_file != NULL) {
			json_object_to_file(ms->json_file, ms->root);
		}
		//HERE
		// Need to free the result_string here if the write_callback_stream function does not get called again in repl mode
		// because it will get freed in that function, as we use it again there
//		if (args_detect != NULL && args_detect[repl] != repl) {
			free(result_string);
			result_string = NULL;
//		}
		putchar('\n');
		errno = 0;
//		jsonl_data jd;
//		init_jsonl_data(&jd);
//		int count = process_jsonl_data(result_string, &jd);
//
//		for (size_t i = 0; i < jd.count; i++) {
//			json_object *as = json_object_object_get(json_object_array_get_idx(json_object_object_get(jd.objects[i], "choices"), 0), "delta");
//
//
//			const char *val = json_object_get_string(json_object_object_get(as, "content"));
//
//			if (val != NULL) {
//				printf("%s", val);
//			}
//		}
//		printf("\n");
//		free_jsonl_data(&jd);
	} else {

		json_object *result_json = json_tokener_parse(result_string);

		if (json_object_object_get(result_json, "error") != NULL) {
			errno = 1;
			fprintf(stderr, "Error: %s\n", json_object_get_string(json_object_object_get(json_object_object_get(result_json, "error"), "message")));

//			fprintf(stderr, "%s\n", json_object_to_json_string_ext(result_json, JSON_C_TO_STRING_PRETTY));
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


	}

} while (args_detect != NULL && args_detect[repl] == repl);

cleanup:

	free(args_detect);
	free_main_state(ms);
	return errno;
}
