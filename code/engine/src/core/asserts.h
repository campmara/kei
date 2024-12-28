#ifndef ASSERTS_H
#define ASSERTS_H

#include "defines.h"

// Disable assertions by commenting the line below
#define KEI_ASSERTIONS_ENABLED

#ifdef KEI_ASSERTIONS_ENABLED
#if _MSC_VER
#include <intrin.h>
#define debug_break() __debugbreak()
#else
#define debug_break() __builtin_trap()
#endif

KEI_API void
report_assertion_failure(const char *expression, const char *message, const char *file, int32 line);

#define KEI_ASSERT(expr)                                                                        \
    {                                                                                              \
        if (expr) {                                                                                \
        } else {                                                                                   \
            report_assertion_failure(#expr, "", __FILE__, __LINE__);                               \
            debug_break();                                                                         \
        }                                                                                          \
    }

#define KEI_ASSERT_MSG(expr, message)                                                           \
    {                                                                                              \
        if (expr) {                                                                                \
        } else {                                                                                   \
            report_assertion_failure(#expr, message, __FILE__, __LINE__);                          \
            debug_break();                                                                         \
        }                                                                                          \
    }

#ifdef _DEBUG
#define KEI_ASSERT_DEBUG(expr)                                                                  \
    {                                                                                              \
        if (expr) {                                                                                \
        } else {                                                                                   \
            report_assertion_failure(#expr, "", __FILE__, __LINE__);                               \
            debug_break();                                                                         \
        }                                                                                          \
    }
#else
#define KEI_ASSERT_DEBUG(expr) // Does nothing
#endif

#else
#define KEI_ASSERT(expr)              // Does nothing
#define KEI_ASSERT_MSG(expr, message) // Does nothing
#define KEI_ASSERT_DEBUG(expr)        // Does nothing
#endif

#endif