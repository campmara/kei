#ifndef GAME_H
#define GAME_H

#include <defines.h>
#include <game_types.h>

typedef struct game_state {
    float32 delta_time;
} game_state;

bool8 game_initialize(game *game_instance);
bool8 game_update(game *game_instance, float32 delta_time);
bool8 game_render(game *game_instance, float32 delta_time);
void game_on_resize(game *game_instance, uint32 width, uint32 height);

#endif