#ifndef HPM_H
#define HPM_H

#include <stdint.h>

/* HPM (Hubble Package Manager) - Gerenciador de pacotes para HubbleOS */

#define HPM_MAX_PACKAGES 150
#define HPM_MAX_INSTALLED 50

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

void hpm_init(void);
int hpm_install(const char *pkg_name);
int hpm_list(hpm_package_t *pkgs, int max);
int hpm_list_by_category(const char *cat, hpm_package_t *pkgs, int max);
int hpm_remove(const char *pkg_name);
int hpm_search(const char *query, hpm_package_t *results, int max);
int hpm_installed(hpm_installed_t *pkgs, int max);
int hpm_info(const char *pkg_name, hpm_package_t *info);

#endif
