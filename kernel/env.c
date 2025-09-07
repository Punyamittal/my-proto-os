#include "env.h"
#include "screen.h"

// Define NULL for kernel environment
#ifndef NULL
#define NULL ((void*)0)
#endif

// Define size_t for kernel environment
typedef unsigned int size_t;

// String function implementations for kernel environment
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++));
    return dest;
}

size_t strlen(const char* s) {
    size_t len = 0;
    while (s[len]) len++;
    return len;
}

char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

char* strstr(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack;
    
    for (; *haystack; haystack++) {
        const char* h = haystack;
        const char* n = needle;
        while (*h && *n && (*h == *n)) {
            h++;
            n++;
        }
        if (!*n) return (char*)haystack;
    }
    return NULL;
}

// Global environment storage
static env_var_t env_vars[MAX_ENV_VARS];
static int env_count = 0;

// Initialize environment
void init_environment() {
    env_count = 0;
    
    // Try to load .env file
    if (load_env_file(".env")) {
        print_string("Environment loaded from .env file\n", VGA_LIGHT_GREEN);
    } else {
        print_string("No .env file found, using default environment\n", VGA_LIGHT_YELLOW);
    }
}

// Load environment variables from file
int load_env_file(const char* filename) {
    // In a real OS, you'd read from the actual file system
    // For now, we'll simulate loading some common environment variables
    
    // Simulate loading from .env file
    set_env_var("GEMINI_API_KEY", "your_gemini_api_key_here");
    set_env_var("OPENAI_API_KEY", "your_openai_api_key_here");
    set_env_var("ANTHROPIC_API_KEY", "your_anthropic_api_key_here");
    
    // In a real implementation, you would:
    // 1. Open the .env file
    // 2. Parse each line (key=value format)
    // 3. Store in env_vars array
    // 4. Handle comments (# lines) and empty lines
    
    return 1; // Success
}

// Get environment variable value
const char* get_env_var(const char* key) {
    if (!key) return NULL;
    
    for (int i = 0; i < env_count; i++) {
        if (strcmp(env_vars[i].key, key) == 0) {
            return env_vars[i].value;
        }
    }
    
    return NULL; // Not found
}

// Set environment variable
int set_env_var(const char* key, const char* value) {
    if (!key || !value) return 0;
    
    // Check if key already exists
    for (int i = 0; i < env_count; i++) {
        if (strcmp(env_vars[i].key, key) == 0) {
            // Update existing variable
            strncpy(env_vars[i].value, value, MAX_ENV_VALUE_LENGTH - 1);
            env_vars[i].value[MAX_ENV_VALUE_LENGTH - 1] = '\0';
            return 1;
        }
    }
    
    // Add new variable
    if (env_count < MAX_ENV_VARS) {
        strncpy(env_vars[env_count].key, key, MAX_ENV_KEY_LENGTH - 1);
        env_vars[env_count].key[MAX_ENV_KEY_LENGTH - 1] = '\0';
        
        strncpy(env_vars[env_count].value, value, MAX_ENV_VALUE_LENGTH - 1);
        env_vars[env_count].value[MAX_ENV_VALUE_LENGTH - 1] = '\0';
        
        env_count++;
        return 1;
    }
    
    return 0; // No space left
}

// Print all environment variables (for debugging)
void print_environment() {
    print_string("Environment Variables:\n", VGA_LIGHT_CYAN);
    
    for (int i = 0; i < env_count; i++) {
        print_string("  ", VGA_LIGHT_GREY);
        print_string(env_vars[i].key, VGA_LIGHT_YELLOW);
        print_string(" = ", VGA_LIGHT_GREY);
        
        // Mask API keys for security
        if (strstr(env_vars[i].key, "API_KEY") != NULL) {
            int len = strlen(env_vars[i].value);
            if (len > 8) {
                print_string("***", VGA_LIGHT_RED);
                for (int j = len - 5; j < len; j++) {
                    if (j >= 0) {
                        print_char(env_vars[i].value[j], VGA_LIGHT_WHITE);
                    }
                }
            } else {
                print_string("***", VGA_LIGHT_RED);
            }
        } else {
            print_string(env_vars[i].value, VGA_LIGHT_WHITE);
        }
        
        print_string("\n", VGA_LIGHT_GREY);
    }
}


