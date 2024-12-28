#include "core/kei_input.h"
#include "core/kei_event.h"
#include "core/kei_memory.h"
#include "core/kei_logger.h"

typedef struct keyboard_state {
    bool8 keys[256];
} keyboard_state;

typedef struct mouse_state {
    int16 x;
    int16 y;
    uint8 buttons[BUTTON_MAX_BUTTONS];
} mouse_state;

typedef struct input_state {
    keyboard_state keyboard_state_current;
    keyboard_state keyboard_state_previous;
    mouse_state mouse_state_current;
    mouse_state mouse_state_previous;
} input_state;

// Internal input state
static bool8 is_initialized = FALSE;
static input_state state = {};

bool8 kei_input_is_key_down(keys key) {
    if (!is_initialized) {
        return FALSE;
    }
    return state.keyboard_state_current.keys[key] == TRUE;
}

bool8 kei_input_is_key_up(keys key) {
    if (!is_initialized) {
        return TRUE;
    }
    return state.keyboard_state_current.keys[key] == FALSE;
}

bool8 kei_input_was_key_down(keys key) {
    if (!is_initialized) {
        return FALSE;
    }
    return state.keyboard_state_previous.keys[key] == TRUE;
}

bool8 kei_input_was_key_up(keys key) {
    if (!is_initialized) {
        return TRUE;
    }
    return state.keyboard_state_previous.keys[key] == FALSE;
}

bool8 kei_input_is_button_down(buttons button) {
    if (!is_initialized) {
        return FALSE;
    }
    return state.mouse_state_current.buttons[button] == TRUE;
}

bool8 kei_input_is_button_up(buttons button) {
    if (!is_initialized) {
        return TRUE;
    }
    return state.mouse_state_current.buttons[button] == FALSE;
}

bool8 kei_input_was_button_down(buttons button) {
    if (!is_initialized) {
        return FALSE;
    }
    return state.mouse_state_previous.buttons[button] == TRUE;
}

bool8 kei_input_was_button_up(buttons button) {
    if (!is_initialized) {
        return TRUE;
    }
    return state.mouse_state_previous.buttons[button] == FALSE;
}

void kei_input_get_mouse_position(int32 *x, int32 *y) {
    if (!is_initialized) {
        *x = 0;
        *y = 0;
        return;
    }
    *x = state.mouse_state_current.x;
    *y = state.mouse_state_current.y;
}

void kei_input_get_mouse_position_previous(int32 *x, int32 *y) {
    if (!is_initialized) {
        *x = 0;
        *y = 0;
        return;
    }
    *x = state.mouse_state_previous.x;
    *y = state.mouse_state_previous.y;
}

void kei_input_initialize() {
    kei_memory_zero(&state, sizeof(input_state));
    is_initialized = TRUE;
    KEI_INFO("Input subsystem initialized.");
}

void kei_input_shutdown() {
    // TODO: Add shutdown routines when needed.
    is_initialized = FALSE;
}

void kei_input_update(float64 delta_time) {
    if (!is_initialized) {
        return;
    }

    // Copy current states to previous states.
    kei_memory_copy(&state.keyboard_state_previous,
                    &state.keyboard_state_current,
                    sizeof(keyboard_state));
    kei_memory_copy(&state.mouse_state_previous, &state.mouse_state_current, sizeof(mouse_state));
}

void kei_input_process_key(keys key, bool8 is_pressed) {
    // Only handle this if the state actually changed.
    if (state.keyboard_state_current.keys[key] != is_pressed) {
        state.keyboard_state_current.keys[key] = is_pressed;

        // Fire off an event for immediate processing.
        event_data event;
        event.data.uint16[0] = key;
        kei_event_fire(is_pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED, 0, event);
    }
}

void kei_input_process_button(buttons button, bool8 is_pressed) {
    // Only handle this if the state actually changed.
    if (state.mouse_state_current.buttons[button] != is_pressed) {
        state.mouse_state_current.buttons[button] = is_pressed;

        // Fire off an event for immediate processing.
        event_data event;
        event.data.uint16[0] = button;
        kei_event_fire(is_pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED,
                       0,
                       event);
    }
}

void kei_input_process_mouse_move(int16 x, int16 y) {
    // Only handle this if the state actually changed.
    if (state.mouse_state_current.x != x || state.mouse_state_current.y != y) {
        // NOTE: Enable this if debugging.
        KEI_DEBUG("Mouse position: %i, %i!", x, y);

        // Update internal state.
        state.mouse_state_current.x = x;
        state.mouse_state_current.y = y;

        // Fire off an event for immediate processing.
        event_data event;
        event.data.uint16[0] = x;
        event.data.uint16[1] = y;
        kei_event_fire(EVENT_CODE_MOUSE_MOVED, 0, event);
    }
}

void kei_input_process_mouse_wheel(int8 z_delta) {
    // NOTE: No internal state to update.

    // Fire off an event for immediate processing.
    event_data event;
    event.data.uint8[0] = z_delta;
    kei_event_fire(EVENT_CODE_MOUSE_WHEEL, 0, event);
}
