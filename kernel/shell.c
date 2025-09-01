#include "shell.h"
#include "screen.h"
#include "keyboard.h"
#include "mouse.h"
#include "langchain.h"
#include "env.h"
#include "voice.h"
#include "assistant.h"
#include <string.h>

// Global shell state
static char command_history[MAX_HISTORY][MAX_COMMAND_LENGTH];
static int history_index = 0;
static int history_count = 0;
static char current_prompt[PROMPT_LENGTH] = "ProtoOS> ";
static langchain_session_t ai_session;

// Built-in commands array
static shell_command_t builtin_commands[] = {
    {"help", "Show available commands", cmd_help},
    {"clear", "Clear the screen", cmd_clear},
    {"ai", "Interact with AI assistant", cmd_ai},
    {"chat", "Chat with AI (alias for 'ai')", cmd_chat},
    {"model", "Set AI model type", cmd_model},
    {"setkey", "Set API key for AI models", cmd_set_api_key},
    {"env", "Show environment variables", cmd_env},
    {"voice", "Voice assistant commands", cmd_voice},
    {"assistant", "AI assistant system", cmd_assistant},
    {"search", "Web search functionality", cmd_search},
    {"weather", "Get weather information", cmd_weather},
    {"news", "Get latest news", cmd_news},
    {"history", "Show command history", cmd_history},
    {"reset", "Reset AI conversation", cmd_reset},
    {"mouse", "Mouse control commands", cmd_mouse},
    {"exit", "Exit the shell", cmd_exit},
    {"", "", NULL} // End marker
};

// Initialize shell
void init_shell() {
    // Initialize environment first
    init_environment();
    
    // Initialize AI assistant system
    init_assistant_system();
    
    // Try to get Gemini API key from environment
    const char* gemini_key = get_env_var(ENV_GEMINI_API_KEY);
    if (gemini_key && strcmp(gemini_key, "your_gemini_api_key_here") != 0) {
        // Use the actual API key from .env
        langchain_init(&ai_session, gemini_key, MODEL_GOOGLE_GEMINI);
        print_string("Gemini API key loaded from environment\n", VGA_LIGHT_GREEN);
    } else {
        // Use demo key initially
        langchain_init(&ai_session, "demo-key", MODEL_GOOGLE_GEMINI);
        print_string("Using demo API key. Use 'setkey' to set your Gemini API key\n", VGA_LIGHT_YELLOW);
    }
    
    // Start the AI assistant
    start_assistant();
    
    // Clear command history
    history_count = 0;
    history_index = 0;
    
    print_string("ProtoOS Shell with AI Assistant Integration\n", VGA_LIGHT_CYAN);
    print_string("Type 'help' for available commands\n", VGA_LIGHT_GREY);
    print_string("Type 'ai' to start chatting with the AI\n", VGA_LIGHT_GREEN);
    print_string("Voice Assistant: Say 'Hey Proto' to activate\n", VGA_LIGHT_YELLOW);
    print_string("Default model: Google Gemini\n", VGA_LIGHT_YELLOW);
    print_string("Use 'setkey' to configure your Gemini API key\n", VGA_LIGHT_YELLOW);
    print_string("==========================================\n", VGA_LIGHT_GREY);
}

// Print shell prompt
void print_prompt() {
    print_string(current_prompt, VGA_LIGHT_YELLOW);
}

// Add command to history
void add_to_history(const char* command) {
    if (strlen(command) == 0) return;
    
    strncpy(command_history[history_index], command, MAX_COMMAND_LENGTH - 1);
    command_history[history_index][MAX_COMMAND_LENGTH - 1] = '\0';
    
    history_index = (history_index + 1) % MAX_HISTORY;
    if (history_count < MAX_HISTORY) {
        history_count++;
    }
}

// Parse command line into arguments
int parse_command(const char* command_line, char* argv[], int max_args) {
    if (!command_line || !argv || max_args <= 0) return 0;
    
    int argc = 0;
    const char* start = command_line;
    const char* end = command_line;
    
    // Skip leading whitespace
    while (*start == ' ' || *start == '\t') start++;
    
    while (*start != '\0' && argc < max_args - 1) {
        end = start;
        
        // Find end of current argument
        while (*end != '\0' && *end != ' ' && *end != '\t') end++;
        
        if (end > start) {
            int len = end - start;
            argv[argc] = (char*)start;
            start = end;
            argc++;
        }
        
        // Skip whitespace
        while (*start == ' ' || *start == '\t') start++;
    }
    
    argv[argc] = NULL;
    return argc;
}

