// https://curl.se/libcurl/c/curl_easy_setopt.html
// https://help.openai.com/en/articles/7042661-chatgpt-api-transition-guide
// https://ai.stackexchange.com/questions/39837/meaning-of-roles-in-the-api-of-gpt-4-chatgpt-system-user-assistant
// https://stackoverflow.com/questions/3840582/still-reachable-leak-detected-by-valgrind

//chat json data is saved in $HOME/.chatgpt.json

#include <curl/curl.h>
#include <json-c/json.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "construct_json.h"

struct memory {
	char *response;
	size_t size;
};

/* https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html */
size_t write_callback(void *data, size_t size, size_t nmemb, void *userp)
{

	size_t realsize = size * nmemb;
	struct memory *mem = (struct memory *)userp;
	char *ptr = realloc(mem->response, mem->size + realsize + 1);

	if(ptr == NULL) {
		return 0;  /* out of memory! */
	}

	mem->response = ptr;
	memcpy(&(mem->response[mem->size]), data, realsize);
	mem->size += realsize;
	mem->response[mem->size] = 0;
	return realsize;
}

int main(int argc, char **argv)
{
	char *api_key = getenv("OPENAI_API_KEY");
	char *save_file = calloc(strlen(getenv("HOME")) + strlen("/.chatgpt.json") + 1, 1);
	CURL *hnd = NULL;
	struct curl_slist *slist1 = NULL;

	if (save_file == NULL) {
		fprintf(stderr, "%s\n", "Could not allocate space for filename.");
		goto cleanup;
	}

	strncpy(save_file, getenv("HOME"), strlen(getenv("HOME")));
	strcat(save_file, "/.chatgpt.json");

	json_object *root = json_object_from_file(save_file);

	if (api_key == NULL) {
		fprintf(stderr, "%s\n", "Error: No API key found. Please set the environment variable OPENAI_API_KEY.");
		goto cleanup;
	}

	if (root == NULL) {
		root = new_chatgpt();
	}

	if (argc == 1) {
		fprintf(stderr, "%s\n\n", "Error: please provide an argument");
		fprintf(stderr, "Information:\nUsing API key: %s\nSaving chat history in: %s\n", api_key, save_file);
		goto cleanup;
	}


	if(isatty(0)) {
		int opt;

		char *sys_prompt = NULL;
		char *user_prompt = NULL;

		while ((opt = getopt(argc, argv, "m:t:s:u:")) != -1) {
			switch (opt) {
			case 'm':
				set_model(root, optarg);
				break;
			case 't':
				set_temp(root, atoi(optarg));
				break;
			case 's':
//				add_text_prompt(root, "system", optarg);
				sys_prompt = calloc(strlen(optarg) + 1, 1);
				if (sys_prompt == NULL) {
					goto cleanup;
				}
				memcpy(sys_prompt, optarg, strlen(optarg));
				break;
			case 'u':
//				add_text_prompt(root, "user", optarg);
				user_prompt = calloc(strlen(optarg) + 1, 1);
				if (user_prompt == NULL) {
					goto cleanup;
				}
				memcpy(user_prompt, optarg, strlen(optarg));
				break;
			default:
				printf("Usage: %s [-m model] [-t temperature] [-s system-prompt] [-u user-prompt]\n", argv[0]);
				goto cleanup;
			}
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
			free(sys_prompt);
			free(user_prompt);
			goto cleanup;
		}

	} else {
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

	struct memory chunk = { .response = NULL, .size = 0 };

	// initialize curl
	curl_global_init(CURL_GLOBAL_DEFAULT);
	CURLcode ret = 0;
	slist1 = curl_slist_append(slist1, "Content-Type: application/json");

	// get api key
	char *bear = "Authorization: Bearer ";
	char *auth = calloc(strlen(bear) + strlen(api_key) + 1, 1);
	strncpy(auth, bear, strlen(bear));
	strcat(auth, api_key);

	// give the api key to curl and set easy options
	slist1 = curl_slist_append(slist1, auth);
	free(auth);
	auth = NULL;

	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
	curl_easy_setopt(hnd, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
	curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, json_object_to_json_string(root));
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)strlen(json_object_to_json_string(root)));
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
	curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.74.0");
	curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
	curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

	ret = curl_easy_perform(hnd);

	if (ret != CURLE_OK || chunk.size <= 0 || chunk.response == NULL) {
		fprintf(stderr, "%s\n", "Error: curl status was not ok");
		goto cleanup;
	}

	char *result_string = calloc(chunk.size + 1, 1);
	memcpy(result_string, chunk.response, chunk.size);

	json_object *result_json = json_tokener_parse(result_string);

//	print the replay as full json object
//	printf("%s\n",json_object_to_json_string_ext(result_json, JSON_C_TO_STRING_PRETTY));

	json_object *assist_message = json_object_object_get(json_object_array_get_idx(json_object_object_get(result_json, "choices"), 0), "message");

	json_object *text = json_object_object_get(assist_message, "content");

	add_text_prompt(root, "assistant", json_object_get_string(text));
	json_object_to_file(save_file, root);

	puts(json_object_get_string(json_object_object_get(assist_message, "content")));

	free(result_string);
	result_string = NULL;

	json_object_put(result_json);
	result_json = NULL;

	free(chunk.response);
	chunk.response = NULL;
	chunk.size = 0;

	cleanup:

	curl_easy_cleanup(hnd);
	hnd = NULL;

	curl_slist_free_all(slist1);
	slist1 = NULL;

	json_object_put(root);
	root = NULL;

	free(save_file);
	save_file = NULL;

	curl_global_cleanup();

	return 0;
}
