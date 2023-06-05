/*
https://jsonlint.com/
https://github.com/rbtylee/tutorial-jsonc/blob/master/tutorial/legacy.md
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <json-c/json.h>

/*
Read chars from FILE *f and return as a json_object*
*/
//json_object *ftojson(FILE *f)
//{
//	char c       = '\0';
//	int status   = 0;
//	char *s      = NULL;
//
//	for (uintmax_t i = 1;; i++) {
//
//		c = fgetc(f);
//
//		if (c == EOF || feof(f)) {
//			break;
//		}
//
//		s = realloc(s, i);
//
//		if (s == NULL) {
//			break;
//		}
//
//		s[i - 1] = c;
//	}
//
//	json_object *root = json_tokener_parse(s);
//
//	free(s);
//
//	return root;
//}

int main(void)
{
	bool has_error = false;

	json_object *root = json_object_from_file("tmp.json");
	if (root == NULL) {
		fprintf(stderr, "%s\n", "Could not allocate the json object");
		has_error = true;
		goto cleanup;
	}

//	get string from json
	json_object *message_array = json_object_object_get(root, "messages");
	if (message_array == NULL) {
		message_array = json_object_new_array();
		json_object_object_add(root, "messages", message_array);
//		json_object_object_add(root, "role", json_object_new_string());
	}

	puts(json_object_get_string(message_array, "content"));

	printf("%s\n", json_object_to_json_string_ext(message_array, JSON_C_TO_STRING_PRETTY));

//	printf("%s\n", json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY));

//	json_object_to_file("tmp2.json", root);

	cleanup:
	json_object_put(root);

	if (has_error) {
		return 1;
	} else {
		return 0;
	}
}