// Process a command line
void process_command(const char* command_line) {
    if (!command_line || strlen(command_line) == 0) return;
    
    // Add to history
    add_to_history(command_line);
    
    // Parse command and arguments
    char* argv[MAX_ARGS];
    int argc = parse_command(command_line, argv, MAX_ARGS);
    
    if (argc == 0) return;
    
    // Find and execute command
    int found = 0;
    for (int i = 0; builtin_commands[i].function != NULL; i++) {
        if (strcmp(argv[0], builtin_commands[i].name) == 0) {
            builtin_commands[i].function(argc, argv);
            found = 1;
            break;
        }
    }
    
    if (!found) {
        print_string("Command not found: ", VGA_LIGHT_RED);
        print_string(argv[0], VGA_LIGHT_RED);
        print_string("\nType 'help' for available commands\n", VGA_LIGHT_RED);
    }
}

// Run the shell
void run_shell() {
    char command_buffer[MAX_COMMAND_LENGTH];
    int buffer_pos = 0;
    
    while (1) {
        print_prompt();
        
        // Read command from user
        buffer_pos = 0;
        memset(command_buffer, 0, MAX_COMMAND_LENGTH);
        
        while (1) {
            // Poll for keyboard input
            poll_keyboard();
            
            // Poll for mouse input
            poll_mouse();
            
            if (key_available()) {
                char key = get_char();
                
                if (key == '\n' || key == '\r') {
                    print_string("\n", VGA_LIGHT_GREY);
                    break;
                } else if (key == '\b' && buffer_pos > 0) {
                    buffer_pos--;
                    command_buffer[buffer_pos] = '\0';
                    print_char('\b', VGA_LIGHT_GREY);
                    print_char(' ', VGA_LIGHT_GREY);
                    print_char('\b', VGA_LIGHT_GREY);
                } else if (key >= 32 && key < 127 && buffer_pos < MAX_COMMAND_LENGTH - 1) {
                    command_buffer[buffer_pos++] = key;
                    print_char(key, VGA_LIGHT_WHITE);
                }
            }
            
            // Check for mouse clicks
            if (is_mouse_button_pressed(0)) { // Left click
                // Move cursor to mouse position
                int mouse_x, mouse_y;
                get_mouse_position(&mouse_x, &mouse_y);
                set_cursor(mouse_y, mouse_x);
                // Could add click-to-position functionality here
            }
            
            // Small delay to prevent excessive CPU usage
            for (volatile int i = 0; i < 1000; i++);
        }
        
        // Process the command
        process_command(command_buffer);
    }
}

