#include "noe.h"

void *MemorySet(void *dst, int value, size_t length)
{
    for(size_t i = 0; i < (length); ++i) 
        CAST(int8_t *, dst)[i] = CAST(int8_t, value);
    return dst;
}

void *MemoryCopy(void *dst, const void *src, size_t length)
{
    for(size_t i = 0; i < (length); ++i) 
        CAST(uint8_t *, dst)[i] = CAST(const uint8_t *, src)[i];
    return dst;
}

char *StringCopy(char *dst, const char *src, size_t length)
{
    for(size_t i = 0; i < length; ++i)
        dst[i] = src[i];
    return dst;
}

size_t StringLength(const char *str)
{
    size_t i = 0;
    while(str[i++] != '\0');
    return i;
}

const char *StringFind(const char *haystack, const char *needle)
{
    if (!*needle) return (char *)haystack;

    while (*haystack) {
        const char *p1 = haystack;
        const char *p2 = needle;
        while (*p1 && *p2 && *p1 == *p2) {
            p1++;
            p2++;
        }
        if (!*p2) {
            return haystack;
        }
        haystack++;
    }

    return NULL; 
}

