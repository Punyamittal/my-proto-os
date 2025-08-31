#include "langchain.h"
#include "network.h"
#include "json.h"
#include "screen.h"
#include <string.h>

// Initialize LangChain session
void langchain_init(langchain_session_t* session, const char* api_key, int model_type) {
    if (!session) return;
    
    // Set API key
    strncpy(session->api_key, api_key, MAX_API_KEY_LENGTH - 1);
    session->api_key[MAX_API_KEY_LENGTH - 1] = '\0';
    
    // Set default model
    session->model_type = model_type;
    switch (model_type) {
        case MODEL_OPENAI_GPT:
            strcpy(session->model_name, "gpt-3.5-turbo");
            break;
        case MODEL_ANTHROPIC_CLAUDE:
            strcpy(session->model_name, "claude-3-sonnet");
            break;
        case MODEL_LOCAL_LLM:
            strcpy(session->model_name, "local-llm");
            break;
        case MODEL_GOOGLE_GEMINI:
            strcpy(session->model_name, "gemini-pro");
            break;
        default:
            strcpy(session->model_name, "unknown");
            break;
    }
    
    // Initialize conversation history
    session->history_count = 0;
    session->max_tokens = 1000;
    session->temperature = 0.7;
    
    // Add system message
    langchain_add_message(session, "system", "You are a helpful AI assistant running on ProtoOS, a custom operating system. Keep responses concise and helpful.");
    
    print_string("LangChain session initialized with model: ", VGA_LIGHT_GREEN);
    print_string(session->model_name, VGA_LIGHT_GREEN);
    print_string("\n", VGA_LIGHT_GREEN);
}

// Add a message to conversation history
int langchain_add_message(langchain_session_t* session, const char* role, const char* content) {
    if (!session || !role || !content) return 0;
    
    if (session->history_count >= MAX_CONVERSATION_HISTORY) {
        // Remove oldest message (shift all messages down)
        for (int i = 0; i < MAX_CONVERSATION_HISTORY - 1; i++) {
            session->history[i] = session->history[i + 1];
        }
        session->history_count = MAX_CONVERSATION_HISTORY - 1;
    }
    
    int idx = session->history_count;
    strncpy(session->history[idx].role, role, 15);
    session->history[idx].role[15] = '\0';
    
    strncpy(session->history[idx].content, content, MAX_PROMPT_LENGTH - 1);
    session->history[idx].content[MAX_PROMPT_LENGTH - 1] = '\0';
    
    session->history[idx].timestamp = get_timestamp();
    session->history_count++;
    
    return 1;
}

// Clear conversation history
int langchain_clear_history(langchain_session_t* session) {
    if (!session) return 0;
    
    session->history_count = 0;
    
    // Re-add system message
    langchain_add_message(session, "system", "You are a helpful AI assistant running on ProtoOS, a custom operating system. Keep responses concise and helpful.");
    
    return 1;
}

// Set AI model
int langchain_set_model(langchain_session_t* session, int model_type, const char* model_name) {
    if (!session) return 0;
    
    session->model_type = model_type;
    if (model_name) {
        strncpy(session->model_name, model_name, 63);
        session->model_name[63] = '\0';
    }
    
    return 1;
}

// Get current timestamp (simplified)
long get_timestamp() {
    // In a real OS, this would use system time
    // For now, return a simple counter
    static long counter = 0;
    return counter++;
}

// Format conversation prompt with history
void format_conversation_prompt(langchain_session_t* session, const char* user_prompt, char* formatted_prompt, int max_size) {
    if (!session || !user_prompt || !formatted_prompt) return;
    
    int written = 0;
    
    // Add conversation history
    for (int i = 0; i < session->history_count && written < max_size - 1; i++) {
        written += snprintf(formatted_prompt + written, max_size - written, 
            "%s: %s\n", session->history[i].role, session->history[i].content);
    }
    
    // Add current user prompt
    written += snprintf(formatted_prompt + written, max_size - written, 
        "user: %s\nassistant:", user_prompt);
    
    formatted_prompt[max_size - 1] = '\0';
}

// Main chat function
int langchain_chat(langchain_session_t* session, const char* prompt, char* response, int max_response_size) {
    if (!session || !prompt || !response) return 0;
    
    // Add user message to history
    langchain_add_message(session, "user", prompt);
    
    // Call appropriate AI API based on model type
    int success = 0;
    switch (session->model_type) {
        case MODEL_OPENAI_GPT:
            success = openai_chat_completion(session, prompt, response, max_response_size);
            break;
        case MODEL_ANTHROPIC_CLAUDE:
            success = anthropic_chat_completion(session, prompt, response, max_response_size);
            break;
        case MODEL_LOCAL_LLM:
            success = local_llm_completion(session, prompt, response, max_response_size);
            break;
        case MODEL_GOOGLE_GEMINI:
            success = gemini_chat_completion(session, prompt, response, max_response_size);
            break;
        default:
            strcpy(response, "Error: Unknown model type");
            return 0;
    }
    
    if (success) {
        // Add AI response to history
        langchain_add_message(session, "assistant", response);
    }
    
    return success;
}

