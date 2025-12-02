#include "hpm.h"
#include "filesystem.h"
#include "efh.h"
#include <stdint.h>

/* Forward declarations */
int strcmp(const char *a, const char *b);
const char *strstr(const char *haystack, const char *needle);
int strlen(const char *s);

/* HPM built-in package database (150+ packages) */
static hpm_package_t hpm_packages[] = {
    /* Core development tools */
    {"gcc", "12.2.0", 52428800, "https://gcc.gnu.org/releases/gcc-12.2.0", "dev", "binutils,libc"},
    {"nasm", "2.16.01", 2097152, "https://www.nasm.us/nasm-2.16.01", "dev", "libc"},
    {"git", "2.40.0", 5242880, "https://github.com/git/git/releases/2.40.0", "vcs", "libssl,libcurl"},
    {"make", "4.3", 1048576, "https://gnu.org/make-4.3", "dev", "libc"},
    {"cmake", "3.26.0", 3145728, "https://cmake.org/3.26", "dev", "libc"},
    {"python3", "3.11.0", 20971520, "https://python.org/releases/3.11.0", "lang", "libssl"},
    {"perl", "5.36.0", 15728640, "https://perl.org/releases/5.36.0", "lang", "libc"},
    {"ruby", "3.2.0", 12582912, "https://ruby-lang.org/releases/3.2.0", "lang", "libc"},
    {"lua", "5.4.4", 524288, "https://lua.org/releases/5.4.4", "lang", "libc"},
    {"nodejs", "18.0.0", 25165824, "https://nodejs.org/releases/18.0.0", "lang", "libssl"},
    
    /* Build automation */
    {"autoconf", "2.71", 1048576, "https://gnu.org/autoconf-2.71", "dev", "m4"},
    {"automake", "1.16.5", 1048576, "https://gnu.org/automake-1.16.5", "dev", "perl"},
    {"libtool", "2.4.7", 1048576, "https://gnu.org/libtool-2.4.7", "dev", "bash"},
    {"ninja", "1.11.1", 524288, "https://ninja-build.org/releases/1.11.1", "dev", "libc"},
    {"scons", "4.5.2", 2097152, "https://scons.org/releases/4.5.2", "dev", "python3"},
    {"meson", "1.0.1", 1048576, "https://mesonbuild.com/1.0.1", "dev", "python3"},
    {"bazel", "6.0.0", 5242880, "https://bazel.build/releases/6.0.0", "dev", "java"},
    
    /* Debugging & profiling */
    {"gdb", "13.1", 5242880, "https://sourceware.org/gdb/releases/gdb-13.1", "debug", "libc"},
    {"lldb", "16.0.0", 4194304, "https://llvm.org/releases/16.0.0", "debug", "libllvm"},
    {"valgrind", "3.20.0", 4194304, "https://valgrind.org/releases/3.20.0", "debug", "libc"},
    {"strace", "6.2", 1048576, "https://strace.io/releases/6.2", "debug", "libc"},
    {"perf", "6.1", 1048576, "https://kernel.org/tools/perf/releases/6.1", "perf", "libc"},
    
    /* Version control */
    {"git", "2.40.0", 5242880, "https://github.com/git/git/releases/2.40.0", "vcs", "libssl"},
    {"svn", "1.14.2", 3145728, "https://subversion.apache.org/releases/1.14.2", "vcs", "libssl"},
    {"mercurial", "6.3.0", 2097152, "https://mercurial-scm.org/releases/6.3.0", "vcs", "python3"},
    {"fossil", "2.21", 1048576, "https://fossil-scm.org/releases/2.21", "vcs", "libc"},
    
    /* Text editors */
    {"vim", "9.0.0", 2097152, "https://vim.org/releases/9.0.0", "util", "libc"},
    {"nano", "7.2", 524288, "https://nano-editor.org/releases/7.2", "util", "libc"},
    {"emacs", "28.2", 10485760, "https://gnu.org/software/emacs/28.2", "util", "libc"},
    {"gedit", "43.0", 1048576, "https://wiki.gnome.org/Apps/Gedit/releases/43.0", "util", "libc"},
    
    /* Compression utilities */
    {"gzip", "1.12", 262144, "https://gnu.org/gzip/releases/1.12", "util", "libc"},
    {"bzip2", "1.0.8", 262144, "https://bzip.org/releases/1.0.8", "util", "libc"},
    {"xz", "5.4.0", 262144, "https://xz.tukaani.org/releases/5.4.0", "util", "libc"},
    {"zip", "3.0", 262144, "https://infozip.org/releases/zip-3.0", "util", "libc"},
    {"unzip", "6.0", 262144, "https://infozip.org/releases/unzip-6.0", "util", "libc"},
    {"7z", "22.01", 1048576, "https://7-zip.org/releases/22.01", "util", "libc"},
    {"rar", "6.1", 524288, "https://rar.rarlab.com/releases/6.1", "util", "libc"},
    
    /* Text processing */
    {"sed", "4.9", 262144, "https://gnu.org/sed/releases/4.9", "util", "libc"},
    {"awk", "5.1.0", 262144, "https://gnu.org/awk/releases/5.1.0", "util", "libc"},
    {"grep", "3.10", 262144, "https://gnu.org/grep/releases/3.10", "util", "libc"},
    {"jq", "1.7.0", 524288, "https://jqlang.org/releases/1.7.0", "util", "libc"},
    {"yq", "4.31.0", 262144, "https://github.com/mikefarah/yq/releases/4.31.0", "util", "libc"},
    
    /* File utilities */
    {"find", "4.9.0", 262144, "https://gnu.org/findutils/releases/4.9.0", "util", "libc"},
    {"tar", "1.34", 262144, "https://gnu.org/tar/releases/1.34", "util", "libc"},
    {"file", "5.44", 262144, "https://darwinsys.com/file/releases/5.44", "util", "libc"},
    {"locate", "4.9.0", 262144, "https://gnu.org/findutils/releases/4.9.0", "util", "libc"},
    
    /* Networking - HTTP/FTP */
    {"curl", "7.88.0", 1048576, "https://curl.se/releases/curl-7.88.0", "net", "libssl"},
    {"wget", "1.21.3", 524288, "https://gnu.org/wget/releases/1.21.3", "net", "libssl"},
    {"aria2", "1.36.0", 524288, "https://aria2.github.io/releases/1.36.0", "net", "libssl"},
    
    /* SSH/Secure shell */
    {"openssh", "9.0.0", 2097152, "https://openssh.com/releases/9.0.0", "net", "libssl"},
    {"putty", "0.78", 1048576, "https://putty.org/releases/0.78", "net", "libc"},
    
    /* Network analysis */
    {"tcpdump", "4.99.1", 524288, "https://tcpdump.org/releases/4.99.1", "net", "libpcap"},
    {"wireshark", "4.0.0", 5242880, "https://wireshark.org/download/releases/4.0.0", "net", "libpcap"},
    {"nmap", "7.93", 2097152, "https://nmap.org/releases/7.93", "net", "libssl"},
    
    /* Network utilities */
    {"netcat-openbsd", "1.10", 262144, "https://openbsd.org/releases/netcat", "net", "libc"},
    {"socat", "1.7.4.4", 262144, "https://sourceforge.net/projects/socat/1.7.4.4", "net", "libc"},
    {"telnet", "0.17", 262144, "https://linux.die.net/mirror/gnu/telnet-0.17", "net", "libc"},
    {"ftp", "0.17", 262144, "https://linux.die.net/mirror/gnu/ftp-0.17", "net", "libc"},
    {"rsync", "3.2.7", 524288, "https://rsync.samba.org/releases/3.2.7", "net", "libc"},
    
    /* DNS/Network info */
    {"bind-utils", "9.18.0", 1048576, "https://isc.org/releases/bind-9.18.0", "net", "libc"},
    {"iproute2", "6.1.0", 524288, "https://kernel.org/releases/iproute2-6.1.0", "net", "libc"},
    {"iptables", "1.8.9", 524288, "https://netfilter.org/releases/iptables-1.8.9", "net", "libc"},
    {"ethtool", "6.1", 262144, "https://kernel.org/releases/ethtool-6.1", "net", "libc"},
    
    /* System information */
    {"htop", "3.2.2", 262144, "https://htop.dev/releases/3.2.2", "sys", "libc"},
    {"iotop", "0.4.4", 262144, "https://github.com/ssimoesfelipe/iotop/releases/0.4.4", "sys", "python3"},
    {"nethogs", "0.8.7", 262144, "https://github.com/raboof/nethogs/releases/0.8.7", "sys", "libc"},
    {"iftop", "1.0", 262144, "https://code.blinkace.com/pdw/iftop/releases/1.0", "sys", "libpcap"},
    
    /* System utilities */
    {"cron", "3.0", 262144, "https://cron.sourceforge.io/releases/3.0", "sys", "libc"},
    {"tmux", "3.3.0", 524288, "https://github.com/tmux/tmux/releases/3.3.0", "sys", "libc"},
    {"screen", "4.9.0", 524288, "https://gnu.org/software/screen/releases/4.9.0", "sys", "libc"},
    {"systemd", "252.0", 5242880, "https://github.com/systemd/systemd/releases/v252.0", "sys", "libc"},
    
    /* Shells */
    {"bash", "5.2.0", 1048576, "https://gnu.org/software/bash/releases/5.2.0", "shell", "libc"},
    {"zsh", "5.9", 1048576, "https://zsh.org/releases/5.9", "shell", "libc"},
    {"fish", "3.5.1", 524288, "https://fishshell.com/releases/3.5.1", "shell", "libc"},
    {"ksh", "93u+", 524288, "https://github.com/ksh93/ksh/releases/93u+", "shell", "libc"},
    
    /* Package managers */
    {"apt", "2.5.0", 2097152, "https://packages.debian.org/releases/apt-2.5.0", "pm", "libc"},
    {"dpkg", "1.21.0", 1048576, "https://packages.debian.org/releases/dpkg-1.21.0", "pm", "libc"},
    
    /* C libraries */
    {"glibc", "2.37", 10485760, "https://gnu.org/software/libc/releases/2.37", "lib", ""},
    {"musl", "1.2.4", 524288, "https://musl.libc.org/releases/1.2.4", "lib", ""},
    {"libcpp", "13.0", 5242880, "https://gcc.gnu.org/releases/libstdc++-13.0", "lib", "glibc"},
    
    /* SSL/Encryption */
    {"openssl", "3.0.7", 5242880, "https://openssl.org/releases/openssl-3.0.7", "lib", "libc"},
    {"libssl", "3.0.7", 2097152, "https://openssl.org/releases/libssl-3.0.7", "lib", "libc"},
    {"libcrypto", "3.0.7", 2097152, "https://openssl.org/releases/libcrypto-3.0.7", "lib", "libc"},
    {"gnupg", "2.4.0", 2097152, "https://gnupg.org/releases/2.4.0", "sec", "libssl"},
    
    /* Network libraries */
    {"libcurl", "7.88.0", 1048576, "https://curl.se/releases/libcurl-7.88.0", "lib", "libssl"},
    {"libwget", "1.21.2", 524288, "https://savannah.gnu.org/projects/wget/", "lib", "libssl"},
    {"libssh2", "1.10.0", 524288, "https://libssh2.org/releases/1.10.0", "lib", "libcrypto"},
    
    /* Data & markup */
    {"libxml2", "2.10.3", 1048576, "https://xmlsoft.org/releases/2.10.3", "lib", "libc"},
    {"libjson-c", "0.16", 262144, "https://github.com/json-c/json-c/releases/0.16", "lib", "libc"},
    {"libyaml", "0.2.5", 262144, "https://pyyaml.org/releases/libyaml-0.2.5", "lib", "libc"},
    
    /* Compression */
    {"zlib", "1.2.13", 262144, "https://zlib.net/releases/zlib-1.2.13", "lib", "libc"},
    {"libbz2", "1.0.8", 262144, "https://bzip.org/releases/1.0.8", "lib", "libc"},
    {"libxz", "5.4.0", 262144, "https://xz.tukaani.org/releases/5.4.0", "lib", "libc"},
    
    /* Testing */
    {"gtest", "1.13.0", 524288, "https://github.com/google/googletest/releases/1.13.0", "test", "libcpp"},
    {"catch2", "3.3.2", 262144, "https://github.com/catchorg/Catch2/releases/3.3.2", "test", "libcpp"},
    
    /* Security scanning */
    {"aide", "0.18.0", 1048576, "https://aide.sourceforge.io/releases/0.18.0", "sec", "libc"},
    {"chkrootkit", "0.55", 1048576, "https://chkrootkit.org/releases/0.55", "sec", "libc"},
    
    /* Documentation */
    {"doxygen", "1.9.5", 2097152, "https://doxygen.org/releases/1.9.5", "doc", "libc"},
    {"sphinx", "5.3.0", 1048576, "https://sphinx-doc.org/releases/5.3.0", "doc", "python3"},
    {"pandoc", "3.1.0", 2097152, "https://pandoc.org/releases/3.1.0", "doc", "libc"},
    
    /* Containers & virtualization */
    {"docker", "20.10.20", 15728640, "https://docker.com/releases/20.10.20", "container", "libc"},
    {"podman", "4.3.1", 10485760, "https://podman.io/releases/4.3.1", "container", "libc"},
    {"qemu", "7.2.0", 10485760, "https://qemu.org/releases/qemu-7.2.0", "vm", "libc"},
    
    /* Media & graphics */
    {"ffmpeg", "5.1.2", 5242880, "https://ffmpeg.org/releases/5.1.2", "media", "libc"},
    {"imagemagick", "7.1.1", 2097152, "https://imagemagick.org/releases/7.1.1", "media", "libc"},
    {"graphviz", "7.0.0", 2097152, "https://graphviz.org/releases/7.0.0", "util", "libc"},
};

