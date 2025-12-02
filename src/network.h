#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>

/* Simple HTTP downloader for package downloads */

typedef struct {
    uint8_t data[262144]; /* 256KB buffer for downloaded files */
    uint32_t size;
} http_response_t;

/* Download a file from URL and return in response buffer */
int http_get(const char *url, http_response_t *resp);

/* Extract filename from URL */
void url_get_filename(const char *url, char *buf, int max);

/* Check if URL is reachable */
int network_available(void);

#endif
