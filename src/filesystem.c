#include "filesystem.h"
#include <stdint.h>

/* Forward declaration */
int strcmp(const char *a, const char *b);

/* Minimal in-memory filesystem (simplified, no real FAT) */
static vfile_t fs_files[32];
static int fs_file_count = 0;

void fs_init(void) {
    fs_file_count = 0;
}

int fs_open(const char *name, vfile_t *f) {
    for (int i = 0; i < fs_file_count; ++i) {
        if (strcmp(fs_files[i].name, name) == 0) {
            *f = fs_files[i];
            return 0;
        }
    }
    return -1; /* not found */
}

int fs_read(vfile_t *f, uint8_t *buf, uint32_t off, uint32_t len) {
    if (off >= f->size) return 0;
    uint32_t to_read = len;
    if (off + len > f->size) to_read = f->size - off;
    /* In real FS, read from disk using cluster; here stub */
    return to_read;
}

int fs_list(vfile_t *files, int max) {
    int cnt = fs_file_count < max ? fs_file_count : max;
    for (int i = 0; i < cnt; ++i) files[i] = fs_files[i];
    return cnt;
}