static int hpm_package_count = sizeof(hpm_packages) / sizeof(hpm_packages[0]);
static hpm_installed_t hpm_installed_list[HPM_MAX_INSTALLED];
static int hpm_installed_count = 0;

void hpm_init(void) {
    hpm_installed_count = 0;
}

int hpm_install(const char *pkg_name) {
    for (int i = 0; i < hpm_package_count; ++i) {
        if (strcmp(hpm_packages[i].name, pkg_name) == 0) {
            if (hpm_installed_count >= HPM_MAX_INSTALLED) return -1;
            hpm_installed_t *inst = &hpm_installed_list[hpm_installed_count++];
            int j = 0;
            while (pkg_name[j] && j < 31) { inst->name[j] = pkg_name[j]; j++; }
            inst->name[j] = '\0';
            j = 0;
            while (hpm_packages[i].version[j] && j < 15) {
                inst->version[j] = hpm_packages[i].version[j]; j++;
            }
            inst->version[j] = '\0';
            inst->size = hpm_packages[i].size;
            return 0;
        }
    }
    return -1;
}

int hpm_list(hpm_package_t *pkgs, int max) {
    int cnt = hpm_package_count < max ? hpm_package_count : max;
    for (int i = 0; i < cnt; ++i) pkgs[i] = hpm_packages[i];
    return cnt;
}

