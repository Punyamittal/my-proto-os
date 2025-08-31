#ifndef NETWORK_H
#define NETWORK_H

// Network configuration
#define MAX_HTTP_REQUEST_SIZE 4096
#define MAX_HTTP_RESPONSE_SIZE 8192
#define MAX_URL_LENGTH 256

// HTTP methods
#define HTTP_GET 0
#define HTTP_POST 1

// HTTP status codes
#define HTTP_OK 200
#define HTTP_BAD_REQUEST 400
#define HTTP_UNAUTHORIZED 401
#define HTTP_NOT_FOUND 404
#define HTTP_INTERNAL_ERROR 500

// Network functions
void init_network();
int http_request(const char* url, int method, const char* data, char* response, int max_response_size);
int parse_http_response(const char* response, char* body, int max_body_size);
int extract_status_code(const char* response);

// Simple HTTP client functions
int http_get(const char* url, char* response, int max_response_size);
int http_post(const char* url, const char* data, char* response, int max_response_size);

// URL parsing
int parse_url(const char* url, char* host, char* path, int* port);

#endif // NETWORK_H
