#include "assistant.h"
#include "voice.h"
#include "langchain.h"
#include "network.h"
#include "screen.h"
#include <string.h>

// Global assistant system state
static assistant_system_t assistant_state;
static int assistant_initialized = 0;

// Initialize assistant system
void init_assistant_system() {
    if (assistant_initialized) return;
    
    // Initialize assistant state
    assistant_state.mode = ASSISTANT_MODE_ALWAYS_ON;
    assistant_state.capabilities = CAPABILITY_WEB_SEARCH | CAPABILITY_WEATHER | 
                                  CAPABILITY_NEWS | CAPABILITY_AI_CHAT | 
                                  CAPABILITY_VOICE_COMMANDS;
    assistant_state.is_active = 0;
    assistant_state.voice_enabled = 1;
    assistant_state.search_enabled = 1;
    assistant_state.weather_enabled = 1;
    assistant_state.news_enabled = 1;
    
    assistant_initialized = 1;
    
    print_string("ProtoOS AI Assistant System Initialized\n", VGA_LIGHT_GREEN);
    print_string("Capabilities: Web Search, Weather, News, AI Chat, Voice Commands\n", VGA_LIGHT_CYAN);
    print_string("Mode: Always-On Voice Assistant\n", VGA_LIGHT_YELLOW);
    print_string("Say 'Hey Proto' to activate voice commands\n", VGA_LIGHT_GREEN);
}

// Start the assistant
int start_assistant() {
    if (!assistant_initialized) {
        init_assistant_system();
    }
    
    assistant_state.is_active = 1;
    
    // Start voice system if enabled
    if (assistant_state.voice_enabled) {
        init_voice_system();
        start_voice_listening();
    }
    
    print_string("AI Assistant is now active and listening\n", VGA_LIGHT_GREEN);
    print_string("Voice commands: 'Hey Proto', 'Proto Assistant', 'Hey OS'\n", VGA_LIGHT_CYAN);
    
    return 1;
}

// Stop the assistant
int stop_assistant() {
    assistant_state.is_active = 0;
    print_string("AI Assistant stopped\n", VGA_LIGHT_YELLOW);
    return 1;
}

// Set assistant mode
int set_assistant_mode(int mode) {
    if (mode < 0 || mode > 2) return 0;
    
    assistant_state.mode = mode;
    
    switch (mode) {
        case ASSISTANT_MODE_VOICE:
            print_string("Assistant mode: Voice-Only\n", VGA_LIGHT_CYAN);
            break;
        case ASSISTANT_MODE_TEXT:
            print_string("Assistant mode: Text-Only\n", VGA_LIGHT_CYAN);
            break;
        case ASSISTANT_MODE_ALWAYS_ON:
            print_string("Assistant mode: Always-On Voice Assistant\n", VGA_LIGHT_CYAN);
            break;
    }
    
    return 1;
}

// Enable a capability
int enable_capability(int capability) {
    assistant_state.capabilities |= capability;
    
    if (capability & CAPABILITY_VOICE_COMMANDS) {
        assistant_state.voice_enabled = 1;
        print_string("Voice commands enabled\n", VGA_LIGHT_GREEN);
    }
    if (capability & CAPABILITY_WEB_SEARCH) {
        assistant_state.search_enabled = 1;
        print_string("Web search enabled\n", VGA_LIGHT_GREEN);
    }
    if (capability & CAPABILITY_WEATHER) {
        assistant_state.weather_enabled = 1;
        print_string("Weather service enabled\n", VGA_LIGHT_GREEN);
    }
    if (capability & CAPABILITY_NEWS) {
        assistant_state.news_enabled = 1;
        print_string("News service enabled\n", VGA_LIGHT_GREEN);
    }
    
    return 1;
}

// Disable a capability
int disable_capability(int capability) {
    assistant_state.capabilities &= ~capability;
    
    if (capability & CAPABILITY_VOICE_COMMANDS) {
        assistant_state.voice_enabled = 0;
        print_string("Voice commands disabled\n", VGA_LIGHT_YELLOW);
    }
    if (capability & CAPABILITY_WEB_SEARCH) {
        assistant_state.search_enabled = 0;
        print_string("Web search disabled\n", VGA_LIGHT_YELLOW);
    }
    if (capability & CAPABILITY_WEATHER) {
        assistant_state.weather_enabled = 0;
        print_string("Weather service disabled\n", VGA_LIGHT_YELLOW);
    }
    if (capability & CAPABILITY_NEWS) {
        assistant_state.news_enabled = 0;
        print_string("News service disabled\n", VGA_LIGHT_YELLOW);
    }
    
    return 1;
}