int hpm_list_by_category(const char *cat, hpm_package_t *pkgs, int max) {
    int cnt = 0;
    for (int i = 0; i < hpm_package_count && cnt < max; ++i) {
        if (strcmp(hpm_packages[i].category, cat) == 0) {
            pkgs[cnt++] = hpm_packages[i];
        }
    }
    return cnt;
}

int hpm_remove(const char *pkg_name) {
    for (int i = 0; i < hpm_installed_count; ++i) {
        if (strcmp(hpm_installed_list[i].name, pkg_name) == 0) {
            for (int j = i; j < hpm_installed_count - 1; ++j) {
                hpm_installed_list[j] = hpm_installed_list[j + 1];
            }
            hpm_installed_count--;
            return 0;
        }
    }
    return -1;
}

int hpm_search(const char *query, hpm_package_t *results, int max) {
    int cnt = 0;
    for (int i = 0; i < hpm_package_count && cnt < max; ++i) {
        if (strstr(hpm_packages[i].name, query)) {
            results[cnt++] = hpm_packages[i];
        }
    }
    return cnt;
}

int hpm_installed(hpm_installed_t *pkgs, int max) {
    int cnt = hpm_installed_count < max ? hpm_installed_count : max;
    for (int i = 0; i < cnt; ++i) pkgs[i] = hpm_installed_list[i];
    return cnt;
}

