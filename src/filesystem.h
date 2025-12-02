#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdint.h>

/* Minimal FAT12/16 filesystem (enough to store files) */
typedef struct {
    char name[64];
    uint32_t size;
    uint32_t cluster;
} vfile_t;

void fs_init(void);
int fs_open(const char *name, vfile_t *f);
int fs_read(vfile_t *f, uint8_t *buf, uint32_t off, uint32_t len);
int fs_write(const char *name, const uint8_t *data, uint32_t len);
int fs_list(vfile_t *files, int max);

#endif
