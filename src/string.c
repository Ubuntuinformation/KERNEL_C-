#include <stdint.h>

/* String utilities for freestanding environment */

int strcmp(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return *(unsigned char*)a - *(unsigned char*)b;
}

int strncmp(const char *a, const char *b, int n) {
    for (int i = 0; i < n; ++i) {
        if (a[i] != b[i] || a[i] == '\0' || b[i] == '\0') return a[i] - b[i];
    }
    return 0;
}

int strlen(const char *s) {
    int len = 0;
    while (s[len]) len++;
    return len;
}

const char *strstr(const char *haystack, const char *needle) {
    while (*haystack) {
        const char *h = haystack, *n = needle;
        while (*n && *h == *n) { h++; n++; }
        if (!*n) return haystack;
        haystack++;
    }
    return 0;
}

void itoa(int val, char *buf, int base) {
    if (val == 0) { buf[0] = '0'; buf[1] = 0; return; }
    int i = 0, neg = val < 0;
    if (neg) val = -val;
    char digits[] = "0123456789ABCDEF";
    while (val > 0) { buf[i++] = digits[val % base]; val /= base; }
    if (neg) buf[i++] = '-';
    buf[i] = 0;
    /* Reverse */
    for (int j = 0; j < i/2; ++j) {
        char tmp = buf[j]; buf[j] = buf[i-1-j]; buf[i-1-j] = tmp;
    }
}

void *memcpy(void *dest, const void *src, int n) {
    unsigned char *d = (unsigned char*)dest;
    const unsigned char *s = (const unsigned char*)src;
    for (int i = 0; i < n; ++i) d[i] = s[i];
    return dest;
}
