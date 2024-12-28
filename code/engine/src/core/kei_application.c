#include "kei_application.h"
#include "game_types.h"
#include "kei_logger.h"
#include "platform/kei_platform.h"
#include "core/kei_memory.h"
#include "core/kei_event.h"
#include "core/kei_input.h"

typedef struct application_state {
    game *game_instance;
    bool8 is_running;
    bool8 is_suspended;
    platform_state p_state;
    int16 width;
    int16 height;
    float64 last_time;
} application_state;

static bool8 is_initialized = FALSE;
static application_state app_state;

// Event handlers
bool8 kei_application_on_event(uint16 code,
                               void *sender,
                               void *listener_instance,
                               event_data event);
bool8 kei_application_on_key(uint16 code, void *sender, void *listener_instance, event_data event);

bool8 kei_application_create(game *game_instance) {
    if (is_initialized) {
        KEI_ERROR("kei_application_create called more than once.");
        return FALSE;
    }

    app_state.game_instance = game_instance;

    // Initialize subsystems
    kei_logger_initialize();
    kei_input_initialize();

    // TODO: Remove this
    KEI_FATAL("A test message: %f", 3.14f);
    KEI_ERROR("A test message: %f", 3.14f);
    KEI_WARN("A test message: %f", 3.14f);
    KEI_INFO("A test message: %f", 3.14f);
    KEI_DEBUG("A test message: %f", 3.14f);
    KEI_TRACE("A test message: %f", 3.14f);

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

    if (!kei_event_initialize()) {
        KEI_ERROR("Event system failed initialization. Application cannot continue.");
        return FALSE;
    }

    kei_event_register(EVENT_CODE_APPLICATION_QUIT, 0, kei_application_on_event);
    kei_event_register(EVENT_CODE_KEY_PRESSED, 0, kei_application_on_key);
    kei_event_register(EVENT_CODE_KEY_RELEASED, 0, kei_application_on_key);

    // Perform platform startup
    if (!kei_platform_initialize(&app_state.p_state,
                                 game_instance->app_config.name,
                                 game_instance->app_config.start_pos_x,
                                 game_instance->app_config.start_pos_y,
                                 game_instance->app_config.start_width,
                                 game_instance->app_config.start_height)) {
        return FALSE;
    }

    // Initialize the game
    if (!app_state.game_instance->initialize(app_state.game_instance)) {
        KEI_FATAL("Game failed to initialize.");
        return FALSE;
    }

    app_state.game_instance->on_resize(app_state.game_instance, app_state.width, app_state.height);

    is_initialized = TRUE;

    return TRUE;
}

bool8 kei_application_run() {
    KEI_INFO(kei_memory_get_usage_str());

    while (app_state.is_running) {
        if (!kei_platform_pump_messages(&app_state.p_state)) {
            app_state.is_running = FALSE;
        }

        if (!app_state.is_suspended) {
            // Call game's update routine
            if (!app_state.game_instance->update(app_state.game_instance, (float32)0)) {
                KEI_FATAL("Game update failed, shutting down.");
                app_state.is_running = FALSE;
                break;
            }

            // Call game's render routine
            if (!app_state.game_instance->render(app_state.game_instance, (float32)0)) {
                KEI_FATAL("Game render failed, shutting down.");
                app_state.is_running = FALSE;
                break;
            }

            // NOTE: Input update / state copying should always be handled after any input should be
            // recorded, i.e. before this line. As a safety, input is the last thing to be updated
            // before this frame ends.
            kei_input_update(0);
        }
    }

    app_state.is_running = FALSE;

    // Shutdown subsystems.

    // Unregister from events before susbsytem shutdown.
    kei_event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, kei_application_on_event);
    kei_event_unregister(EVENT_CODE_KEY_PRESSED, 0, kei_application_on_key);
    kei_event_unregister(EVENT_CODE_KEY_RELEASED, 0, kei_application_on_key);
    kei_event_shutdown();

    kei_input_shutdown();
    kei_platform_shutdown(&app_state.p_state);

    return TRUE;
}

bool8 kei_application_on_event(uint16 code,
                               void *sender,
                               void *listener_instance,
                               event_data event) {
    switch (code) {
        case EVENT_CODE_APPLICATION_QUIT: {
            KEI_INFO("EVENT_CODE_APPLICATION_QUIT received, shutting down.\n");
            app_state.is_running = FALSE;
            return TRUE;
        }
    }

    return FALSE;
}

bool8 kei_application_on_key(uint16 code, void *sender, void *listener_instance, event_data event) {
    if (code == EVENT_CODE_KEY_PRESSED) {
        uint16 key_code = event.data.uint16[0];
        if (key_code == KEY_ESCAPE) {
            // NOTE: Technically firing an event to itself, but there may be other listeners.
            event_data quit_event = {};
            kei_event_fire(EVENT_CODE_APPLICATION_QUIT, 0, quit_event);
            return TRUE; // Block anything else from processing this.
        } else if (key_code == KEY_A) {
            KEI_DEBUG("Explicit - A key pressed!"); // Example of checking for a key.
        } else {
            KEI_DEBUG("'%c' key pressed in window.", key_code);
        }
    } else if (code == EVENT_CODE_KEY_RELEASED) {
        uint16 key_code = event.data.uint16[0];
        if (key_code == KEY_B) {
            KEI_DEBUG("Explicit - B key released!"); // Example of checking for a key.
        } else {
            KEI_DEBUG("'%c' key released in window.", key_code);
        }
    }

    return FALSE;
}