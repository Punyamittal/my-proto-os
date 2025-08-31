#ifndef SHELL_H
#define SHELL_H

// Shell configuration
#define MAX_COMMAND_LENGTH 256
#define MAX_ARGS 16
#define MAX_HISTORY 20
#define PROMPT_LENGTH 64

// Command structure
typedef struct {
    char name[32];
    char description[128];
    int (*function)(int argc, char* argv[]);
} shell_command_t;

// Shell functions
void init_shell();
void run_shell();
void process_command(const char* command_line);
void print_prompt();
void print_help();

// Built-in commands
int cmd_help(int argc, char* argv[]);
int cmd_clear(int argc, char* argv[]);
int cmd_ai(int argc, char* argv[]);
int cmd_model(int argc, char* argv[]);
int cmd_history(int argc, char* argv[]);
int cmd_reset(int argc, char* argv[]);
int cmd_exit(int argc, char* argv[]);

// AI-specific commands
int cmd_chat(int argc, char* argv[]);
int cmd_set_api_key(int argc, char* argv[]);
int cmd_set_model(int argc, char* argv[]);
int cmd_clear_history(int argc, char* argv[]);

#endif // SHELL_H
