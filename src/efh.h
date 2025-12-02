#ifndef EFH_H
#define EFH_H

#include <stdint.h>

/* EFH (Executable For HubbleOS) format:
   - 4-byte magic: 'EFH\0'
   - 4-byte version: 1
   - 4-byte entry_offset (offset to executable code)
   - 4-byte code_size
   - 4-byte data_offset
   - 4-byte data_size
   - followed by: metadata (name, author, description), code, data
*/

typedef struct {
    char magic[4];
    uint32_t version;
    uint32_t entry_offset;
    uint32_t code_size;
    uint32_t data_offset;
    uint32_t data_size;
    char name[32];
    char author[32];
    char desc[64];
} efh_header_t;

int efh_validate(const uint8_t *data, uint32_t len);
int efh_execute(const uint8_t *data, uint32_t len);
int efh_create(const char *name, const char *author, const uint8_t *code,
               uint32_t code_size, uint8_t *out, uint32_t max_out);

#endif
