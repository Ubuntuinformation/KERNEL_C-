#include "efh.h"
#include <stdint.h>

int efh_validate(const uint8_t *data, uint32_t len) {
    if (len < sizeof(efh_header_t)) return -1;
    efh_header_t *h = (efh_header_t*)data;
    if (h->magic[0] != 'E' || h->magic[1] != 'F' || h->magic[2] != 'H' || h->magic[3] != 0)
        return -1;
    if (h->version != 1) return -1;
    return 0;
}

int efh_execute(const uint8_t *data, uint32_t len) {
    if (efh_validate(data, len) != 0) return -1;
    efh_header_t *h = (efh_header_t*)data;
    /* Call the entry point as a function pointer */
    typedef int (*entry_t)(void);
    entry_t entry = (entry_t)(data + h->entry_offset);
    return entry();
}

int efh_create(const char *name, const char *author, const uint8_t *code,
               uint32_t code_size, uint8_t *out, uint32_t max_out) {
    uint32_t total_size = sizeof(efh_header_t) + code_size;
    if (total_size > max_out) return -1;
    
    efh_header_t *h = (efh_header_t*)out;
    h->magic[0] = 'E'; h->magic[1] = 'F'; h->magic[2] = 'H'; h->magic[3] = 0;
    h->version = 1;
    h->entry_offset = sizeof(efh_header_t);
    h->code_size = code_size;
    h->data_offset = 0;
    h->data_size = 0;
    
    int i = 0;
    while (name[i] && i < 31) { h->name[i] = name[i]; i++; }
    h->name[i] = '\0';
    
    i = 0;
    while (author[i] && i < 31) { h->author[i] = author[i]; i++; }
    h->author[i] = '\0';
    
    void *memcpy(void *dest, const void *src, int n);
    memcpy(out + sizeof(efh_header_t), code, code_size);
    return total_size;
}