// Built-in command implementations
int cmd_help(int argc, char* argv[]) {
    print_string("Available commands:\n", VGA_LIGHT_CYAN);
    
    for (int i = 0; builtin_commands[i].function != NULL; i++) {
        print_string("  ", VGA_LIGHT_GREY);
        print_string(builtin_commands[i].name, VGA_LIGHT_YELLOW);
        print_string(" - ", VGA_LIGHT_GREY);
        print_string(builtin_commands[i].description, VGA_LIGHT_WHITE);
        print_string("\n", VGA_LIGHT_GREY);
    }
    
    print_string("\nAI Commands:\n", VGA_LIGHT_GREEN);
    print_string("  ai <message> - Chat with AI\n", VGA_LIGHT_WHITE);
    print_string("  model <type> - Set AI model (openai, claude, gemini, local)\n", VGA_LIGHT_WHITE);
    print_string("  setkey <key> - Set API key for AI models\n", VGA_LIGHT_WHITE);
    print_string("  reset - Clear AI conversation history\n", VGA_LIGHT_WHITE);
    
    print_string("\nVoice Assistant Commands:\n", VGA_LIGHT_GREEN);
    print_string("  voice - Voice assistant controls\n", VGA_LIGHT_WHITE);
    print_string("  assistant - AI assistant system\n", VGA_LIGHT_WHITE);
    print_string("  Say 'Hey Proto' to activate voice commands\n", VGA_LIGHT_WHITE);
    
    print_string("\nInformation Commands:\n", VGA_LIGHT_GREEN);
    print_string("  search <query> - Web search\n", VGA_LIGHT_WHITE);
    print_string("  weather <location> - Weather information\n", VGA_LIGHT_WHITE);
    print_string("  news <topic> - Latest news\n", VGA_LIGHT_WHITE);
    
    print_string("\nAvailable AI Models:\n", VGA_LIGHT_GREEN);
    print_string("  openai - OpenAI GPT models\n", VGA_LIGHT_WHITE);
    print_string("  claude - Anthropic Claude models\n", VGA_LIGHT_WHITE);
    print_string("  gemini - Google Gemini models (default)\n", VGA_LIGHT_WHITE);
    print_string("  local - Local LLM models\n", VGA_LIGHT_WHITE);
    
    print_string("\nEnvironment:\n", VGA_LIGHT_GREEN);
    print_string("  env - Show environment variables\n", VGA_LIGHT_WHITE);
    print_string("  API keys are automatically loaded from .env file\n", VGA_LIGHT_WHITE);
    
    print_string("\nMouse Commands:\n", VGA_LIGHT_GREEN);
    print_string("  mouse status - Show mouse status\n", VGA_LIGHT_WHITE);
    print_string("  mouse show - Show mouse cursor\n", VGA_LIGHT_WHITE);
    print_string("  mouse hide - Hide mouse cursor\n", VGA_LIGHT_WHITE);
    print_string("  mouse pos - Show mouse position\n", VGA_LIGHT_WHITE);
    
    return 0;
}

int cmd_clear(int argc, char* argv[]) {
    clear_screen();
    return 0;
}

int cmd_ai(int argc, char* argv[]) {
    if (argc < 2) {
        print_string("Usage: ai <message>\n", VGA_LIGHT_RED);
        print_string("Example: ai What is ProtoOS?\n", VGA_LIGHT_GREY);
        return 1;
    }
    
    // Combine all arguments into one message
    char message[MAX_COMMAND_LENGTH];
    strcpy(message, argv[1]);
    
    for (int i = 2; i < argc; i++) {
        strcat(message, " ");
        strcat(message, argv[i]);
    }
    
    print_string("AI: ", VGA_LIGHT_GREEN);
    
    // Get AI response
    char response[MAX_RESPONSE_LENGTH];
    if (langchain_chat(&ai_session, message, response, MAX_RESPONSE_LENGTH)) {
        print_string(response, VGA_LIGHT_WHITE);
    } else {
        print_string("Sorry, I couldn't process your request.", VGA_LIGHT_RED);
    }
    
    print_string("\n", VGA_LIGHT_WHITE);
    return 0;
}

int cmd_chat(int argc, char* argv[]) {
    return cmd_ai(argc, argv);
}

int cmd_model(int argc, char* argv[]) {
    if (argc != 2) {
        print_string("Usage: model <type>\n", VGA_LIGHT_RED);
        print_string("Types: openai, claude, gemini, local\n", VGA_LIGHT_GREY);
        return 1;
    }
    
    if (strcmp(argv[1], "openai") == 0) {
        langchain_set_model(&ai_session, MODEL_OPENAI_GPT, "gpt-3.5-turbo");
        print_string("AI model set to OpenAI GPT\n", VGA_LIGHT_GREEN);
    } else if (strcmp(argv[1], "claude") == 0) {
        langchain_set_model(&ai_session, MODEL_ANTHROPIC_CLAUDE, "claude-3-sonnet");
        print_string("AI model set to Anthropic Claude\n", VGA_LIGHT_GREEN);
    } else if (strcmp(argv[1], "gemini") == 0) {
        langchain_set_model(&ai_session, MODEL_GOOGLE_GEMINI, "gemini-pro");
        print_string("AI model set to Google Gemini\n", VGA_LIGHT_GREEN);
    } else if (strcmp(argv[1], "local") == 0) {
        langchain_set_model(&ai_session, MODEL_LOCAL_LLM, "local-llm");
        print_string("AI model set to Local LLM\n", VGA_LIGHT_GREEN);
    } else {
        print_string("Unknown model type. Use: openai, claude, gemini, or local\n", VGA_LIGHT_RED);
        return 1;
    }
    
    return 0;
}

