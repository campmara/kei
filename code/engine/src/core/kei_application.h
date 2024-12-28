#ifndef KEI_APPLICATION_H
#define KEI_APPLICATION_H

#include "defines.h"

struct game;

typedef struct application_config {
    int16 start_pos_x;  // Window starting position x-axis
    int16 start_pos_y;  // Window starting position y-axis
    int16 start_width;  // Window starting width
    int16 start_height; // Window starting height
    char *name;         // Application name used in windowing
} application_config;

KEI_API bool8 kei_application_create(struct game *game_instance);
KEI_API bool8 kei_application_run();

#endif