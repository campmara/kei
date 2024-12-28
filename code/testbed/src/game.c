#include <core/kei_logger.h>

#include "game.h"

bool8 game_initialize(game *game_instance) {
    KEI_DEBUG("game_intialize() called!");
    return TRUE;
}

bool8 game_update(game *game_instance, float32 delta_time) {
    return TRUE;
}

bool8 game_render(game *game_instance, float32 delta_time) {
    return TRUE;
}

void game_on_resize(game *game_instance, uint32 width, uint32 height) {}