int cmd_set_api_key(int argc, char* argv[]) {
    if (argc != 2) {
        print_string("Usage: setkey <api_key>\n", VGA_LIGHT_RED);
        print_string("Example: setkey your_actual_api_key_here\n", VGA_LIGHT_GREY);
        return 1;
    }
    
    // Update the API key in the session
    strncpy(ai_session.api_key, argv[1], MAX_API_KEY_LENGTH - 1);
    ai_session.api_key[MAX_API_KEY_LENGTH - 1] = '\0';
    
    print_string("API key updated successfully!\n", VGA_LIGHT_GREEN);
    print_string("Current model: ", VGA_LIGHT_CYAN);
    print_string(ai_session.model_name, VGA_LIGHT_WHITE);
    print_string("\n", VGA_LIGHT_WHITE);
    
    return 0;
}

int cmd_env(int argc, char* argv[]) {
    print_environment();
    return 0;
}

int cmd_voice(int argc, char* argv[]) {
    if (argc < 2) {
        print_string("Voice Assistant Commands:\n", VGA_LIGHT_CYAN);
        print_string("  voice start - Start voice listening\n", VGA_LIGHT_WHITE);
        print_string("  voice stop - Stop voice listening\n", VGA_LIGHT_WHITE);
        print_string("  voice status - Show voice system status\n", VGA_LIGHT_WHITE);
        print_string("  voice help - Show voice command help\n", VGA_LIGHT_WHITE);
        return 0;
    }
    
    if (strcmp(argv[1], "start") == 0) {
        start_voice_listening();
    } else if (strcmp(argv[1], "stop") == 0) {
        print_string("Voice listening stopped\n", VGA_LIGHT_YELLOW);
    } else if (strcmp(argv[1], "status") == 0) {
        print_string("Voice system is active and ready\n", VGA_LIGHT_GREEN);
    } else if (strcmp(argv[1], "help") == 0) {
        print_assistant_help();
    } else {
        print_string("Unknown voice command. Use: start, stop, status, help\n", VGA_LIGHT_RED);
        return 1;
    }
    
    return 0;
}

int cmd_assistant(int argc, char* argv[]) {
    if (argc < 2) {
        print_string("AI Assistant Commands:\n", VGA_LIGHT_CYAN);
        print_string("  assistant start - Start AI assistant\n", VGA_LIGHT_WHITE);
        print_string("  assistant stop - Stop AI assistant\n", VGA_LIGHT_WHITE);
        print_string("  assistant status - Show assistant status\n", VGA_LIGHT_WHITE);
        print_string("  assistant help - Show assistant help\n", VGA_LIGHT_WHITE);
        return 0;
    }
    
    if (strcmp(argv[1], "start") == 0) {
        start_assistant();
    } else if (strcmp(argv[1], "stop") == 0) {
        stop_assistant();
    } else if (strcmp(argv[1], "status") == 0) {
        assistant_system_t status;
        if (get_assistant_status(&status)) {
            print_string("Assistant Status:\n", VGA_LIGHT_CYAN);
            print_string("  Active: ", VGA_LIGHT_WHITE);
            print_string(status.is_active ? "Yes" : "No", status.is_active ? VGA_LIGHT_GREEN : VGA_LIGHT_RED);
            print_string("\n  Mode: ", VGA_LIGHT_WHITE);
            print_string(status.mode == ASSISTANT_MODE_ALWAYS_ON ? "Always-On" : "Other", VGA_LIGHT_CYAN);
            print_string("\n  Voice: ", VGA_LIGHT_WHITE);
            print_string(status.voice_enabled ? "Enabled" : "Disabled", status.voice_enabled ? VGA_LIGHT_GREEN : VGA_LIGHT_RED);
            print_string("\n", VGA_LIGHT_WHITE);
        }
    } else if (strcmp(argv[1], "help") == 0) {
        print_assistant_help();
    } else {
        print_string("Unknown assistant command. Use: start, stop, status, help\n", VGA_LIGHT_RED);
        return 1;
    }
    
    return 0;
}

