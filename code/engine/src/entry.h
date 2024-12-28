#ifndef ENTRY_H
#define ENTRY_H

#include "core/kei_application.h"
#include "core/kei_logger.h"
#include "core/kei_memory.h"
#include "game_types.h"

// Externally-defined function to create a game.
extern bool8 create_game(game *out_game);

// The main entry point of the application
int main(void) {
    kei_memory_initialize();

    // Request the game instance from the application
    game game_instance;
    if (!create_game(&game_instance)) {
        KEI_FATAL("Could not create game!");
        return -1;
    }

    // Ensure the function pointers exist
    if (!game_instance.initialize || !game_instance.update || !game_instance.render ||
        !game_instance.on_resize) {
        KEI_FATAL("The game's function pointers must be assigned!");
        return -2;
    }

    // Initialization
    if (!kei_application_create(&game_instance)) {
        KEI_INFO("Application failed to create!");
        return 1;
    }

    // Begin the game loop
    if (!kei_application_run()) {
        KEI_INFO("Application did not shut down gracefully.");
        return 2;
    }

    kei_memory_shutdown();

    return 0;
}

#endif