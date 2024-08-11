#pragma once
#include <stdbool.h>
#include <json-c/json.h>

struct main_state {
	char *user_prompt;
	char *system_prompt;
	char *json_file;
	const char *api_key;
	json_object *root;
};

struct main_program_state {
	const char *restrict openai_api_key;
	const char *restrict anthropic_api_key;

	char *model;
	float temperature;
	char **messages;
	bool stream;

};
