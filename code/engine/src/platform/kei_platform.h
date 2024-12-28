#ifndef PLATFORM_H
#define PLATFORM_H

#include "defines.h"

typedef struct platform_state {
    void *internal_state;
} platform_state;

// Setup / cleanup
bool8 kei_platform_initialize(platform_state *p_state,
                              const char *application_name,
                              int32 x,
                              int32 y,
                              int32 width,
                              int32 height);
void kei_platform_shutdown(platform_state *p_state);

// Runs every frame and polls the platform messages
bool8 kei_platform_pump_messages(platform_state *p_state);

// Memory-related
void *kei_platform_memory_alloc(uint64 size, bool8 is_aligned);
void kei_platform_memory_free(void *block, bool8 is_aligned);
void *kei_platform_memory_zero(void *block, uint64 size);
void *kei_platform_memory_copy(void *dest, const void *source, uint64 size);
void *kei_platform_memory_set(void *dest, int32 value, uint64 size);

// Console logging
void kei_platform_console_write(const char *message, uint8 color);
void kei_platform_console_write_error(const char *message, uint8 color);

// Time
float64 kei_platform_get_absolute_time();

/// @brief Sleep on main thread. Should only be used for giving time back to the OS for unused
/// update power. Does NOT get exported.
/// @param ms Time in milliseconds to sleep.
void kei_platform_sleep(uint64 ms);

#endif