// OpenAI chat completion
int openai_chat_completion(langchain_session_t* session, const char* prompt, char* response, int max_response_size) {
    // Create OpenAI API request
    char request_body[2048];
    char formatted_prompt[2048];
    
    format_conversation_prompt(session, prompt, formatted_prompt, sizeof(formatted_prompt));
    
    snprintf(request_body, sizeof(request_body),
        "{\"model\":\"%s\",\"messages\":[{\"role\":\"user\",\"content\":\"%s\"}],\"max_tokens\":%d,\"temperature\":%.1f}",
        session->model_name, formatted_prompt, session->max_tokens, session->temperature);
    
    // Make HTTP request to OpenAI API
    char http_response[8192];
    char api_url[256];
    snprintf(api_url, sizeof(api_url), "https://api.openai.com/v1/chat/completions");
    
    // In a real implementation, you'd make an actual HTTP request
    // For now, we'll simulate the response
    if (strstr(prompt, "hello") || strstr(prompt, "hi")) {
        strcpy(response, "Hello! I'm your AI assistant running on ProtoOS. How can I help you today?");
    } else if (strstr(prompt, "help")) {
        strcpy(response, "I can help you with various tasks. Try asking me questions about programming, system administration, or general knowledge.");
    } else if (strstr(prompt, "os") || strstr(prompt, "operating system")) {
        strcpy(response, "ProtoOS is a custom operating system built from scratch. It includes a kernel, bootloader, and now AI capabilities through LangChain integration!");
    } else {
        strcpy(response, "That's an interesting question! I'm running on ProtoOS, a custom operating system. While I'm currently in simulation mode, I can help you explore the system.");
    }
    
    return 1;
}

// Anthropic Claude completion
int anthropic_chat_completion(langchain_session_t* session, const char* prompt, char* response, int max_response_size) {
    // Similar to OpenAI but for Claude
    // For now, return a Claude-specific response
    strcpy(response, "Hello! I'm Claude, running on ProtoOS. This is a simulated response, but in a real implementation, I'd connect to the Anthropic API.");
    return 1;
}

// Local LLM completion
int local_llm_completion(langchain_session_t* session, const char* prompt, char* response, int max_response_size) {
    // For local LLMs, you'd typically load a model file and run inference
    // For now, return a local response
    strcpy(response, "This is a simulated local LLM response. In a real implementation, you'd load a local model file and run inference directly on ProtoOS.");
    return 1;
}

// Google Gemini completion
int gemini_chat_completion(langchain_session_t* session, const char* prompt, char* response, int max_response_size) {
    // Create Gemini API request
    char request_body[2048];
    char formatted_prompt[2048];
    
    format_conversation_prompt(session, prompt, formatted_prompt, sizeof(formatted_prompt));
    
    // Gemini uses a different API format
    snprintf(request_body, sizeof(request_body),
        "{\"contents\":[{\"parts\":[{\"text\":\"%s\"}]}],\"generationConfig\":{\"temperature\":%.1f,\"maxOutputTokens\":%d}}",
        formatted_prompt, session->temperature, session->max_tokens);
    
    // Make HTTP request to Gemini API
    char http_response[8192];
    char api_url[256];
    snprintf(api_url, sizeof(api_url), "https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent?key=%s", session->api_key);
    
    // In a real implementation, you'd make an actual HTTP request
    // For now, we'll simulate the response with Gemini-specific behavior
    if (strstr(prompt, "hello") || strstr(prompt, "hi")) {
        strcpy(response, "Hello! I'm Gemini, Google's AI assistant running on ProtoOS. I'm designed to be helpful, creative, and accurate. How can I assist you today?");
    } else if (strstr(prompt, "help")) {
        strcpy(response, "I'm Gemini, and I can help you with a wide range of tasks! I'm particularly good at creative writing, coding, analysis, and general knowledge. What would you like to explore?");
    } else if (strstr(prompt, "os") || strstr(prompt, "operating system")) {
        strcpy(response, "ProtoOS is a fascinating custom operating system! It's built from scratch with a kernel, bootloader, and now AI integration. I'm impressed by the LangChain integration - that's a sophisticated approach to AI capabilities in an OS.");
    } else if (strstr(prompt, "gemini") || strstr(prompt, "google")) {
        strcpy(response, "I'm Gemini, Google's most capable AI model. I'm designed to be multimodal, creative, and helpful across many domains. Running on ProtoOS is quite unique - it shows how AI can be integrated at the operating system level!");
    } else {
        strcpy(response, "That's an interesting question! As Gemini, I'm running on ProtoOS, which is a custom operating system with AI integration. While I'm currently in simulation mode, I'm designed to be helpful, creative, and accurate in real-world applications.");
    }
    
    return 1;
}

// Utility function to create a simple prompt template
int create_prompt_template(const char* template_str, const char* variables[], const char* values[], int num_vars, char* result, int max_size) {
    if (!template_str || !result) return 0;
    
    strncpy(result, template_str, max_size - 1);
    result[max_size - 1] = '\0';
    
    // Simple variable substitution (replace {var} with values)
    for (int i = 0; i < num_vars && i < 10; i++) {
        char placeholder[32];
        snprintf(placeholder, sizeof(placeholder), "{%s}", variables[i]);
        
        // Find and replace placeholder
        char* pos = strstr(result, placeholder);
        if (pos && values[i]) {
            int placeholder_len = strlen(placeholder);
            int value_len = strlen(values[i]);
            int remaining_len = strlen(pos + placeholder_len);
            
            if (pos - result + value_len + remaining_len < max_size) {
                memmove(pos + value_len, pos + placeholder_len, remaining_len + 1);
                memcpy(pos, values[i], value_len);
            }
        }
    }
    
    return 1;
}
