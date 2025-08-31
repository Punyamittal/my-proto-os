#ifndef ASSISTANT_H
#define ASSISTANT_H

// Assistant system configuration
#define MAX_ASSISTANT_QUERY_LENGTH 512
#define MAX_SEARCH_RESULTS 10
#define MAX_WEATHER_INFO_LENGTH 256
#define MAX_NEWS_ITEM_LENGTH 512

// Assistant modes
#define ASSISTANT_MODE_VOICE 0
#define ASSISTANT_MODE_TEXT 1
#define ASSISTANT_MODE_ALWAYS_ON 2

// Assistant capabilities
#define CAPABILITY_WEB_SEARCH 0x01
#define CAPABILITY_WEATHER 0x02
#define CAPABILITY_NEWS 0x04
#define CAPABILITY_AI_CHAT 0x08
#define CAPABILITY_SYSTEM_CONTROL 0x10
#define CAPABILITY_VOICE_COMMANDS 0x20

// Search result structure
typedef struct {
    char title[128];
    char url[256];
    char snippet[256];
    float relevance;
} search_result_t;

// Weather information structure
typedef struct {
    char location[64];
    char condition[32];
    float temperature;
    int humidity;
    char forecast[128];
} weather_info_t;

// News item structure
typedef struct {
    char headline[128];
    char source[64];
    char summary[256];
    char url[256];
    long timestamp;
} news_item_t;

// Assistant system structure
typedef struct {
    int mode;
    int capabilities;
    int is_active;
    int voice_enabled;
    int search_enabled;
    int weather_enabled;
    int news_enabled;
} assistant_system_t;

// Assistant functions
void init_assistant_system();
int start_assistant();
int stop_assistant();
int set_assistant_mode(int mode);
int enable_capability(int capability);
int disable_capability(int capability);

// Web search functions
int perform_web_search(const char* query, search_result_t* results, int max_results);
int search_weather(const char* location, weather_info_t* weather);
int search_news(const char* topic, news_item_t* news, int max_items);

// Assistant interaction functions
int process_assistant_query(const char* query, char* response, int max_response);
int handle_voice_query(const char* voice_input, char* response, int max_response);
int get_assistant_status(assistant_system_t* status);

// Utility functions
int is_assistant_ready();
void print_assistant_help();
int get_current_time(char* time_str, int max_length);
int get_current_date(char* date_str, int max_length);

#endif // ASSISTANT_H
