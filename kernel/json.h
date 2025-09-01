#ifndef JSON_H
#define JSON_H

// JSON parsing configuration
#define MAX_JSON_DEPTH 10
#define MAX_JSON_KEY_LENGTH 64
#define MAX_JSON_VALUE_LENGTH 512

// JSON value types
#define JSON_NULL 0
#define JSON_STRING 1
#define JSON_NUMBER 2
#define JSON_BOOL 3
#define JSON_OBJECT 4
#define JSON_ARRAY 5

// JSON parsing result
typedef struct {
    int type;
    char key[MAX_JSON_KEY_LENGTH];
    char value[MAX_JSON_VALUE_LENGTH];
    int success;
} json_parse_result_t;

// JSON parsing functions
int json_parse(const char* json_string, json_parse_result_t* results, int max_results);
int json_extract_string(const char* json_string, const char* key, char* value, int max_value_size);
int json_extract_number(const char* json_string, const char* key, double* value);
int json_extract_bool(const char* json_string, const char* key, int* value);

// JSON creation functions
int json_create_object(char* buffer, int max_size, const char* key, const char* value);
int json_create_array(char* buffer, int max_size, const char* values[], int num_values);

// Utility functions
int json_validate(const char* json_string);
int json_get_array_length(const char* json_string, const char* key);

#endif // JSON_H