int cmd_search(int argc, char* argv[]) {
    if (argc < 2) {
        print_string("Usage: search <query>\n", VGA_LIGHT_RED);
        print_string("Example: search weather in London\n", VGA_LIGHT_GREY);
        return 1;
    }
    
    // Combine all arguments into one query
    char query[MAX_COMMAND_LENGTH];
    strcpy(query, argv[1]);
    
    for (int i = 2; i < argc; i++) {
        strcat(query, " ");
        strcat(query, argv[i]);
    }
    
    // Perform web search
    search_result_t results[3];
    int num_results = perform_web_search(query, results, 3);
    
    if (num_results > 0) {
        print_string("Search Results:\n", VGA_LIGHT_GREEN);
        for (int i = 0; i < num_results; i++) {
            print_string("  ", VGA_LIGHT_GREY);
            print_string(results[i].title, VGA_LIGHT_YELLOW);
            print_string("\n    ", VGA_LIGHT_GREY);
            print_string(results[i].snippet, VGA_LIGHT_WHITE);
            print_string("\n    URL: ", VGA_LIGHT_CYAN);
            print_string(results[i].url, VGA_LIGHT_WHITE);
            print_string("\n", VGA_LIGHT_WHITE);
        }
    } else {
        print_string("No search results found.\n", VGA_LIGHT_RED);
    }
    
    return 0;
}

int cmd_weather(int argc, char* argv[]) {
    char location[64] = "Current Location";
    
    if (argc >= 2) {
        strncpy(location, argv[1], 63);
        location[63] = '\0';
    }
    
    weather_info_t weather;
    if (search_weather(location, &weather)) {
        print_string("Weather for ", VGA_LIGHT_CYAN);
        print_string(location, VGA_LIGHT_WHITE);
        print_string(":\n", VGA_LIGHT_WHITE);
        print_string("  Condition: ", VGA_LIGHT_GREEN);
        print_string(weather.condition, VGA_LIGHT_WHITE);
        print_string("\n  Temperature: ", VGA_LIGHT_GREEN);
        
        char temp_str[16];
        snprintf(temp_str, sizeof(temp_str), "%.1f°C", weather.temperature);
        print_string(temp_str, VGA_LIGHT_WHITE);
        
        print_string("\n  Humidity: ", VGA_LIGHT_GREEN);
        
        char hum_str[16];
        snprintf(hum_str, sizeof(hum_str), "%d%%", weather.humidity);
        print_string(hum_str, VGA_LIGHT_WHITE);
        
        print_string("\n  Forecast: ", VGA_LIGHT_GREEN);
        print_string(weather.forecast, VGA_LIGHT_WHITE);
        print_string("\n", VGA_LIGHT_WHITE);
    } else {
        print_string("Failed to get weather information.\n", VGA_LIGHT_RED);
        return 1;
    }
    
    return 0;
}

int cmd_news(int argc, char* argv[]) {
    char topic[64] = "Technology";
    
    if (argc >= 2) {
        strncpy(topic, argv[1], 63);
        topic[63] = '\0';
    }
    
    news_item_t news[2];
    int num_items = search_news(topic, news, 2);
    
    if (num_items > 0) {
        print_string("Latest news about ", VGA_LIGHT_CYAN);
        print_string(topic, VGA_LIGHT_WHITE);
        print_string(":\n", VGA_LIGHT_WHITE);
        
        for (int i = 0; i < num_items; i++) {
            print_string("  ", VGA_LIGHT_GREY);
            print_string(news[i].headline, VGA_LIGHT_YELLOW);
            print_string("\n    Source: ", VGA_LIGHT_CYAN);
            print_string(news[i].source, VGA_LIGHT_WHITE);
            print_string("\n    Summary: ", VGA_LIGHT_GREEN);
            print_string(news[i].summary, VGA_LIGHT_WHITE);
            print_string("\n    URL: ", VGA_LIGHT_CYAN);
            print_string(news[i].url, VGA_LIGHT_WHITE);
            print_string("\n", VGA_LIGHT_WHITE);
        }
    } else {
        print_string("No news found for the specified topic.\n", VGA_LIGHT_RED);
        return 1;
    }
    
    return 0;
}

