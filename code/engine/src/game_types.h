#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include "core/kei_application.h"

typedef struct game {
    // Application configuration (window size, position, etc.)
    application_config app_config;

    // Function pointer to game's init function
    bool8 (*initialize)(struct game *game_instance);

    // Function pointer to game's update function
    bool8 (*update)(struct game *game_instance, float32 delta_time);

    // Function pointer to game's render function
    bool8 (*render)(struct game *game_instance, float32 delta_time);

    // Function pointer to handle resizes
    void (*on_resize)(struct game *game_instance, uint32 width, uint32 height);

    // Game-specific game state. Created and managed by the game
    void *state;
} game;

#endif