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

struct chat_message {
	char *role;
	char *message;
};

struct main_program_state {
	const char *restrict api_key;
	char *restrict model;
	struct chat_message *chat_messages;
	float temperature;
	bool stream;
};
