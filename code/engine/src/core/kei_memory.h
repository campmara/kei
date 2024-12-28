#ifndef KEI_MEMORY_H
#define KEI_MEMORY_H

#include "defines.h"

typedef enum memory_tag {
    // For temporary use. Should be assigned one of the below or have a new tag created.
    MEMORY_TAG_UNKNOWN,
    MEMORY_TAG_ARRAY,
    MEMORY_TAG_KEI_LIST,
    MEMORY_TAG_DICT,
    MEMORY_TAG_RING_QUEUE,
    MEMORY_TAG_BST,
    MEMORY_TAG_STRING,
    MEMORY_TAG_APPLICATION,
    MEMORY_TAG_JOB,
    MEMORY_TAG_TEXTURE,
    MEMORY_TAG_MATERIAL_INSTANCE,
    MEMORY_TAG_RENDERER,
    MEMORY_TAG_GAME,
    MEMORY_TAG_TRANSFORM,
    MEMORY_TAG_ENTITY,
    MEMORY_TAG_ENTITY_NODE,
    MEMORY_TAG_SCENE,

    MEMORY_TAG_MAX_TAGS
} memory_tag;

KEI_API void kei_memory_initialize();
KEI_API void kei_memory_shutdown();

KEI_API void *kei_memory_alloc(uint64 size, memory_tag tag);
KEI_API void kei_memory_free(void *block, uint64 size, memory_tag tag);
KEI_API void *kei_memory_zero(void *block, uint64 size);
KEI_API void *kei_memory_copy(void *dest, const void *source, uint64 size);
KEI_API void *kei_memory_set(void *dest, int32 value, uint64 size);

KEI_API char *kei_memory_get_usage_str();

#endif