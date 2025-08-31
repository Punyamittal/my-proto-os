#ifndef LANGCHAIN_H
#define LANGCHAIN_H

// LangChain configuration
#define MAX_PROMPT_LENGTH 1024
#define MAX_RESPONSE_LENGTH 2048
#define MAX_CONVERSATION_HISTORY 10
#define MAX_API_KEY_LENGTH 128

// AI model types
#define MODEL_OPENAI_GPT 0
#define MODEL_ANTHROPIC_CLAUDE 1
#define MODEL_LOCAL_LLM 2
#define MODEL_GOOGLE_GEMINI 3

// Conversation message structure
typedef struct {
    char role[16];        // "user", "assistant", "system"
    char content[MAX_PROMPT_LENGTH];
    long timestamp;
} conversation_message_t;

// LangChain session structure
typedef struct {
    char api_key[MAX_API_KEY_LENGTH];
    int model_type;
    char model_name[64];
    conversation_message_t history[MAX_CONVERSATION_HISTORY];
    int history_count;
    int max_tokens;
    float temperature;
} langchain_session_t;

// LangChain functions
void langchain_init(langchain_session_t* session, const char* api_key, int model_type);
int langchain_chat(langchain_session_t* session, const char* prompt, char* response, int max_response_size);
int langchain_add_message(langchain_session_t* session, const char* role, const char* content);
int langchain_clear_history(langchain_session_t* session);
int langchain_set_model(langchain_session_t* session, int model_type, const char* model_name);

// AI API specific functions
int openai_chat_completion(langchain_session_t* session, const char* prompt, char* response, int max_response_size);
int anthropic_chat_completion(langchain_session_t* session, const char* prompt, char* response, int max_response_size);
int local_llm_completion(langchain_session_t* session, const char* prompt, char* response, int max_response_size);
int gemini_chat_completion(langchain_session_t* session, const char* prompt, char* response, int max_response_size);

// Utility functions
long get_timestamp();
void format_conversation_prompt(langchain_session_t* session, const char* user_prompt, char* formatted_prompt, int max_size);

#endif // LANGCHAIN_H