int cmd_history(int argc, char* argv[]) {
    print_string("Command history:\n", VGA_LIGHT_CYAN);
    
    for (int i = 0; i < history_count; i++) {
        int idx = (history_index - history_count + i + MAX_HISTORY) % MAX_HISTORY;
        print_string("  ", VGA_LIGHT_GREY);
        
        char num_str[16];
        int temp = i + 1;
        int j = 0;
        do {
            num_str[j++] = '0' + (temp % 10);
            temp /= 10;
        } while (temp > 0);
        
        while (j > 0) {
            print_char(num_str[--j], VGA_LIGHT_BLUE);
        }
        
        print_string(": ", VGA_LIGHT_GREY);
        print_string(command_history[idx], VGA_LIGHT_WHITE);
        print_string("\n", VGA_LIGHT_GREY);
    }
    
    return 0;
}

int cmd_reset(int argc, char* argv[]) {
    langchain_clear_history(&ai_session);
    print_string("AI conversation history cleared\n", VGA_LIGHT_GREEN);
    return 0;
}

int cmd_mouse(int argc, char* argv[]) {
    if (argc < 2) {
        print_string("Mouse Commands:\n", VGA_LIGHT_CYAN);
        print_string("  mouse status - Show mouse status\n", VGA_LIGHT_WHITE);
        print_string("  mouse show - Show mouse cursor\n", VGA_LIGHT_WHITE);
        print_string("  mouse hide - Hide mouse cursor\n", VGA_LIGHT_WHITE);
        print_string("  mouse pos - Show mouse position\n", VGA_LIGHT_WHITE);
        return 0;
    }
    
    if (strcmp(argv[1], "status") == 0) {
        print_string("Mouse Status:\n", VGA_LIGHT_CYAN);
        print_string("  Driver: PS/2 Mouse Driver\n", VGA_LIGHT_WHITE);
        print_string("  Visible: ", VGA_LIGHT_WHITE);
        print_string(mouse_state.visible ? "Yes" : "No", mouse_state.visible ? VGA_LIGHT_GREEN : VGA_LIGHT_RED);
        print_string("\n  Left Button: ", VGA_LIGHT_WHITE);
        print_string(mouse_state.left_button ? "Pressed" : "Released", mouse_state.left_button ? VGA_LIGHT_GREEN : VGA_LIGHT_GREY);
        print_string("\n  Right Button: ", VGA_LIGHT_WHITE);
        print_string(mouse_state.right_button ? "Pressed" : "Released", mouse_state.right_button ? VGA_LIGHT_GREEN : VGA_LIGHT_GREY);
        print_string("\n  Middle Button: ", VGA_LIGHT_WHITE);
        print_string(mouse_state.middle_button ? "Pressed" : "Released", mouse_state.middle_button ? VGA_LIGHT_GREEN : VGA_LIGHT_GREY);
        print_string("\n", VGA_LIGHT_WHITE);
    } else if (strcmp(argv[1], "show") == 0) {
        show_mouse_cursor();
        print_string("Mouse cursor shown\n", VGA_LIGHT_GREEN);
    } else if (strcmp(argv[1], "hide") == 0) {
        hide_mouse_cursor();
        print_string("Mouse cursor hidden\n", VGA_LIGHT_YELLOW);
    } else if (strcmp(argv[1], "pos") == 0) {
        int x, y;
        get_mouse_position(&x, &y);
        print_string("Mouse position: ", VGA_LIGHT_CYAN);
        
        char pos_str[32];
        int temp = x;
        int i = 0;
        do {
            pos_str[i++] = '0' + (temp % 10);
            temp /= 10;
        } while (temp > 0);
        
        while (i > 0) {
            print_char(pos_str[--i], VGA_LIGHT_WHITE);
        }
        
        print_string(", ", VGA_LIGHT_GREY);
        
        temp = y;
        i = 0;
        do {
            pos_str[i++] = '0' + (temp % 10);
            temp /= 10;
        } while (temp > 0);
        
        while (i > 0) {
            print_char(pos_str[--i], VGA_LIGHT_WHITE);
        }
        
        print_string("\n", VGA_LIGHT_WHITE);
    } else {
        print_string("Unknown mouse command. Use: status, show, hide, pos\n", VGA_LIGHT_RED);
        return 1;
    }
    
    return 0;
}

int cmd_exit(int argc, char* argv[]) {
    print_string("Exiting shell...\n", VGA_LIGHT_YELLOW);
    return -1; // Signal to exit
}
