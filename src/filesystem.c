#include "filesystem.h"
#include <stdint.h>

/* Forward declarations */
int strcmp(const char *a, const char *b);
int strlen(const char *s);
int strncmp(const char *a, const char *b, int n);
const char *strstr(const char *haystack, const char *needle);
void *memcpy(void *dest, const void *src, int n);

/* Virtual filesystem with directory support */
static vfile_t fs_files[64];
static int fs_file_count = 0;
static char fs_cwd[256] = "/";

void fs_init(void) {
    fs_file_count = 0;
    /* Create root directory */
    vfile_t root;
    root.is_dir = 1;
    root.is_executable = 0;
    root.size = 0;
    root.cluster = 0;
    root.permissions = 0755;
    for (int i = 0; i < 64; i++) root.name[i] = 0;
    root.name[0] = '/';
    fs_files[fs_file_count++] = root;
}

int fs_open(const char *name, vfile_t *f) {
    for (int i = 0; i < fs_file_count; ++i) {
        if (strcmp(fs_files[i].name, name) == 0) {
            *f = fs_files[i];
            return 0;
        }
    }
    return -1;
}

int fs_read(vfile_t *f, uint8_t *buf, uint32_t off, uint32_t len) {
    if (off >= f->size) return 0;
    uint32_t to_read = len;
    if (off + len > f->size) to_read = f->size - off;
    return to_read;
}

int fs_write(const char *name, const uint8_t *data, uint32_t len) {
    if (fs_file_count >= 64) return -1;
    for (int i = 0; i < fs_file_count; ++i) {
        if (strcmp(fs_files[i].name, name) == 0) {
            fs_files[i].size = len;
            return 0;
        }
    }
    vfile_t f;
    f.is_dir = 0;
    f.is_executable = 0;
    f.size = len;
    f.cluster = 0;
    f.permissions = 0644;
    int i = 0;
    while (i < 64 && name[i]) {
        f.name[i] = name[i];
        i++;
    }
    f.name[i] = '\0';
    fs_files[fs_file_count++] = f;
    return 0;
}

int fs_list(vfile_t *files, int max) {
    int cnt = fs_file_count < max ? fs_file_count : max;
    for (int i = 0; i < cnt; ++i) files[i] = fs_files[i];
    return cnt;
}

int fs_mkdir(const char *path) {
    if (fs_file_count >= 64) return -1;
    for (int i = 0; i < fs_file_count; ++i) {
        if (strcmp(fs_files[i].name, path) == 0) return -1;
    }
    vfile_t dir;
    dir.is_dir = 1;
    dir.is_executable = 0;
    dir.size = 0;
    dir.cluster = 0;
    dir.permissions = 0755;
    int i = 0;
    while (i < 64 && path[i]) {
        dir.name[i] = path[i];
        i++;
    }
    dir.name[i] = '\0';
    fs_files[fs_file_count++] = dir;
    return 0;
}

int fs_rmdir(const char *path) {
    for (int i = 0; i < fs_file_count; ++i) {
        if (strcmp(fs_files[i].name, path) == 0 && fs_files[i].is_dir) {
            for (int j = i; j < fs_file_count - 1; ++j) {
                fs_files[j] = fs_files[j + 1];
            }
            fs_file_count--;
            return 0;
        }
    }
    return -1;
}

int fs_rm(const char *path) {
    for (int i = 0; i < fs_file_count; ++i) {
        if (strcmp(fs_files[i].name, path) == 0 && !fs_files[i].is_dir) {
            for (int j = i; j < fs_file_count - 1; ++j) {
                fs_files[j] = fs_files[j + 1];
            }
            fs_file_count--;
            return 0;
        }
    }
    return -1;
}

int fs_touch(const char *path) {
    for (int i = 0; i < fs_file_count; ++i) {
        if (strcmp(fs_files[i].name, path) == 0) {
            return 0;
        }
    }
    return fs_write(path, (uint8_t*)"", 0);
}

int fs_mv(const char *src, const char *dst) {
    for (int i = 0; i < fs_file_count; ++i) {
        if (strcmp(fs_files[i].name, src) == 0) {
            int j = 0;
            while (j < 64 && dst[j]) {
                fs_files[i].name[j] = dst[j];
                j++;
            }
            fs_files[i].name[j] = '\0';
            return 0;
        }
    }
    return -1;
}

int fs_cp(const char *src, const char *dst) {
    vfile_t srcf;
    if (fs_open(src, &srcf) != 0) return -1;
    vfile_t dstf = srcf;
    int i = 0;
    while (i < 64 && dst[i]) {
        dstf.name[i] = dst[i];
        i++;
    }
    dstf.name[i] = '\0';
    if (fs_file_count >= 64) return -1;
    fs_files[fs_file_count++] = dstf;
    return 0;
}

int fs_chmod(const char *path, uint32_t perms) {
    for (int i = 0; i < fs_file_count; ++i) {
        if (strcmp(fs_files[i].name, path) == 0) {
            fs_files[i].permissions = perms;
            return 0;
        }
    }
    return -1;
}

const char *fs_pwd(void) {
    return fs_cwd;
}

int fs_cd(const char *path) {
    /* Simple cd without full path resolution */
    if (strcmp(path, "/") == 0) {
        fs_cwd[0] = '/';
        fs_cwd[1] = '\0';
        return 0;
    }
    if (strcmp(path, "..") == 0) {
        if (fs_cwd[1] != '\0') {
            for (int i = strlen(fs_cwd) - 1; i >= 0; --i) {
                if (fs_cwd[i] == '/') {
                    fs_cwd[i+1] = '\0';
                    if (i == 0) fs_cwd[1] = '\0';
                    return 0;
                }
            }
        }
        return 0;
    }
    vfile_t f;
    if (fs_open(path, &f) == 0 && f.is_dir) {
        int i = 0;
        while (i < 256 && path[i]) {
            fs_cwd[i] = path[i];
            i++;
        }
        fs_cwd[i] = '\0';
        return 0;
    }
    return -1;
}

int fs_exists(const char *path) {
    vfile_t f;
    return fs_open(path, &f) == 0 ? 1 : 0;
}


