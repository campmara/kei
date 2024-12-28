#ifndef KEI_STRING_H
#define KEI_STRING_H

#include "defines.h"

// Returns the length of the given string.
KEI_API uint64 kei_string_length(const char *str);
KEI_API char *kei_string_duplicate(const char *str);

#endif