#ifndef HPM_H
#define HPM_H

#include <stdint.h>

/* HPM (Hubble Package Manager) - Gerenciador de pacotes para HubbleOS */

#define HPM_MAX_PACKAGES 150
#define HPM_MAX_INSTALLED 50
#define HPM_MAX_DOWNLOADS 10

typedef struct {
    char name[32];
    char version[16];
    uint32_t size;
    char url[128];
    char category[16];  /* dev, lib, util, etc */
    char deps[64];      /* comma-separated dependencies */
} hpm_package_t;

typedef struct {
    char name[32];
    char version[16];
    uint32_t size;
} hpm_installed_t;

typedef struct {
    char filename[64];
    uint8_t data[262144]; /* 256KB buffer */
    uint32_t size;
    uint8_t is_downloaded;
} hpm_download_cache_t;

void hpm_init(void);
int hpm_install(const char *pkg_name);
int hpm_list(hpm_package_t *pkgs, int max);
int hpm_list_by_category(const char *cat, hpm_package_t *pkgs, int max);
int hpm_remove(const char *pkg_name);
int hpm_search(const char *query, hpm_package_t *results, int max);
int hpm_installed(hpm_installed_t *pkgs, int max);
int hpm_info(const char *pkg_name, hpm_package_t *info);

/* Download and convert functions */
int hpm_download(const char *pkg_name, hpm_download_cache_t *cache);
int hpm_convert_to_efh(const char *pkg_name, const uint8_t *binary_data, uint32_t size);
int hpm_list_downloads(hpm_download_cache_t *downloads, int max);
int hpm_extract_download(const char *filename, uint8_t *buf, int max);

#endif
