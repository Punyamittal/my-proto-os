#ifndef ENV_H
#define ENV_H

// Environment configuration
#define MAX_ENV_VARS 20
#define MAX_ENV_KEY_LENGTH 64
#define MAX_ENV_VALUE_LENGTH 256

// Environment variable structure
typedef struct {
    char key[MAX_ENV_KEY_LENGTH];
    char value[MAX_ENV_VALUE_LENGTH];
} env_var_t;

// Environment functions
void init_environment();
int load_env_file(const char* filename);
const char* get_env_var(const char* key);
int set_env_var(const char* key, const char* value);

// Common environment variables
#define ENV_GEMINI_API_KEY "GEMINI_API_KEY"
#define ENV_OPENAI_API_KEY "OPENAI_API_KEY"
#define ENV_ANTHROPIC_API_KEY "ANTHROPIC_API_KEY"

#endif // ENV_H
