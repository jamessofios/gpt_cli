#pragma once
#include <stdio.h>
#include <json-c/json.h>

// load file: json_object *root = json_object_from_file("tmp.json");
// save file: json_object_to_file("tmp2.json", root);

/*
https://stackoverflow.com/questions/36998026/append-json-object-arrays-in-c-using-jsonc-library
*/

#define free_chatgpt(chat) json_object_put(chat)

void add_text_prompt(json_object *input, const char *role, const char *prompt_text);
void set_temp(json_object *input, float temp);
void set_model(json_object *input, char* model);
json_object *new_chatgpt(void);
//void add_json_prompt(json_object *dest, json_object *src);
