#include "parse_jsonl.h"

// Initialize the JSONL data structure
void init_jsonl_data(jsonl_data *data) {
    data->count = 0;
    data->capacity = 16;
    data->objects = calloc(data->capacity, sizeof(json_object*));
}

// Free the JSONL data structure
void free_jsonl_data(jsonl_data *data) {
    for (size_t i = 0; i < data->count; i++) {
        json_object_put(data->objects[i]);
        data->objects[i] = NULL;
    }
    free(data->objects);
    data->objects = NULL;
}

// Process a single JSON line
static int process_json_line(const char *line, jsonl_data *data) {

	if (line == NULL || data == NULL) { return 0; }

	const char *substring = NULL;
	char *jsonl_string = NULL;

	if (line[0] == 'd' || line[0] == 'D') {
		substring = line + strlen("data: ");
		if (substring[0] == '[') { return 0; }
		jsonl_string = calloc(1, strlen(substring) + 1);
		memcpy(jsonl_string, substring, strlen(substring));
	} else if (line[0] == 'e' || line[0] == 'E') {
		substring = line + strlen("event: ");
		if (substring[0] == '[') { return 0; }
		jsonl_string = calloc(1, strlen(substring) + 1);
		memcpy(jsonl_string, substring, strlen(substring));
	} else {
		jsonl_string = calloc(1, strlen(line) + 1);
		strcpy(jsonl_string, line);
	}

	// Tokenizer fails if the last char is a newline \n

	json_object *obj = json_tokener_parse(jsonl_string);

	if (jsonl_string != line) { free(jsonl_string); jsonl_string = NULL; }

	if (obj == NULL) {
	//fprintf(stderr, "Failed to parse JSON line\n");
		return 0;
	}

	// Resize array if needed
	if (data->count >= data->capacity) {
		data->capacity *= 2;
		data->objects = realloc(data->objects, sizeof(json_object*) * data->capacity);
	}

	data->objects[data->count++] = obj;

	return 1;
}

// Process JSONL data line by line
int process_jsonl_data(const char *jsonl_string, jsonl_data *data) {
    if (jsonl_string == NULL || data == NULL) {
        return 0;
    }

    const char *start = jsonl_string;
    const char *end = jsonl_string + strlen(jsonl_string);

    while (start < end) {
        // Find next newline
        const char *newline = memchr(start, '\n', end - start);

        if (newline == NULL) {
            // Handle last line if it doesn't have a newline
            size_t len = end - start;
            if (len > 0) {
//                char *line = strndup(start, len);
                char *line = calloc(len + 1, 1);
                if (line != NULL) {
                    memcpy(line, start, len);
                    process_json_line(line, data);
                    free(line);
                }
            }
            break;
        }

        // Extract line
        size_t len = newline - start;
        if (len > 0) {
//            char *line = strndup(start, len);
	    // process_json_line() will fail if the json line ends with \n
            char *line = calloc(len + 1, 1);
            if (line != NULL) {
                memcpy(line, start, len);
                process_json_line(line, data);
                free(line);
		line = NULL;
            }
        }

        start = newline + 1;
    }

    return data->count;
}

// Caller must free the return of the fn()
json_object* sanitize_and_parse_jsonl(const char *restrict jsonl_string) {

    if (jsonl_string == NULL) { return 0; }

    json_object *root = NULL;

    const char *start = jsonl_string;
    const char *end = jsonl_string + strlen(jsonl_string);

    while (start < end) {
        // Find next newline
        const char *newline = memchr(start, '\n', end - start);

        if (newline == NULL) {
            // Handle last line if it doesn't have a newline
            size_t len = end - start;
            if (len > 0) {
//                char *line = strndup(start, len);
                char *line = calloc(len + 1, 1);
                if (line != NULL) {
                    memcpy(line, start, len);
                    root = parse_jsonl(line);
                    free(line);
                }
            }
            break;
        }

        // Extract line
        size_t len = newline - start;
        if (len > 0) {
//            char *line = strndup(start, len);
	    // process_json_line() will fail if the json line ends with \n
            char *line = calloc(len + 1, 1);
            if (line != NULL) {
                memcpy(line, start, len);
                root = parse_jsonl(line);
                free(line);
		line = NULL;
            }
        }

        start = newline + 1;
    }

    return root;
}

static json_object* parse_jsonl(const char *restrict line) {

	if (line == NULL) { return 0; }

	const char *substring = NULL;
	char *jsonl_string = NULL;

//	remove prefix from jsonl
	if (line[0] == 'd' || line[0] == 'D') {
		substring = line + strlen("data: ");
		jsonl_string = calloc(1, strlen(substring) + 1);
		memcpy(jsonl_string, substring, strlen(substring));
	} else if (line[0] == 'e' || line[0] == 'E') {
		substring = line + strlen("event: ");
		jsonl_string = calloc(1, strlen(substring) + 1);
		memcpy(jsonl_string, substring, strlen(substring));
	} else {
		jsonl_string = calloc(1, strlen(line) + 1);
		strcpy(jsonl_string, line);
	}

	json_object *root = json_tokener_parse(jsonl_string);

	if (jsonl_string != line) { free(jsonl_string); jsonl_string = NULL; }

//	if (root == NULL) {
//		fprintf(stderr, "Failed to parse JSON line\n");
//		return NULL;
//	}

	return root;
}

//int main(void) {
//    // Example JSONL string
//    const char *jsonl_string =
//        "data: {\"name\":\"John\",\"age\":30}\nevent: {\"name\":\"Alice\",\"age\":25}\n {\"name\":\"Bob\",\"age\":35}\ndata: null\nevent: stop";
//
//    jsonl_data data;
//    init_jsonl_data(&data);
//
//    int count = process_jsonl_data(jsonl_string, &data);
//    printf("Processed %d JSON objects\n", count);
//
//    // Print each parsed object
//    for (size_t i = 0; i < data.count; i++) {
//        printf("Object %zu:\n%s\n", i + 1, json_object_to_json_string(data.objects[i]));
//
//	const char *val = json_object_get_string(json_object_object_get(data.objects[i], "name"));
//
//	if (val != NULL) {
//		puts(val);
//	}
//    }
//
//    free_jsonl_data(&data);
//
//    return 0;
//}
