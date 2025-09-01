#include "json.h"
#include <string.h>

// Skip whitespace characters
static void skip_whitespace(const char** json) {
    while (**json == ' ' || **json == '\t' || **json == '\n' || **json == '\r') {
        (*json)++;
    }
}

// Skip a JSON string
static void skip_string(const char** json) {
    if (**json == '"') {
        (*json)++;
        while (**json != '"' && **json != '\0') {
            if (**json == '\\') {
                (*json)++; // Skip escaped character
            }
            (*json)++;
        }
        if (**json == '"') {
            (*json)++;
        }
    }
}

// Skip a JSON value
static void skip_value(const char** json) {
    skip_whitespace(json);
    
    switch (**json) {
        case '"':
            skip_string(json);
            break;
        case '{':
            (*json)++;
            while (**json != '}' && **json != '\0') {
                skip_whitespace(json);
                if (**json == '"') {
                    skip_string(json);
                    skip_whitespace(json);
                    if (**json == ':') {
                        (*json)++;
                        skip_value(json);
                    }
                } else {
                    (*json)++;
                }
            }
            if (**json == '}') (*json)++;
            break;
        case '[':
            (*json)++;
            while (**json != ']' && **json != '\0') {
                skip_value(json);
                skip_whitespace(json);
                if (**json == ',') (*json)++;
            }
            if (**json == ']') (*json)++;
            break;
        case 't':
            if (strncmp(*json, "true", 4) == 0) {
                *json += 4;
            }
            break;
        case 'f':
            if (strncmp(*json, "false", 5) == 0) {
                *json += 5;
            }
            break;
        case 'n':
            if (strncmp(*json, "null", 4) == 0) {
                *json += 4;
            }
            break;
        default:
            // Skip numbers
            while ((**json >= '0' && **json <= '9') || **json == '.' || **json == '-' || **json == 'e' || **json == 'E') {
                (*json)++;
            }
            break;
    }
}

// Parse a JSON string value
static int parse_string_value(const char** json, char* value, int max_size) {
    if (**json != '"') return 0;
    
    (*json)++;
    int i = 0;
    
    while (**json != '"' && **json != '\0' && i < max_size - 1) {
        if (**json == '\\') {
            (*json)++;
            switch (**json) {
                case '"': value[i++] = '"'; break;
                case '\\': value[i++] = '\\'; break;
                case '/': value[i++] = '/'; break;
                case 'b': value[i++] = '\b'; break;
                case 'f': value[i++] = '\f'; break;
                case 'n': value[i++] = '\n'; break;
                case 'r': value[i++] = '\r'; break;
                case 't': value[i++] = '\t'; break;
                default: value[i++] = **json; break;
            }
        } else {
            value[i++] = **json;
        }
        (*json)++;
    }
    
    value[i] = '\0';
    
    if (**json == '"') {
        (*json)++;
        return 1;
    }
    
    return 0;
}

// Parse JSON and extract key-value pairs
int json_parse(const char* json_string, json_parse_result_t* results, int max_results) {
    if (!json_string || !results || max_results <= 0) return 0;
    
    const char* json = json_string;
    int result_count = 0;
    
    skip_whitespace(&json);
    
    if (*json != '{') return 0;
    json++;
    
    while (*json != '}' && *json != '\0' && result_count < max_results) {
        skip_whitespace(&json);
        
        if (*json != '"') break;
        
        // Parse key
        if (!parse_string_value(&json, results[result_count].key, MAX_JSON_KEY_LENGTH)) {
            break;
        }
        
        skip_whitespace(&json);
        if (*json != ':') break;
        json++;
        
        skip_whitespace(&json);
        
        // Parse value
        if (*json == '"') {
            results[result_count].type = JSON_STRING;
            parse_string_value(&json, results[result_count].value, MAX_JSON_VALUE_LENGTH);
        } else if (*json == 't' && strncmp(json, "true", 4) == 0) {
            results[result_count].type = JSON_BOOL;
            strcpy(results[result_count].value, "true");
            json += 4;
        } else if (*json == 'f' && strncmp(json, "false", 5) == 0) {
            results[result_count].type = JSON_BOOL;
            strcpy(results[result_count].value, "false");
            json += 5;
        } else if (*json == 'n' && strncmp(json, "null", 4) == 0) {
            results[result_count].type = JSON_NULL;
            strcpy(results[result_count].value, "null");
            json += 4;
        } else if ((*json >= '0' && *json <= '9') || *json == '-') {
            results[result_count].type = JSON_NUMBER;
            int i = 0;
            while ((*json >= '0' && *json <= '9') || *json == '.' || *json == '-' || *json == 'e' || *json == 'E') {
                if (i < MAX_JSON_VALUE_LENGTH - 1) {
                    results[result_count].value[i++] = *json;
                }
                json++;
            }
            results[result_count].value[i] = '\0';
        } else {
            // Skip complex values for now
            skip_value(&json);
            results[result_count].type = JSON_OBJECT;
            strcpy(results[result_count].value, "[complex]");
        }
        
        results[result_count].success = 1;
        result_count++;
        
        skip_whitespace(&json);
        if (*json == ',') {
            json++;
        }
    }
    
    return result_count;
}

