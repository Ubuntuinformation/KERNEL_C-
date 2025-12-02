#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdint.h>

/* Virtual filesystem with directory support */
typedef struct {
    char name[64];
    uint32_t size;
    uint32_t cluster;
    uint8_t is_dir;
    uint8_t is_executable;
    uint32_t permissions; /* rwx for owner/group/other */
} vfile_t;

typedef struct {
    char path[256];
    uint8_t data[65536]; /* Max 64KB per file in memory */
} vfs_node_t;

void fs_init(void);
int fs_open(const char *name, vfile_t *f);
int fs_read(vfile_t *f, uint8_t *buf, uint32_t off, uint32_t len);
int fs_write(const char *name, const uint8_t *data, uint32_t len);
int fs_list(vfile_t *files, int max);
int fs_mkdir(const char *path);
int fs_rmdir(const char *path);
int fs_rm(const char *path);
int fs_touch(const char *path);
int fs_mv(const char *src, const char *dst);
int fs_cp(const char *src, const char *dst);
int fs_chmod(const char *path, uint32_t perms);
const char *fs_pwd(void);
int fs_cd(const char *path);
int fs_exists(const char *path);

#endif
