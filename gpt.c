// https://curl.se/libcurl/c/curl_easy_setopt.html
// https://help.openai.com/en/articles/7042661-chatgpt-api-transition-guide
// https://ai.stackexchange.com/questions/39837/meaning-of-roles-in-the-api-of-gpt-4-chatgpt-system-user-assistant
// https://stackoverflow.com/questions/3840582/still-reachable-leak-detected-by-valgrind

#include <curl/curl.h>
#include <json-c/json.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct response {
	char *memory;
	size_t size;
};

/* https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html */
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct response *mem = (struct response*)userp;
	char *ptr = realloc(mem->memory, mem->size + realsize + 1);
	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;
	return realsize;
}

int main(int argc, char **argv)
{
	char *api_key = getenv("OPENAI_API_KEY");
	if (api_key == NULL) {
		fprintf(stderr, "%s\n", "Error: No API key found.");
		exit(1);
	}
	char *input = "{\"model\":\"gpt-3.5-turbo\",\"messages\":[{\"role\":\"system\",\"content\":\"Hello\"},{\"role\":\"user\",\"content\":\"Hello\"}],\"temperature\":1}";

	curl_global_init(CURL_GLOBAL_DEFAULT);
	struct response chunk = { .memory = NULL, .size = 0 };

	json_object *root = json_tokener_parse(input);
	CURLcode ret = 0;
	CURL *hnd = NULL;
	struct curl_slist *slist1 = NULL;

	slist1 = NULL;
	slist1 = curl_slist_append(slist1, "Content-Type: application/json");

//	get api key
	char *bear = "Authorization: Bearer ";
	char *auth = calloc(1, strlen(bear) + strlen(api_key) + 1);
	strncpy(auth, bear, strlen(bear));
	strncat(auth, api_key, strlen(api_key));

	slist1 = curl_slist_append(slist1, auth);
	free(auth);
	auth = NULL;

	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void*)&chunk);
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

	if (ret != CURLE_OK) {
		goto cleanup;
	}

	char *s = calloc(1, chunk.size + 1);
	strncpy(s, chunk.memory, chunk.size);

	json_object *result = json_tokener_parse(s);

	printf("%s\n", json_object_to_json_string(result));

cleanup:
	free(s);
	s = NULL;

	free(chunk.memory);
	chunk.memory = NULL;
	chunk.size = 0;

	curl_easy_cleanup(hnd);
	hnd = NULL;

	curl_slist_free_all(slist1);
	slist1 = NULL;

	curl_global_cleanup();

	json_object_put(root);
	root = NULL;

	json_object_put(result);
	result = NULL;

	return (int)ret;
}
