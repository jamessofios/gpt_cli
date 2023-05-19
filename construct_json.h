#pragma once
#include <stdio.h>
#include <json-c/json.h>

// load file: json_object *root = json_object_from_file("tmp.json");
// save file: json_object_to_file("tmp2.json", root);

/*
https://stackoverflow.com/questions/36998026/append-json-object-arrays-in-c-using-jsonc-library
*/

void add_text_prompt(json_object *input, char *role, char* prompt_text)
{
	json_object *prompt_json = json_object_new_object();
	json_object_object_add(prompt_json, "role", json_object_new_string(role));
	json_object_object_add(prompt_json, "content", json_object_new_string(prompt_text));

	json_object *message_array = json_object_object_get(input, "messages");

	if (message_array == NULL) {
		message_array = json_object_new_array();
		json_object_object_add(input, "role", json_object_new_string(role));
	}

	json_object_array_add(message_array, prompt_json);
}

void add_json_prompt(json_object *dest, json_object *src)
{
	json_object *message_array = json_object_object_get(dest, "messages");

	if (message_array == NULL) {
		message_array = json_object_new_array();
//		json_object_object_add(input, "role", json_object_new_string(role));
	}

	json_object_array_add(message_array, src);
}

void set_temp(json_object *input, int temp)
{
	json_object_object_add(input, "temperature", json_object_new_int(temp));
}

void set_model(json_object *input, char* model)
{
	json_object_object_add(input, "model", json_object_new_string(model));
}

json_object *new_chatgpt(void)
{
	json_object *main_obj       = json_object_new_object();
	json_object *message_array  = json_object_new_array();

	json_object_object_add(main_obj, "model", json_object_new_string("gpt-3.5-turbo"));
	json_object_object_add(main_obj, "temperature", json_object_new_int(1));
	json_object_object_add(main_obj, "messages", message_array);

	return main_obj;
}

#define free_chatgpt(chat) json_object_put(chat)

//int main(void) {
//
//	json_object *chat = new_chatgpt();
//
//	add_text_prompt(chat, "system", "you are a man");
//	add_text_prompt(chat, "user", "what are you");
//	add_text_prompt(chat, "assistant", "a man");
//	printf("%s\n", json_object_to_json_string(chat));
//
//	free_chatgpt(chat);
//	return 0;
//}
