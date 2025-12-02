#include <stdint.h>
#include "drivers.h"
#include "filesystem.h"
#include "efh.h"
#include "hpm.h"
#include "network.h"

/* Forward declarations */
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, int n);
int strlen(const char *s);
const char *strstr(const char *haystack, const char *needle);
void itoa(int val, char *buf, int base);

/* HubbleOS Kernel v2.0: Complete Unix-like OS with HPM, EFH, Full commands */

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

/* Scancode -> ASCII mapping */
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
        if (sc & 0x80) continue;
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

/* Parse command arguments (simple whitespace split) */
int parse_args(const char *line, char **args, int max_args) {
    int argc = 0;
    int i = 0, j = 0;
    char arg[128];
    
    while (line[i] && argc < max_args) {
        if (line[i] == ' ' || line[i] == '\t') {
            if (j > 0) {
                arg[j] = '\0';
                int k = 0;
                while (k < 128 && arg[k]) {
                    args[argc][k] = arg[k];
                    k++;
                }
                args[argc][k] = '\0';
                argc++;
                j = 0;
            }
            i++;
        } else {
            arg[j++] = line[i++];
        }
    }
    if (j > 0 && argc < max_args) {
        arg[j] = '\0';
        int k = 0;
        while (k < 128 && arg[k]) {
            args[argc][k] = arg[k];
            k++;
        }
        args[argc][k] = '\0';
        argc++;
    }
    return argc;
}

void show_help(void) {
    puts("========================================\n");
    puts("  HubbleOS v2.0 - Unix-like Shell\n");
    puts("========================================\n");
    puts("\nSystem Commands:\n");
    puts("  help              - Show this help\n");
    puts("  clear             - Clear screen\n");
    puts("  pwd               - Print working directory\n");
    puts("  echo <text>       - Echo text\n");
    puts("  reboot            - Reboot system\n");
    puts("  exit              - Halt system\n");
    
    puts("\nFile Management:\n");
    puts("  ls                - List files/dirs\n");
    puts("  ls <dir>          - List specific directory\n");
    puts("  cat <file>        - Show file contents\n");
    puts("  touch <file>      - Create empty file\n");
    puts("  rm <file>         - Remove file\n");
    puts("  mv <src> <dst>    - Move/rename file\n");
    puts("  cp <src> <dst>    - Copy file\n");
    puts("  chmod <mode> <f>  - Change permissions\n");
    
    puts("\nDirectory Management:\n");
    puts("  cd <dir>          - Change directory\n");
    puts("  cd ..             - Go to parent\n");
    puts("  cd /              - Go to root\n");
    puts("  mkdir <name>      - Create directory\n");
    puts("  rmdir <name>      - Remove directory\n");
    
    puts("\nPackage Manager (HPM):\n");
    puts("  hpm list          - List all packages\n");
    puts("  hpm cat <cat>     - List by category\n");
    puts("  hpm search <q>    - Search packages\n");
    puts("  hpm info <pkg>    - Show package info\n");
    puts("  hpm install <pkg> - Install package\n");
    puts("  hpm installed     - Installed packages\n");
    puts("  hpm download <p>  - Download package\n");
    puts("  hpm convert <p>   - Convert to .efh\n");
    puts("  hpm cache         - List downloads\n");
    
    puts("\nCategories: dev,lang,vcs,shell,util,net,\n");
    puts("            sys,lib,test,debug,doc,container,media\n");
    puts("========================================\n");
}