// Perform web search
int perform_web_search(const char* query, search_result_t* results, int max_results) {
    if (!query || !results || max_results <= 0) return 0;
    if (!assistant_state.search_enabled) return 0;
    
    print_string("Performing web search for: ", VGA_LIGHT_CYAN);
    print_string(query, VGA_LIGHT_WHITE);
    print_string("\n", VGA_LIGHT_WHITE);
    
    // In a real implementation, this would:
    // 1. Send search query to search engine API
    // 2. Parse search results
    // 3. Return structured results
    
    // Simulate search results
    if (max_results >= 1) {
        strncpy(results[0].title, "Search Results for: ", 127);
        strncat(results[0].title, query, 127 - strlen(results[0].title));
        strcpy(results[0].url, "https://search.protoos.com/results");
        strcpy(results[0].snippet, "Here are the search results for your query. In a real implementation, these would be actual web search results from Google, Bing, or other search engines.");
        results[0].relevance = 0.95f;
    }
    
    if (max_results >= 2) {
        strcpy(results[1].title, "Additional Information");
        strcpy(results[1].url, "https://info.protoos.com");
        strcpy(results[1].snippet, "More detailed information and related content would appear here based on your search query.");
        results[1].relevance = 0.87f;
    }
    
    print_string("Search completed. Found relevant results.\n", VGA_LIGHT_GREEN);
    return (max_results >= 2) ? 2 : 1;
}

// Search weather information
int search_weather(const char* location, weather_info_t* weather) {
    if (!location || !weather) return 0;
    if (!assistant_state.weather_enabled) return 0;
    
    print_string("Getting weather for: ", VGA_LIGHT_CYAN);
    print_string(location, VGA_LIGHT_WHITE);
    print_string("\n", VGA_LIGHT_WHITE);
    
    // In a real implementation, this would:
    // 1. Call weather API service
    // 2. Parse weather data
    // 3. Return current conditions and forecast
    
    // Simulate weather data
    strncpy(weather->location, location, 63);
    strcpy(weather->condition, "Sunny");
    weather->temperature = 22.5f;
    weather->humidity = 65;
    strcpy(weather->forecast, "Clear skies with light breeze. High 24°C, Low 18°C.");
    
    print_string("Weather: ", VGA_LIGHT_GREEN);
    print_string(weather->condition, VGA_LIGHT_WHITE);
    print_string(", ", VGA_LIGHT_WHITE);
    
    char temp_str[16];
    snprintf(temp_str, sizeof(temp_str), "%.1f°C", weather->temperature);
    print_string(temp_str, VGA_LIGHT_WHITE);
    print_string("\n", VGA_LIGHT_WHITE);
    
    return 1;
}

// Search news
int search_news(const char* topic, news_item_t* news, int max_items) {
    if (!topic || !news || max_items <= 0) return 0;
    if (!assistant_state.news_enabled) return 0;
    
    print_string("Getting news about: ", VGA_LIGHT_CYAN);
    print_string(topic, VGA_LIGHT_WHITE);
    print_string("\n", VGA_LIGHT_WHITE);
    
    // In a real implementation, this would:
    // 1. Call news API service
    // 2. Parse news articles
    // 3. Return relevant news items
    
    // Simulate news results
    if (max_items >= 1) {
        strncpy(news[0].headline, "Latest News About: ", 127);
        strncat(news[0].headline, topic, 127 - strlen(news[0].headline));
        strcpy(news[0].source, "ProtoOS News Service");
        strcpy(news[0].summary, "Here you would see the latest news articles related to your topic. In a real implementation, this would fetch from news APIs like NewsAPI, GNews, or similar services.");
        strcpy(news[0].url, "https://news.protoos.com");
        news[0].timestamp = 1234567890; // Simulated timestamp
    }
    
    print_string("News retrieved successfully.\n", VGA_LIGHT_GREEN);
    return 1;
}

// Process assistant query
int process_assistant_query(const char* query, char* response, int max_response) {
    if (!query || !response || max_response <= 0) return 0;
    
    print_string("Assistant Query: ", VGA_LIGHT_YELLOW);
    print_string(query, VGA_LIGHT_WHITE);
    print_string("\n", VGA_LIGHT_WHITE);
    
    // Check for different types of queries
    if (strstr(query, "search") || strstr(query, "find") || strstr(query, "look up")) {
        // Handle web search
        search_result_t results[2];
        int num_results = perform_web_search(query, results, 2);
        
        snprintf(response, max_response, "I found %d search results for your query. The top result is: %s", 
                num_results, results[0].title);
        return 1;
    }
    
    if (strstr(query, "weather") || strstr(query, "temperature")) {
        // Handle weather query
        weather_info_t weather;
        if (search_weather("Current Location", &weather)) {
            snprintf(response, max_response, "Current weather: %s, %.1f°C, %d%% humidity. %s", 
                    weather.condition, weather.temperature, weather.humidity, weather.forecast);
            return 1;
        }
    }
    
    if (strstr(query, "news") || strstr(query, "latest")) {
        // Handle news query
        news_item_t news[1];
        if (search_news("Technology", news)) {
            snprintf(response, max_response, "Latest news: %s", news[0].headline);
            return 1;
        }
    }
    
    if (strstr(query, "time") || strstr(query, "date")) {
        // Handle time/date query
        char time_str[32], date_str[32];
        get_current_time(time_str, sizeof(time_str));
        get_current_date(date_str, sizeof(date_str));
        
        snprintf(response, max_response, "Current time: %s, Date: %s", time_str, date_str);
        return 1;
    }
    
    // Default: treat as AI chat query
    strcpy(response, "I understand your query. Let me process this with my AI capabilities and provide you with a comprehensive answer.");
    return 1;
}

