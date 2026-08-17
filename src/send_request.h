#pragma once
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include "parse_options.h"
#include "construct_json.h"
#include "parse_jsonl.h"
#include "global.h"

// Memory struct is only used for curl callback
struct memory {
	char *response;
	size_t size;
};

// Callback funciton for curl
size_t write_callback(void *data, size_t size, size_t nmemb, void *userp);
size_t write_callback_stream(void *data, size_t size, size_t nmemb, void *userp);

// You must free the string that is returned
char* send_request(const char *restrict url, const char *restrict api_key, const char *restrict json_str, enum argument_detection *args_detect);
