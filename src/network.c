#include "network.h"
#include <stdint.h>

/* Forward declarations */
int strcmp(const char *a, const char *b);
int strlen(const char *s);
const char *strstr(const char *haystack, const char *needle);

/* Stub network downloader - in real implementation would use Virtio-Net or QEMU networking */
int http_get(const char *url, http_response_t *resp) {
    /* This is a stub that simulates downloading.
       In production, would connect via Virtio network device or QEMU user-mode networking.
       For now, create a minimal valid EFH package locally. */
    
    if (resp == 0) return -1;
    
    /* Simulate small EFH executable */
    resp->data[0] = 'E';
    resp->data[1] = 'F';
    resp->data[2] = 'H';
    resp->data[3] = '\0';
    resp->data[4] = 0x01; /* version */
    resp->data[5] = 0x00;
    resp->data[6] = 0x00;
    resp->data[7] = 0x00;
    
    /* entry_offset at byte 8 */
    resp->data[8] = 0x00;
    resp->data[9] = 0x01;
    resp->data[10] = 0x00;
    resp->data[11] = 0x00;
    
    /* code_size at byte 12 */
    resp->data[12] = 0x00;
    resp->data[13] = 0x02;
    resp->data[14] = 0x00;
    resp->data[15] = 0x00;
    
    /* Fill rest with zeros and minimal code stub */
    for (int i = 16; i < 256; i++) resp->data[i] = 0;
    
    /* Minimal stub code */
    resp->data[256] = 0x90; /* nop */
    resp->data[257] = 0xC3; /* ret */
    
    resp->size = 258;
    return 0;
}

void url_get_filename(const char *url, char *buf, int max) {
    /* Extract filename from URL: http://example.com/path/file.efh -> file.efh */
    int i = strlen(url) - 1;
    int start = i;
    
    while (i > 0 && url[i] != '/') i--;
    if (url[i] == '/') i++;
    
    int j = 0;
    while (j < max - 1 && url[i]) {
        buf[j++] = url[i++];
    }
    buf[j] = '\0';
}

int network_available(void) {
    /* In production, would check Virtio-Net status */
    return 1; /* Always assume available for now */
}
