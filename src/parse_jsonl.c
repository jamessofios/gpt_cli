#include "parse_jsonl.h"

// caller must free the returned string
char *do_json(const char* line)
{
	if (line == NULL) { return NULL; }

	const char *substring = NULL;
	json_object *obj = NULL;
	char *ret = NULL;

	if (line[0] == 'd' || line[0] == 'D') {
		substring = line + strlen("data: ");
		if (substring[0] == '[') {
			return 0;
		} else {
			obj = json_tokener_parse(substring);

			#ifdef DEBUG_ASSERTS
			assert(obj != NULL);
			#else
			if(obj == NULL) {
				goto cleanup;
			}
			#endif

			json_object *choices = json_object_object_get(obj, "choices");
			if (json_object_get_type(choices) != json_type_array) { goto cleanup; }

			json_object *first_choice = json_object_array_get_idx(choices, 0);
			if (json_object_get_type(first_choice) != json_type_object) { goto cleanup; }

			json_object *finish_reason = json_object_object_get(first_choice, "finish_reason");
			if (json_object_get_type(finish_reason) == json_type_string && !strcmp(json_object_get_string(finish_reason),"stop") ) { goto cleanup; }

			json_object *delta = json_object_object_get(first_choice, "delta");
			if (json_object_get_type(delta) != json_type_object) { goto cleanup; }

			json_object *content = json_object_object_get(delta, "content");
			if (json_object_get_type(content) != json_type_string) { goto cleanup; }

			const char *tmp = json_object_get_string(content);
			ret = calloc(1, strlen(tmp) + 1);
			memcpy(ret, tmp, strlen(tmp));
		}

	} else {
		return NULL;
	}
	cleanup:
	json_object_put(obj);
	return ret;
}