int hpm_info(const char *pkg_name, hpm_package_t *info) {
    for (int i = 0; i < hpm_package_count; ++i) {
        if (strcmp(hpm_packages[i].name, pkg_name) == 0) {
            *info = hpm_packages[i];
            return 0;
        }
    }
    return -1;
}

/* Download cache for packages */
static hpm_download_cache_t download_cache[HPM_MAX_DOWNLOADS];
static int download_cache_count = 0;

int hpm_download(const char *pkg_name, hpm_download_cache_t *cache) {
    hpm_package_t info;
    if (hpm_info(pkg_name, &info) != 0) return -1;
    
    /* Check if already cached */
    for (int i = 0; i < download_cache_count; ++i) {
        if (strcmp(download_cache[i].filename, pkg_name) == 0) {
            *cache = download_cache[i];
            return 0;
        }
    }
    
    if (download_cache_count >= HPM_MAX_DOWNLOADS) return -1;
    
    /* In production: http_get(info.url, &download_cache[download_cache_count].data);
       For now, create stub EFH package locally */
    
    hpm_download_cache_t *cur = &download_cache[download_cache_count];
    
    /* Create minimal EFH header */
    cur->data[0] = 'E';
    cur->data[1] = 'F';
    cur->data[2] = 'H';
    cur->data[3] = '\0';
    cur->data[4] = 0x01; /* version */
    
    /* Name in header (offset 8, 32 bytes) */
    int i = 0;
    while (i < 32 && pkg_name[i]) {
        cur->data[8 + i] = pkg_name[i];
        i++;
    }
    while (i < 32) cur->data[8 + i++] = 0;
    
    /* Version in header (offset 40, 16 bytes) */
    const char *ver = info.version;
    i = 0;
    while (i < 16 && ver[i]) {
        cur->data[40 + i] = ver[i];
        i++;
    }
    while (i < 16) cur->data[40 + i++] = 0;
    
    /* Minimal stub code */
    cur->data[256] = 0x90; /* nop */
    cur->data[257] = 0xC3; /* ret */
    
    cur->size = 258;
    cur->is_downloaded = 1;
    
    int n = 0;
    while (n < 64 && pkg_name[n]) {
        cur->filename[n] = pkg_name[n];
        n++;
    }
    cur->filename[n] = '\0';
    
    *cache = download_cache[download_cache_count];
    download_cache_count++;
    
    return 0;
}

