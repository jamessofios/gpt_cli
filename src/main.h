#pragma once
#include <json-c/json.h>

struct main_state {
	char *user_prompt;
	char *system_prompt;
	char *json_file;
	const char *api_key;
	json_object *root;
};
