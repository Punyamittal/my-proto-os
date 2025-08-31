#include "network.h"
#include "screen.h"
#include <string.h>

// Network buffer
static char http_buffer[MAX_HTTP_RESPONSE_SIZE];

// Initialize network (placeholder for now)
void init_network() {
    // In a real OS, this would initialize network drivers
    // For now, we'll just print a message
    print_string("Network subsystem initialized (simulated)\n", VGA_LIGHT_CYAN);
}

// Parse URL into components
int parse_url(const char* url, char* host, char* path, int* port) {
    // Simple URL parser for http://host:port/path format
    if (strncmp(url, "http://", 7) != 0) {
        return -1; // Only HTTP supported for now
    }
    
    const char* host_start = url + 7;
    const char* colon = strchr(host_start, ':');
    const char* slash = strchr(host_start, '/');
    
    if (colon && slash && colon < slash) {
        // host:port/path
        int host_len = colon - host_start;
        strncpy(host, host_start, host_len);
        host[host_len] = '\0';
        
        int port_len = slash - colon - 1;
        char port_str[16];
        strncpy(port_str, colon + 1, port_len);
        port_str[port_len] = '\0';
        *port = atoi(port_str);
        
        strcpy(path, slash);
    } else if (slash) {
        // host/path (default port 80)
        int host_len = slash - host_start;
        strncpy(host, host_start, host_len);
        host[host_len] = '\0';
        *port = 80;
        strcpy(path, slash);
    } else {
        // just host (default port 80, path /)
        strcpy(host, host_start);
        *port = 80;
        strcpy(path, "/");
    }
    
    return 0;
}

// Extract HTTP status code from response
int extract_status_code(const char* response) {
    // Look for "HTTP/1.1 200" pattern
    const char* status_start = strstr(response, "HTTP/1.");
    if (!status_start) return -1;
    
    const char* code_start = strchr(status_start, ' ');
    if (!code_start) return -1;
    
    return atoi(code_start + 1);
}

// Parse HTTP response to extract body
int parse_http_response(const char* response, char* body, int max_body_size) {
    // Find double newline that separates headers from body
    const char* body_start = strstr(response, "\r\n\r\n");
    if (!body_start) {
        body_start = strstr(response, "\n\n");
        if (!body_start) return -1;
        body_start += 2;
    } else {
        body_start += 4;
    }
    
    int body_len = strlen(body_start);
    if (body_len >= max_body_size) {
        body_len = max_body_size - 1;
    }
    
    strncpy(body, body_start, body_len);
    body[body_len] = '\0';
    
    return body_len;
}

// Simulated HTTP request (in a real OS, this would use actual network drivers)
int http_request(const char* url, int method, const char* data, char* response, int max_response_size) {
    char host[128];
    char path[128];
    int port;
    
    if (parse_url(url, host, path, &port) != 0) {
        return -1;
    }
    
    // Simulate HTTP request
    if (method == HTTP_GET) {
        snprintf(response, max_response_size,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 45\r\n"
            "\r\n"
            "{\"message\": \"Simulated GET response\"}"
        );
    } else if (method == HTTP_POST) {
        snprintf(response, max_response_size,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 50\r\n"
            "\r\n"
            "{\"message\": \"Simulated POST response\"}"
        );
    }
    
    return 0;
}

// HTTP GET request
int http_get(const char* url, char* response, int max_response_size) {
    return http_request(url, HTTP_GET, NULL, response, max_response_size);
}

// HTTP POST request
int http_post(const char* url, const char* data, char* response, int max_response_size) {
    return http_request(url, HTTP_POST, data, response, max_response_size);
}

// Simple atoi implementation (since we don't have stdlib)
int atoi(const char* str) {
    int result = 0;
    int sign = 1;
    
    if (*str == '-') {
        sign = -1;
        str++;
    }
    
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return result * sign;
}

// Simple snprintf implementation
int snprintf(char* str, size_t size, const char* format, ...) {
    // Very basic implementation - just copy the format string
    // In a real OS, you'd implement proper formatting
    strncpy(str, format, size - 1);
    str[size - 1] = '\0';
    return strlen(str);
}