// Handle voice query
int handle_voice_query(const char* voice_input, char* response, int max_response) {
    if (!voice_input || !response || max_response <= 0) return 0;
    
    // First, try to recognize the voice command
    voice_command_t voice_cmd;
    if (recognize_speech_command(voice_input, &voice_cmd)) {
        // Handle recognized voice command
        if (voice_cmd.type == VOICE_CMD_SEARCH) {
            return process_assistant_query("search for information", response, max_response);
        } else if (voice_cmd.type == VOICE_CMD_AI_CHAT) {
            return process_assistant_query(voice_input, response, max_response);
        } else if (voice_cmd.type == VOICE_CMD_ASSISTANT) {
            strcpy(response, voice_cmd.response);
            return 1;
        }
    }
    
    // If not recognized, process as general query
    return process_assistant_query(voice_input, response, max_response);
}

// Get assistant status
int get_assistant_status(assistant_system_t* status) {
    if (!status) return 0;
    
    *status = assistant_state;
    return 1;
}

// Check if assistant is ready
int is_assistant_ready() {
    return assistant_initialized && assistant_state.is_active;
}

// Print assistant help
void print_assistant_help() {
    print_string("ProtoOS AI Assistant - Voice Commands\n", VGA_LIGHT_CYAN);
    print_string("=====================================\n", VGA_LIGHT_CYAN);
    print_string("\nWake Words:\n", VGA_LIGHT_YELLOW);
    print_string("  'Hey Proto' - Activate voice assistant\n", VGA_LIGHT_WHITE);
    print_string("  'Proto Assistant' - Alternative wake word\n", VGA_LIGHT_WHITE);
    print_string("  'Hey OS' - Another wake word option\n", VGA_LIGHT_WHITE);
    
    print_string("\nVoice Commands:\n", VGA_LIGHT_YELLOW);
    print_string("  'Search for [topic]' - Web search\n", VGA_LIGHT_WHITE);
    print_string("  'What's the weather' - Weather information\n", VGA_LIGHT_WHITE);
    print_string("  'What time is it' - Current time\n", VGA_LIGHT_WHITE);
    print_string("  'Tell me the news' - Latest news\n", VGA_LIGHT_WHITE);
    print_string("  'Ask AI [question]' - Chat with AI\n", VGA_LIGHT_WHITE);
    
    print_string("\nAssistant Modes:\n", VGA_LIGHT_YELLOW);
    print_string("  Voice-Only - Voice commands only\n", VGA_LIGHT_WHITE);
    print_string("  Text-Only - Text interface only\n", VGA_LIGHT_WHITE);
    print_string("  Always-On - Continuous voice listening\n", VGA_LIGHT_WHITE);
    
    print_string("\nCapabilities:\n", VGA_LIGHT_YELLOW);
    print_string("  Web Search - Search the internet\n", VGA_LIGHT_WHITE);
    print_string("  Weather - Get weather information\n", VGA_LIGHT_WHITE);
    print_string("  News - Latest news updates\n", VGA_LIGHT_WHITE);
    print_string("  AI Chat - Intelligent conversations\n", VGA_LIGHT_WHITE);
    print_string("  Voice Commands - Hands-free operation\n", VGA_LIGHT_WHITE);
}

// Get current time
int get_current_time(char* time_str, int max_length) {
    if (!time_str || max_length <= 0) return 0;
    
    // In a real OS, this would get system time
    // For now, simulate current time
    strncpy(time_str, "14:30:25", max_length - 1);
    time_str[max_length - 1] = '\0';
    
    return 1;
}

// Get current date
int get_current_date(char* date_str, int max_length) {
    if (!date_str || max_length <= 0) return 0;
    
    // In a real OS, this would get system date
    // For now, simulate current date
    strncpy(date_str, "December 15, 2024", max_length - 1);
    date_str[max_length - 1] = '\0';
    
    return 1;
}