// Extract a string value by key
int json_extract_string(const char* json_string, const char* key, char* value, int max_value_size) {
    json_parse_result_t results[10];
    int count = json_parse(json_string, results, 10);
    
    for (int i = 0; i < count; i++) {
        if (strcmp(results[i].key, key) == 0 && results[i].type == JSON_STRING) {
            strncpy(value, results[i].value, max_value_size - 1);
            value[max_value_size - 1] = '\0';
            return 1;
        }
    }
    
    return 0;
}

// Extract a number value by key
int json_extract_number(const char* json_string, const char* key, double* value) {
    json_parse_result_t results[10];
    int count = json_parse(json_string, results, 10);
    
    for (int i = 0; i < count; i++) {
        if (strcmp(results[i].key, key) == 0 && results[i].type == JSON_NUMBER) {
            // Simple atof implementation
            *value = 0.0;
            double sign = 1.0;
            double decimal = 0.0;
            double factor = 0.1;
            
            const char* num_str = results[i].value;
            
            if (*num_str == '-') {
                sign = -1.0;
                num_str++;
            }
            
            while (*num_str >= '0' && *num_str <= '9') {
                *value = *value * 10.0 + (*num_str - '0');
                num_str++;
            }
            
            if (*num_str == '.') {
                num_str++;
                while (*num_str >= '0' && *num_str <= '9') {
                    decimal += factor * (*num_str - '0');
                    factor *= 0.1;
                    num_str++;
                }
            }
            
            *value = sign * (*value + decimal);
            return 1;
        }
    }
    
    return 0;
}

// Extract a boolean value by key
int json_extract_bool(const char* json_string, const char* key, int* value) {
    json_parse_result_t results[10];
    int count = json_parse(json_string, results, 10);
    
    for (int i = 0; i < count; i++) {
        if (strcmp(results[i].key, key) == 0 && results[i].type == JSON_BOOL) {
            *value = (strcmp(results[i].value, "true") == 0) ? 1 : 0;
            return 1;
        }
    }
    
    return 0;
}

// Create a simple JSON object
int json_create_object(char* buffer, int max_size, const char* key, const char* value) {
    int written = snprintf(buffer, max_size, "{\"%s\":\"%s\"}", key, value);
    return (written < max_size) ? written : -1;
}

// Create a simple JSON array
int json_create_array(char* buffer, int max_size, const char* values[], int num_values) {
    if (num_values <= 0) {
        strcpy(buffer, "[]");
        return 2;
    }
    
    int written = 0;
    written += snprintf(buffer, max_size, "[");
    
    for (int i = 0; i < num_values && written < max_size; i++) {
        if (i > 0) {
            written += snprintf(buffer + written, max_size - written, ",");
        }
        written += snprintf(buffer + written, max_size - written, "\"%s\"", values[i]);
    }
    
    written += snprintf(buffer + written, max_size - written, "]");
    return (written < max_size) ? written : -1;
}

// Basic JSON validation
int json_validate(const char* json_string) {
    if (!json_string) return 0;
    
    int brace_count = 0;
    int bracket_count = 0;
    int in_string = 0;
    int escaped = 0;
    
    for (int i = 0; json_string[i] != '\0'; i++) {
        char c = json_string[i];
        
        if (escaped) {
            escaped = 0;
            continue;
        }
        
        if (c == '\\') {
            escaped = 1;
            continue;
        }
        
        if (c == '"') {
            in_string = !in_string;
            continue;
        }
        
        if (in_string) continue;
        
        switch (c) {
            case '{': brace_count++; break;
            case '}': brace_count--; break;
            case '[': bracket_count++; break;
            case ']': bracket_count--; break;
        }
        
        if (brace_count < 0 || bracket_count < 0) return 0;
    }
    
    return (brace_count == 0 && bracket_count == 0 && !in_string);
}

// Get array length (simplified)
int json_get_array_length(const char* json_string, const char* key) {
    // This is a simplified implementation
    // In a real implementation, you'd parse the array structure
    return 0;
}

