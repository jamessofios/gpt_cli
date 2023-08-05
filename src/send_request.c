#include "send_request.h"

char* send_request(const char *restrict url, const char *restrict api_key, const char *restrict json_str)
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
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_callback);
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
		// Curl status was not ok
		goto cleanup;
	}

	result_string = calloc(chunk.size + 1, 1);
	memcpy(result_string, chunk.response, chunk.size);

cleanup:
	free(chunk.response);
	chunk.response = NULL;
	chunk.size = 0;

	curl_easy_cleanup(hnd);
	hnd = NULL;

	curl_slist_free_all(slist1);
	slist1 = NULL;

	curl_global_cleanup();

	return result_string;
}

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
