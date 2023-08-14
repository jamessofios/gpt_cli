#include "construct_json.h"

void add_text_prompt(json_object *input, const char *role, const char *prompt_text)
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
	prompt_json = NULL;
}

void set_temp(json_object *input, float temp)
{
	json_object_object_add(input, "temperature", json_object_new_double(temp));
}

void set_model(json_object *input, char* model)
{
	json_object_object_add(input, "model", json_object_new_string(model));
}

json_object *new_chatgpt(void)
{
	json_object *main_obj = json_object_new_object();

	json_object_object_add(main_obj, "model", json_object_new_string("gpt-3.5-turbo"));
//	json_object_object_add(main_obj, "temperature", json_object_new_int(1));
	json_object_object_add(main_obj, "messages", json_object_new_array());

	return main_obj;
}

// ownership of src goes to dest. dest is the only thing you have to free using json_object_put()
//void add_json_prompt(json_object *dest, json_object *src)
//{
//	json_object *message_array = json_object_object_get(dest, "messages");
//
//	if (message_array == NULL) {
//		message_array = json_object_new_array();
//	}
//
//	json_object_array_add(message_array, src);
//	src = NULL;
//}
