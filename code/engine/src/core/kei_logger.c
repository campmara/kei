#include "kei_logger.h"
#include "asserts.h"
#include "platform/kei_platform.h"

// TODO: temporary
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

bool8 kei_logger_initialize() {
    // TODO: create log file

    KEI_INFO("Log subsystem initialized.");

    return TRUE;
}

void kei_logger_shutdown() {
    // TODO: cleanup logging / write queued entries
}

void kei_log(log_level level, const char *message, ...) {
    const char *level_strings[6] = {
        "[FATAL]: ",
        "[ERROR]: ",
        "[WARN]: ",
        "[INFO]: ",
        "[DEBUG]: ",
        "[TRACE]: ",
    };
    bool8 is_error = level < LOG_LEVEL_WARN;

    // Technically imposes a 32k character limit on a single log entry, but DON'T DO THAT LOL
    const int32 msg_length = 32000;
    char out_message[msg_length];
    memset(out_message, 0, sizeof(out_message));

    // Format original message
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(out_message, msg_length, message, arg_ptr);
    va_end(arg_ptr);

    char out_message2[msg_length];
    sprintf(out_message2, "%s%s\n", level_strings[level], out_message);

    // Platform-specific output.
    if (is_error) {
        kei_platform_console_write_error(out_message2, level);
    } else {
        kei_platform_console_write(out_message2, level);
    }
}

void report_assertion_failure(const char *expression,
                              const char *message,
                              const char *file,
                              int32 line) {
    kei_log(LOG_LEVEL_FATAL,
            "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n",
            expression,
            message,
            file,
            line);
}