#ifndef VOICE_H
#define VOICE_H

// Voice recognition and synthesis configuration
#define MAX_VOICE_COMMAND_LENGTH 512
#define MAX_SPEECH_OUTPUT_LENGTH 1024
#define MAX_AUDIO_BUFFER_SIZE 4096

// Voice command types
#define VOICE_CMD_SEARCH 0
#define VOICE_CMD_ASSISTANT 1
#define VOICE_CMD_SYSTEM 2
#define VOICE_CMD_AI_CHAT 3

// Voice command structure
typedef struct {
    int type;
    char command[MAX_VOICE_COMMAND_LENGTH];
    char response[MAX_SPEECH_OUTPUT_LENGTH];
    float confidence;
} voice_command_t;

// Voice recognition functions
void init_voice_system();
int start_voice_listening();
int process_voice_input(char* command, int max_length);
int recognize_speech_command(const char* audio_data, voice_command_t* result);

// Speech synthesis functions
int speak_text(const char* text);
int speak_response(const char* response);
void set_voice_speed(float speed);
void set_voice_pitch(float pitch);

// Voice assistant functions
int handle_voice_command(const char* command);
int is_wake_word_detected(const char* audio_data);
int is_voice_command_complete(const char* audio_data);

// Audio processing
int capture_audio(char* buffer, int max_size);
int play_audio(const char* buffer, int size);
void set_audio_volume(float volume);

// Wake word detection
#define WAKE_WORD "Hey Proto"
#define WAKE_WORD_ALT "Proto Assistant"
#define WAKE_WORD_ALT2 "Hey OS"

#endif // VOICE_H
