#include "send_request.h"

static char* contatinated_stream_text = NULL;

char* send_request(const char *restrict url, const char *restrict api_key, const char *restrict json_str, enum argument_detection *args_detect)
{
	char *result_string = NULL;

	// initialize curl
	CURL *hnd = NULL;
	struct curl_slist *slist1 = NULL;
	struct memory chunk = { .response = NULL, .size = 0 };

	curl_global_init(CURL_GLOBAL_DEFAULT);
	CURLcode ret = 0;
	slist1 = curl_slist_append(slist1, "Content-Type: application/json");

	// get api key
	char *bear = "Authorization: Bearer ";
	char *auth = calloc(strlen(bear) + strlen(api_key) + 1, 1);
	memcpy(auth, bear, strlen(bear));
	memcpy(auth + strlen(bear), api_key, strlen(api_key));//strcat(auth, api_key);

	// give the api key to curl and set easy options
	slist1 = curl_slist_append(slist1, auth);
	free(auth);
	auth = NULL;


	hnd = curl_easy_init();


	// decide which write_callback() function to use based on if the response will be sreamed or not
	size_t (*write_callback_ptr)(void *data, size_t size, size_t nmemb, void *userp);

	if (args_detect != NULL && args_detect[stream] == stream) {
		// if stream, then fn = write_callback_stream
		write_callback_ptr = write_callback_stream;
	} else {
		// else use normal write_callback
		write_callback_ptr = write_callback;
	}

	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_callback_ptr);
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
	curl_easy_setopt(hnd, CURLOPT_URL, url);
	curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, json_str);
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)strlen(json_str));
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
	curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.74.0");
	curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
	curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

	ret = curl_easy_perform(hnd);

	if (ret != CURLE_OK || chunk.size <= 0 || chunk.response == NULL) {
		goto cleanup;
	}
	if (args_detect != NULL && args_detect[stream] != stream) {
		result_string = calloc(chunk.size + 1, 1);
		memcpy(result_string, chunk.response, chunk.size);
	}
cleanup:

	free(chunk.response);
	chunk.response = NULL;
	chunk.size = 0;

	curl_slist_free_all(slist1);
	slist1 = NULL;

	curl_easy_cleanup(hnd);
	hnd = NULL;
	curl_global_cleanup();

	if (contatinated_stream_text != NULL && args_detect != NULL && args_detect[stream] == stream) {
		if (result_string != NULL) { free(result_string); result_string == NULL; }
//		return contatinated_stream_text;
		char *tmp = calloc(strlen(contatinated_stream_text) + 1, 1);
		memcpy(tmp, contatinated_stream_text, strlen(contatinated_stream_text));
		free(contatinated_stream_text);
		contatinated_stream_text = NULL;
		return tmp;
	} else {
		if (contatinated_stream_text != NULL) { free(contatinated_stream_text); contatinated_stream_text == NULL; }
		return result_string;
	}
	return NULL;
}

/* https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html */
size_t write_callback(void *data, size_t size, size_t nmemb, void *userp)
{

	size_t realsize = size * nmemb;
	struct memory *mem = (struct memory *)userp;
	char *ptr = realloc(mem->response, mem->size + realsize + 1);

	if(ptr == NULL) {
		return 0;
	}

	mem->response = ptr;
	memcpy(&(mem->response[mem->size]), data, realsize);
	mem->size += realsize;
	mem->response[mem->size] = '\0';

	return realsize;
}

size_t write_callback_stream(void *data, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;

	if (userp == NULL) { return realsize;}

//	char *str_data = data;
//	char *jsonl = NULL;
//
//	for (size_t i = 0; i < realsize; i++) {
//		if (str_data[i] == '\n') {
//			jsonl = calloc(1, realsize);
//			memcpy(jsonl, str_data, i - 1);
//		} else if (str_data[i] == '\0') {
//			break;
//		}
//	}

	struct memory *mem = (struct memory *)userp;
	char *ptr = realloc(mem->response, mem->size + realsize + 1);

	if(ptr == NULL) {
		return 0;
	}

	mem->response = ptr;
	memcpy(&(mem->response[mem->size]), data, realsize);
	mem->size += realsize;
	mem->response[mem->size] = '\0';


	// Hook here to print the jsonl immedietly

	jsonl_data json_lines = { .objects = NULL, .count = 0, .capacity = 0 };
	(void) init_jsonl_data(&json_lines);

	(void) process_jsonl_data((char*)data, &json_lines);

//	json_object *root = sanitize_and_parse_jsonl(jsonl);

//	if (root == NULL) { goto cleanup; }

//	for (size_t i = 0; i < jd->count; i++) {
	for (size_t i = 0; i < json_lines.count; i++) {
		json_object *choices = json_object_object_get(json_lines.objects[i], "choices");
		if (json_object_get_type(choices) != json_type_array) { goto cleanup; }

		json_object *first_choice = json_object_array_get_idx(choices, 0);
		if (json_object_get_type(first_choice) != json_type_object) { goto cleanup; }

		json_object *finish_reason = json_object_object_get(first_choice, "finish_reason");
		if (json_object_get_type(finish_reason) == json_type_string && !strcmp(json_object_get_string(finish_reason),"stop") ) { goto cleanup; }

		json_object *delta = json_object_object_get(first_choice, "delta");
		if (json_object_get_type(delta) != json_type_object) { goto cleanup; }

		json_object *content = json_object_object_get(delta, "content");
		if (json_object_get_type(content) != json_type_string) { goto cleanup; }

		const char * const val = json_object_get_string(content);


		if (val != NULL) {
			/* HERE */
			if (contatinated_stream_text == NULL) {
				contatinated_stream_text = calloc(strlen(val) + 1, 1);
				memcpy(contatinated_stream_text, val, strlen(val));
			} else {
				char *tmp = calloc(strlen(contatinated_stream_text) + strlen(val) + 1, 1);
				memcpy(tmp, contatinated_stream_text, strlen(contatinated_stream_text));
				memcpy(tmp + strlen(contatinated_stream_text), val, strlen(val));
				free(contatinated_stream_text);
				contatinated_stream_text = tmp;
				tmp = NULL;
			}
//		printf("%s\n", contatinated_stream_text);
//			add_text_prompt(ms->root, "assistant", val);
//			if (ms->json_file != NULL) {
//				json_object_to_file(ms->json_file, ms->root);
//			}
			/* Here */

			if (!is_terminal(stdout)) {
				printf("%s", val);
			} else {
				printf("%s%s%s", "\033[1;35m", val, "\033[0m");
			}
		} else {
			goto cleanup;
		}
	}

cleanup:
//	if (jsonl != NULL) { free(jsonl); }

	free_jsonl_data(&json_lines);
//	json_object_put(root);
	return realsize;
}