void kernel_main(void) {
    puts("========================================\n");
    puts("  HubbleOS v2.0 (x86 BIOS 32-bit)\n");
    puts("  Executable Format: EFH\n");
    puts("  Package Manager: HPM\n");
    puts("  Shell: Unix-like Commands\n");
    puts("========================================\n\n");

    /* Initialize subsystems */
    ata_init();
    virtio_net_init();
    pci_enumerate();
    fs_init();
    hpm_init();

    puts("Subsystems initialized.\n");
    puts("Type 'help' for commands.\n\n");

    char line[256];
    char *args[16];
    char arg_bufs[16][128];
    for (int i = 0; i < 16; i++) args[i] = arg_bufs[i];

    while (1) {
        puts("hubble> ");
        read_line(line, sizeof(line));
        
        int argc = parse_args(line, args, 16);
        if (argc == 0) continue;

        /* System commands */
        if (strcmp(args[0], "help") == 0) {
            show_help();
        } else if (strcmp(args[0], "pwd") == 0) {
            puts(fs_pwd());
            puts("\n");
        } else if (strcmp(args[0], "clear") == 0) {
            for (int i = 0; i < 80 * 25; ++i) VGA[i] = 0x0700;
            vga_row = 0;
            vga_col = 0;
        } else if (strcmp(args[0], "echo") == 0) {
            for (int i = 1; i < argc; i++) {
                puts(args[i]);
                if (i < argc - 1) puts(" ");
            }
            puts("\n");
        } else if (strcmp(args[0], "reboot") == 0) {
            puts("Rebooting...\n");
            outb(0x64, 0xFE);
        } else if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "halt") == 0) {
            puts("System halted.\n");
            while (1) { __asm__ volatile ("hlt"); }
        }
        
        /* Directory commands */
        else if (strcmp(args[0], "cd") == 0) {
            if (argc < 2) {
                fs_cd("/");
            } else {
                if (fs_cd(args[1]) != 0) {
                    puts("cd: ");
                    puts(args[1]);
                    puts(": No such directory\n");
                }
            }
        } else if (strcmp(args[0], "mkdir") == 0) {
            if (argc < 2) {
                puts("mkdir: missing operand\n");
            } else {
                if (fs_mkdir(args[1]) != 0) {
                    puts("mkdir: ");
                    puts(args[1]);
                    puts(": Cannot create\n");
                } else {
                    puts("Directory created: ");
                    puts(args[1]);
                    puts("\n");
                }
            }
        } else if (strcmp(args[0], "rmdir") == 0) {
            if (argc < 2) {
                puts("rmdir: missing operand\n");
            } else {
                if (fs_rmdir(args[1]) != 0) {
                    puts("rmdir: ");
                    puts(args[1]);
                    puts(": Cannot remove\n");
                } else {
                    puts("Directory removed\n");
                }
            }
        }
        
        /* File commands */
        else if (strcmp(args[0], "ls") == 0) {
            puts("Files and Directories:\n");
            vfile_t files[32];
            int cnt = fs_list(files, 32);
            for (int i = 0; i < cnt; ++i) {
                if (files[i].is_dir) {
                    puts("  [DIR]  ");
                } else {
                    puts("  [FILE] ");
                }
                puts(files[i].name);
                puts(" (");
                itoa(files[i].size, (char*)VGA, 10);
                puts(" bytes)\n");
            }
        } else if (strcmp(args[0], "cat") == 0) {
            if (argc < 2) {
                puts("cat: missing filename\n");
            } else {
                vfile_t f;
                if (fs_open(args[1], &f) != 0) {
                    puts("cat: ");
                    puts(args[1]);
                    puts(": No such file\n");
                } else {
                    puts("File: ");
                    puts(f.name);
                    puts(" (");
                    itoa(f.size, (char*)VGA, 10);
                    puts(" bytes)\n");
                }
            }
        } else if (strcmp(args[0], "touch") == 0) {
            if (argc < 2) {
                puts("touch: missing filename\n");
            } else {
                fs_touch(args[1]);
                puts("File created: ");
                puts(args[1]);
                puts("\n");
            }
        } else if (strcmp(args[0], "rm") == 0) {
            if (argc < 2) {
                puts("rm: missing filename\n");
            } else {
                if (fs_rm(args[1]) != 0) {
                    puts("rm: ");
                    puts(args[1]);
                    puts(": No such file\n");
                } else {
                    puts("File deleted\n");
                }
            }
        } else if (strcmp(args[0], "mv") == 0) {
            if (argc < 3) {
                puts("mv: missing operands\n");
            } else {
                if (fs_mv(args[1], args[2]) != 0) {
                    puts("mv: cannot move ");
                    puts(args[1]);
                    puts("\n");
                } else {
                    puts("Moved: ");
                    puts(args[1]);
                    puts(" -> ");
                    puts(args[2]);
                    puts("\n");
                }
            }
        } else if (strcmp(args[0], "cp") == 0) {
            if (argc < 3) {
                puts("cp: missing operands\n");
            } else {
                if (fs_cp(args[1], args[2]) != 0) {
                    puts("cp: cannot copy ");
                    puts(args[1]);
                    puts("\n");
                } else {
                    puts("Copied: ");
                    puts(args[1]);
                    puts(" -> ");
                    puts(args[2]);
                    puts("\n");
                }
            }
        } else if (strcmp(args[0], "chmod") == 0) {
            if (argc < 3) {
                puts("chmod: missing operands\n");
            } else {
                uint32_t mode = 0;
                if (args[1][0] >= '0' && args[1][0] <= '9') {
                    mode = (args[1][0] - '0') * 64;
                    if (args[1][1]) mode += (args[1][1] - '0') * 8;
                    if (args[1][2]) mode += (args[1][2] - '0');
                }
                if (fs_chmod(args[2], mode) != 0) {
                    puts("chmod: ");
                    puts(args[2]);
                    puts(": No such file\n");
                } else {
                    puts("Permissions changed\n");
                }
            }
        }
        
        /* HPM Package Manager */
        else if (strcmp(args[0], "hpm") == 0) {
            if (argc < 2) {
                puts("hpm: subcommand required (list,cat,search,info,install,etc)\n");
            } else if (strcmp(args[1], "list") == 0) {
                puts("Available Packages:\n");
                hpm_package_t pkgs[50];
                int cnt = hpm_list(pkgs, 50);
                for (int i = 0; i < cnt; ++i) {
                    puts("  ");
                    puts(pkgs[i].name);
                    puts(" (");
                    puts(pkgs[i].version);
                    puts(") [");
                    puts(pkgs[i].category);
                    puts("]\n");
                }
                puts("Total: ");
                itoa(cnt, (char*)VGA, 10);
                puts(" packages\n");
            } else if (strcmp(args[1], "cat") == 0) {
                if (argc < 3) {
                    puts("hpm cat: category required\n");
                } else {
                    puts("Packages in category: ");
                    puts(args[2]);
                    puts("\n");
                    hpm_package_t pkgs[30];
                    int cnt = hpm_list_by_category(args[2], pkgs, 30);
                    for (int i = 0; i < cnt; ++i) {
                        puts("  ");
                        puts(pkgs[i].name);
                        puts(" (");
                        puts(pkgs[i].version);
                        puts(")\n");
                    }
                    if (cnt == 0) puts("  No packages found\n");
                }
            } else if (strcmp(args[1], "search") == 0) {
                if (argc < 3) {
                    puts("hpm search: query required\n");
                } else {
                    puts("Search results for: ");
                    puts(args[2]);
                    puts("\n");
                    hpm_package_t results[20];
                    int cnt = hpm_search(args[2], results, 20);
                    for (int i = 0; i < cnt; ++i) {
                        puts("  ");
                        puts(results[i].name);
                        puts(" - ");
                        puts(results[i].version);
                        puts("\n");
                    }
                    if (cnt == 0) puts("  No results\n");
                }
            } else if (strcmp(args[1], "info") == 0) {
                if (argc < 3) {
                    puts("hpm info: package name required\n");
                } else {
                    hpm_package_t info;
                    if (hpm_info(args[2], &info) == 0) {
                        puts("Package: ");
                        puts(info.name);
                        puts("\nVersion: ");
                        puts(info.version);
                        puts("\nCategory: ");
                        puts(info.category);
                        puts("\nSize: ");
                        itoa(info.size / 1024, (char*)VGA, 10);
                        puts(" KB\nURL: ");
                        puts(info.url);
                        puts("\nDependencies: ");
                        puts(info.deps);
                        puts("\n");
                    } else {
                        puts("Package not found\n");
                    }
                }
            } else if (strcmp(args[1], "install") == 0) {
                if (argc < 3) {
                    puts("hpm install: package name required\n");
                } else {
                    puts("Installing ");
                    puts(args[2]);
                    puts("... ");
                    if (hpm_install(args[2]) == 0) {
                        puts("Done!\n");
                    } else {
                        puts("Failed (not found)\n");
                    }
                }
            } else if (strcmp(args[1], "installed") == 0) {
                puts("Installed Packages:\n");
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
            } else if (strcmp(args[1], "download") == 0) {
                if (argc < 3) {
                    puts("hpm download: package name required\n");
                } else {
                    puts("Downloading ");
                    puts(args[2]);
                    puts("... ");
                    hpm_download_cache_t cache;
                    if (hpm_download(args[2], &cache) == 0) {
                        puts("Done! (");
                        itoa(cache.size, (char*)VGA, 10);
                        puts(" bytes)\n");
                    } else {
                        puts("Failed\n");
                    }
                }
            } else if (strcmp(args[1], "convert") == 0) {
                if (argc < 3) {
                    puts("hpm convert: package name required\n");
                } else {
                    puts("Converting ");
                    puts(args[2]);
                    puts(" to .efh... ");
                    hpm_download_cache_t cache;
                    if (hpm_download(args[2], &cache) == 0) {
                        hpm_convert_to_efh(args[2], cache.data, cache.size);
                        puts("Done!\n");
                    } else {
                        puts("Failed\n");
                    }
                }
            } else if (strcmp(args[1], "cache") == 0) {
                puts("Downloaded Packages:\n");
                hpm_download_cache_t downloads[HPM_MAX_DOWNLOADS];
                int cnt = hpm_list_downloads(downloads, HPM_MAX_DOWNLOADS);
                if (cnt == 0) {
                    puts("  (none)\n");
                } else {
                    for (int i = 0; i < cnt; ++i) {
                        puts("  ");
                        puts(downloads[i].filename);
                        puts(" (");
                        itoa(downloads[i].size, (char*)VGA, 10);
                        puts(" bytes)\n");
                    }
                }
            } else {
                puts("hpm: unknown subcommand: ");
                puts(args[1]);
                puts("\n");
            }
        } else if (strlen(line) > 0) {
            puts("Unknown command: ");
            puts(args[0]);
            puts(" (type 'help' for commands)\n");
        }
    }
}

