#include "core/kei_string.h"
#include "core/kei_memory.h"

#include <string.h>

uint64 kei_string_length(const char *str) {
    return strlen(str);
}

char *kei_string_duplicate(const char *str) {
    uint64 length = kei_string_length(str);
    char *copy = kei_memory_alloc(length + 1, MEMORY_TAG_STRING);
    kei_memory_copy(copy, str, length + 1);
    return copy;
}