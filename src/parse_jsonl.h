#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

// Structure to store parsed JSON objects
typedef struct {
    json_object **objects;
    size_t count;
    size_t capacity;
} jsonl_data;

void init_jsonl_data(jsonl_data *data);
void free_jsonl_data(jsonl_data *data);
int process_jsonl_data(const char *jsonl_string, jsonl_data *data);
static int process_json_line(const char *line, jsonl_data *data);

json_object* sanitize_and_parse_jsonl(const char *restrict jsonl_string);
static json_object* parse_jsonl(const char *restrict line);
