#include <stdio.h>
#include <json-c/json.h>

/*
https://stackoverflow.com/questions/36998026/append-json-object-arrays-in-c-using-jsonc-library
*/

int main(void) {

	json_object *main_obj = json_object_new_object();
	json_object *message_array = json_object_new_array();
	json_object *sys_prompt = json_object_new_object();
	json_object *user_prompt = json_object_new_object();

	json_object_object_add(main_obj, "model", json_object_new_string("gpt-3.5-turbo"));

	json_object_object_add(main_obj, "temperature", json_object_new_int(1));

	json_object_object_add(sys_prompt, "role", json_object_new_string("system"));
	json_object_object_add(sys_prompt, "content", json_object_new_string("Hello"));

	// Add the object to the array
	json_object_array_add(message_array, sys_prompt);

	json_object_object_add(user_prompt, "role", json_object_new_string("user"));
	json_object_object_add(user_prompt, "content", json_object_new_string("Hello"));

	json_object_array_add(message_array, user_prompt);

	// Add the array to the main object
	json_object_object_add(main_obj, "messages", message_array);


	printf("%s\n", json_object_to_json_string(main_obj));

	// Release memory
	json_object_put(main_obj);

	return 0;
}
