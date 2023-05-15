/*
https://jsonlint.com/
https://github.com/rbtylee/tutorial-jsonc/blob/master/tutorial/legacy.md
*/
#include <stdio.h>
#include <json-c/json.h>

int main(void)
{
//	json_object *root = json_object_from_file("chat.json");
//	if (root == NULL)
//		return 1;
//	json_object *model = json_object_object_get(root, "model");
//	json_object *temperature = json_object_object_get(root, "temperature");
//
//	printf("%s, %s\n", json_object_get_string(model), json_object_get_string(temperature));

	char *input = "{\"model\":\"gpt-3.5-turbo\",\"messages\":[{\"role\":\"system\",\"content\":\"Hello\"},{\"role\":\"user\",\"content\":\"Hello\"}],\"temperature\":1}";

	json_object *root = json_tokener_parse(input);


	   printf("The json string:\n\n%s\n\n", json_object_to_json_string(root));

//   printf("The json object to string:\n\n%s\n", json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY));


	json_object_put(root);
	return 0;
}
