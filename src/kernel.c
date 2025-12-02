#include <stdint.h>
#include "drivers.h"
#include "filesystem.h"
#include "efh.h"
#include "hpm.h"

/* Forward declarations */
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, int n);
int strlen(const char *s);
const char *strstr(const char *haystack, const char *needle);
void itoa(int val, char *buf, int base);

/* HubbleOS Kernel: Full-featured with drivers, filesystem, package manager, shell */

static volatile uint16_t *VGA = (uint16_t*)0xB8000;
static int vga_row = 0;
static int vga_col = 0;

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a" (val), "Nd" (port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a" (ret) : "Nd" (port));
    return ret;
}

void putchar(char c) {
    if (c == '\n') {
        vga_col = 0;
        vga_row++;
    } else {
        uint16_t val = (uint16_t)c | (0x07 << 8);
        VGA[vga_row * 80 + vga_col] = val;
        vga_col++;
        if (vga_col >= 80) { vga_col = 0; vga_row++; }
    }
}

void puts(const char *s) {
    while (*s) putchar(*s++);
}

/* Very small scancode -> ascii map for letters, digits and basic symbols. */
static const char scancode_map[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=','\b', '\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'', '`', 0,
    '\\','z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ', 0
};

int read_line(char *buf, int max) {
    int i = 0;
    while (1) {
        uint8_t sc = inb(0x60);
        if (sc == 0) continue;
        if (sc & 0x80) continue; /* key release */
        char c = '?';
        if (sc < 128) c = scancode_map[sc];
        if (c == 0) continue;
        if (c == '\r' || c == '\n') {
            putchar('\n');
            buf[i] = '\0';
            return i;
        }
        if (i < max-1) {
            buf[i++] = c;
            putchar(c);
        }
    }
}

void kernel_main(void) {
    puts("========================================\n");
    puts("  HubbleOS v1.0 (x86 BIOS)\n");
    puts("  Executable Format: EFH\n");
    puts("  Package Manager: HPM\n");
    puts("========================================\n\n");

    /* Initialize subsystems */
    ata_init();
    virtio_net_init();
    pci_enumerate();
    fs_init();
    hpm_init();

    puts("Subsystems initialized.\n");
    puts("Type 'help' for commands.\n\n");

    char line[128];
    while (1) {
        puts("hubble> ");
        read_line(line, sizeof(line));
        
        if (strcmp(line, "help") == 0) {
            puts("========================================\n");
            puts("  HubbleOS Shell Commands\n");
            puts("========================================\n");
            puts("System:\n");
            puts("  help              - Show this help\n");
            puts("  echo <text>       - Echo text\n");
            puts("  ls                - List files\n");
            puts("  cat <file>        - Show file (stub)\n");
            puts("  clear             - Clear screen\n");
            puts("  reboot            - Reboot\n");
            puts("  exit              - Halt\n");
            puts("\nPackage Manager (HPM):\n");
            puts("  hpm list          - List all packages\n");
            puts("  hpm cat <cat>     - List packages by category\n");
            puts("  hpm search <q>    - Search for package\n");
            puts("  hpm info <pkg>    - Show package info\n");
            puts("  hpm install <pkg> - Install package\n");
            puts("  hpm installed     - List installed packages\n");
            puts("\nCategories: dev,lang,vcs,shell,util,net,sys,lib\n");
            puts("                lib,test,debug,doc,container\n");
            puts("========================================\n");
        } else if (strcmp(line, "help") == 0) {
            puts(line + 5);
            puts("\n");
        } else if (strncmp(line, "hpm list", 8) == 0) {
            puts("Available packages:\n");
            hpm_package_t pkgs[20];
            int cnt = hpm_list(pkgs, 20);
            for (int i = 0; i < cnt; ++i) {
                puts("  ");
                puts(pkgs[i].name);
                puts(" (");
                puts(pkgs[i].version);
                puts(") [");
                puts(pkgs[i].category);
                puts("]\n");
            }
            puts("\nTotal: ");
            itoa(cnt, (char*)VGA, 10);
            puts(" packages shown\n");
        } else if (strncmp(line, "hpm cat ", 8) == 0) {
            const char *cat = line + 8;
            puts("Packages in category: ");
            puts(cat);
            puts("\n");
            hpm_package_t pkgs[30];
            int cnt = hpm_list_by_category(cat, pkgs, 30);
            for (int i = 0; i < cnt; ++i) {
                puts("  ");
                puts(pkgs[i].name);
                puts("\n");
            }
            if (cnt == 0) puts("  No packages found\n");
        } else if (strncmp(line, "hpm search ", 11) == 0) {
            const char *query = line + 11;
            puts("Search results for: ");
            puts(query);
            puts("\n");
            hpm_package_t results[20];
            int cnt = hpm_search(query, results, 20);
            for (int i = 0; i < cnt; ++i) {
                puts("  ");
                puts(results[i].name);
                puts(" - ");
                puts(results[i].version);
                puts("\n");
            }
            if (cnt == 0) puts("  No results\n");
        } else if (strncmp(line, "hpm info ", 9) == 0) {
            const char *pkg = line + 9;
            hpm_package_t info;
            if (hpm_info(pkg, &info) == 0) {
                puts("Package: ");
                puts(info.name);
                puts("\n");
                puts("Version: ");
                puts(info.version);
                puts("\n");
                puts("Category: ");
                puts(info.category);
                puts("\n");
                puts("Size: ");
                itoa(info.size / 1024, (char*)VGA, 10);
                puts(" KB\n");
                puts("URL: ");
                puts(info.url);
                puts("\n");
            } else {
                puts("Package not found\n");
            }
        } else if (strncmp(line, "hpm install ", 12) == 0) {
            const char *pkg = line + 12;
            puts("Installing ");
            puts(pkg);
            puts("... ");
            if (hpm_install(pkg) == 0) {
                puts("Done!\n");
            } else {
                puts("Failed (not found)\n");
            }
        } else if (strcmp(line, "hpm installed") == 0) {
            puts("Installed packages:\n");
            hpm_installed_t installed[HPM_MAX_INSTALLED];
            int cnt = hpm_installed(installed, HPM_MAX_INSTALLED);
            if (cnt == 0) {
                puts("  (none)\n");
            } else {
                for (int i = 0; i < cnt; ++i) {
                    puts("  ");
                    puts(installed[i].name);
                    puts(" ");
                    puts(installed[i].version);
                    puts("\n");
                }
            }
        } else if (strcmp(line, "clear") == 0) {
            /* Clear VGA screen */
            for (int i = 0; i < 80 * 25; ++i) VGA[i] = 0x0700;
            vga_row = 0;
            vga_col = 0;
        } else if (strcmp(line, "ls") == 0) {
            puts("Files:\n");
            vfile_t files[32];
            int cnt = fs_list(files, 32);
            for (int i = 0; i < cnt; ++i) {
                puts("  ");
                puts(files[i].name);
                puts(" (");
                itoa(files[i].size, (char*)VGA, 10);
                puts(" bytes)\n");
            }
        } else if (strncmp(line, "cat ", 4) == 0) {
            puts("File contents of: ");
            puts(line + 4);
            puts(" (stub)\n");
        } else if (strcmp(line, "reboot") == 0) {
            puts("Rebooting...\n");
            outb(0x64, 0xFE);
        } else if (strcmp(line, "exit") == 0) {
            puts("System halted.\n");
            while (1) { __asm__ volatile ("hlt"); }
        } else if (strlen(line) > 0) {
            puts("Unknown command: ");
            puts(line);
            puts(" (type 'help' for commands)\n");
        }
    }
}