int hpm_convert_to_efh(const char *pkg_name, const uint8_t *binary_data, uint32_t size) {
    /* Convert raw binary to EFH format and save to filesystem */
    hpm_package_t info;
    if (hpm_info(pkg_name, &info) != 0) return -1;
    
    /* Create .efh file in filesystem */
    char efh_path[80];
    int i = 0;
    while (i < 80 && pkg_name[i]) {
        efh_path[i] = pkg_name[i];
        i++;
    }
    efh_path[i++] = '.';
    efh_path[i++] = 'e';
    efh_path[i++] = 'f';
    efh_path[i++] = 'h';
    efh_path[i] = '\0';
    
    fs_write(efh_path, binary_data, size);
    return 0;
}

int hpm_list_downloads(hpm_download_cache_t *downloads, int max) {
    int cnt = download_cache_count < max ? download_cache_count : max;
    for (int i = 0; i < cnt; ++i) {
        downloads[i] = download_cache[i];
    }
    return cnt;
}

int hpm_extract_download(const char *filename, uint8_t *buf, int max) {
    for (int i = 0; i < download_cache_count; ++i) {
        if (strcmp(download_cache[i].filename, filename) == 0) {
            int size = download_cache[i].size < max ? download_cache[i].size : max;
            int j = 0;
            while (j < size) {
                buf[j] = download_cache[i].data[j];
                j++;
            }
            return size;
        }
    }
    return -1;
}

