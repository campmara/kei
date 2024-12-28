#ifndef KEI_LOGGER_H
#define KEI_LOGGER_H

#include "defines.h"

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

#if KEI_RELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_DEBUG_ENABLED 0
#endif

typedef enum log_level {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5
} log_level;

bool8 kei_logger_initialize();
void kei_logger_shutdown();

KEI_API void kei_log(log_level level, const char *message, ...);

// Logs a fatal message
#define KEI_FATAL(message, ...) kei_log(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);

#ifndef KEI_ERROR
// Logs an error message.
#define KEI_ERROR(message, ...) kei_log(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);
#endif

#if LOG_WARN_ENABLED == 1
// Logs a warning message
#define KEI_WARN(message, ...) kei_log(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
#define KEI_WARN(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
// Logs an info message
#define KEI_INFO(message, ...) kei_log(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
#define KEI_INFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
// Logs a debug message
#define KEI_DEBUG(message, ...) kei_log(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
#define KEI_DEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
// Logs a trace message
#define KEI_TRACE(message, ...) kei_log(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
#define KEI_TRACE(message, ...)
#endif

#endif