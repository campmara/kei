#ifndef KEI_EVENT_H
#define KEI_EVENT_H

#include "defines.h"

typedef struct event_data {
    // 128 byte maximum!
    union {
        int64 int64[2];
        uint64 uint64[2];
        float64 float64[2];

        int32 int32[4];
        uint32 uint32[4];
        float32 float32[4];

        int16 int16[8];
        uint16 uint16[8];

        int8 int8[16];
        uint8 uint8[16];

        char c[16];
    } data;
} event_data;

// System internal event codes. Application should use codes beyond 255.
typedef enum system_event_code {
    // Shuts the applicaton down on the next frame.
    EVENT_CODE_APPLICATION_QUIT = 0x01,

    /*
        Keyboard key pressed.
        Context usage:
        * uint16 key_code = data.data.uint16[0];
    */
    EVENT_CODE_KEY_PRESSED = 0x02,

    /*
        Keyboard key released.
        Context usage:
        * uint16 key_code = data.data.uint16[0];
    */
    EVENT_CODE_KEY_RELEASED = 0x03,

    /*
        Mouse button pressed.
        Context usage:
        * uint16 button = data.data.uint16[0];
    */
    EVENT_CODE_BUTTON_PRESSED = 0x04,

    /*
        Mouse button released.
        Context usage:
        * uint16 button = data.data.uint16[0];
    */
    EVENT_CODE_BUTTON_RELEASED = 0x05,

    /*
        Mouse moved.
        Context usage:
        * uint16 x = data.data.uint16[0];
        * uint16 y = data.data.uint16[1];
    */
    EVENT_CODE_MOUSE_MOVED = 0x06,

    /*
        Mouse wheel turned.
        Context usage:
        * uint8 z_delta = data.data.uint8[0];
    */
    EVENT_CODE_MOUSE_WHEEL = 0x07,

    // Resized/resolution changed from the OS.
    /* Context usage:
     * u16 width = data.data.u16[0];
     * u16 height = data.data.u16[1];
     */
    /*
        Resized/resolution changed from the OS.
        Context usage:
        * uint16 width = data.data.uint16[0];
        * uint16 height = data.data.uint16[1];
    */
    EVENT_CODE_RESIZED = 0x08,

    MAX_EVENT_CODE = 0xFF
} system_event_code;

// Should return true if the event is handled
typedef bool8 (*PFN_on_event)(uint16 code, void *sender, void *listener_inst, event_data data);

bool8 kei_event_initialize();
void kei_event_shutdown();

/// @brief Register to listen for when events are sent with the provided code. Events with duplicate
/// listener / callback combos will not be registered again and will cause this to return FALSE.
/// @param code The event code to listen for.
/// @param listener A pointer to a listener instance. Can be 0 / NULL.
/// @param on_event The callback function pointer to be invoked when the event code is fired.
/// @return TRUE if the event is successfully registered, otherwise FALSE.
KEI_API bool8 kei_event_register(uint16 code, void *listener, PFN_on_event on_event);

/// @brief Unregister from listening for when events are sent with the provided code. If no matching
/// registration is found, this function returns FALSE.
/// @param code The event code to stop listening for.
/// @param listener A pointer to the listener instance. Can be 0 / NULL.
/// @param on_event The callback function pointer to be unregistered.
/// @return TRUE if the event is successfully unregistered, otherwise FALSE.
KEI_API bool8 kei_event_unregister(uint16 code, void *listener, PFN_on_event on_event);

/// @brief Fires an event to listeners of the given code. If an event handler returns TRUE, the
/// event is considered handled and is not passed on to any more listeners.
/// @param code The event code to fire.
/// @param sender A pointer to the sender. Can be 0 / NULL.
/// @param data The event data.
/// @return TRUE if handled, otherwise FALSE.
KEI_API bool8 kei_event_fire(uint16 code, void *sender, event_data event);

#endif