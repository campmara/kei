#include "core/kei_event.h"
#include "core/kei_memory.h"
#include "containers/kei_list.h"
#include "core/kei_logger.h"

// This should be more than enough codes...
#define MAX_MESSAGE_CODES 16384

typedef struct registered_event {
    void *listener;
    PFN_on_event callback;
} registered_event;

typedef struct event_code_entry {
    registered_event *events;
} event_code_entry;

typedef struct event_system_state {
    // Lookup table for event codes.
    event_code_entry registered[MAX_MESSAGE_CODES];
} event_system_state;

// Event system internal state.
static bool8 is_initialized = FALSE;
static event_system_state state;

bool8 kei_event_initialize() {
    if (is_initialized == TRUE) {
        return FALSE;
    }

    is_initialized = FALSE;
    kei_memory_zero(&state, sizeof(state));
    is_initialized = TRUE;

    KEI_INFO("Event subsystem initialized.");

    return TRUE;
}

void kei_event_shutdown() {
    // Free the events lists. Any objects pointed to should be destroyed on their own.
    for (uint16 i = 0; i < MAX_MESSAGE_CODES; ++i) {
        if (state.registered[i].events != 0) {
            kei_list_destroy(state.registered[i].events);
            state.registered[i].events = 0;
        }
    }
}

bool8 kei_event_register(uint16 code, void *listener, PFN_on_event on_event) {
    if (is_initialized == FALSE) {
        return FALSE;
    }

    if (state.registered[code].events == 0) {
        state.registered[code].events = kei_list_create(registered_event);
    }

    uint64 registered_count = kei_list_get_length(state.registered[code].events);
    for (uint64 i = 0; i < registered_count; ++i) {
        if (state.registered[code].events[i].listener == listener) {
            // TODO: warn
            return FALSE;
        }
    }

    // If at this point no duplicate was found, proceed with registration.
    registered_event event;
    event.listener = listener;
    event.callback = on_event;
    kei_list_push(state.registered[code].events, event);

    return TRUE;
}

bool8 kei_event_unregister(uint16 code, void *listener, PFN_on_event on_event) {
    if (is_initialized == FALSE) {
        return FALSE;
    }

    if (state.registered[code].events == 0) {
        // TODO: warn
        return FALSE;
    }

    uint64 registered_count = kei_list_get_length(state.registered[code].events);
    for (uint64 i = 0; i < registered_count; ++i) {
        registered_event e = state.registered[code].events[i];
        if (e.listener == listener && e.callback == on_event) {
            // Found one, remove it.
            registered_event popped_event;
            kei_list_pop_at(state.registered[code].events, i, &popped_event);
            return TRUE;
        }
    }

    // Not found.
    return FALSE;
}

bool8 kei_event_fire(uint16 code, void *sender, event_data event) {
    if (is_initialized == FALSE) {
        return FALSE;
    }

    // If nothing is registered for the code, boot out.
    if (state.registered[code].events == 0) {
        return FALSE;
    }

    uint64 registered_count = kei_list_get_length(state.registered[code].events);
    for (uint64 i = 0; i < registered_count; ++i) {
        registered_event e = state.registered[code].events[i];
        if (e.callback(code, sender, e.listener, event)) {
            // Message has been handled, do not send to other listeners.
            return TRUE;
        }
    }

    // Not found.
    return FALSE;
}