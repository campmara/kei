#include <string.h> // TODO: custom string library
#include <stdio.h>

#include "kei_memory.h"

#include "core/kei_logger.h"
#include "core/kei_string.h"
#include "platform/kei_platform.h"

struct memory_stats {
    uint64 total_allocated;
    uint64 tagged_allocations[MEMORY_TAG_MAX_TAGS];
};

static const char *memory_tag_strings[MEMORY_TAG_MAX_TAGS] = {"UNKNOWN    ",
                                                              "ARRAY      ",
                                                              "DARRAY     ",
                                                              "DICT       ",
                                                              "RING_QUEUE ",
                                                              "BST        ",
                                                              "STRING     ",
                                                              "APPLICATION",
                                                              "JOB        ",
                                                              "TEXTURE    ",
                                                              "MAT_INST   ",
                                                              "RENDERER   ",
                                                              "GAME       ",
                                                              "TRANSFORM  ",
                                                              "ENTITY     ",
                                                              "ENTITY_NODE",
                                                              "SCENE      "};

static struct memory_stats stats;

void kei_memory_initialize() {
    kei_platform_memory_zero(&stats, sizeof(stats));
    KEI_INFO("Memory subsystem initialized.");
}

void kei_memory_shutdown() {}

void *kei_memory_alloc(uint64 size, memory_tag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        KEI_WARN("kei_memory_alloc called using MEMORY_TAG_UNKNOWN. Re-class this allocation.");
    }

    stats.total_allocated += size;
    stats.tagged_allocations[tag] += size;

    // TODO: Memory alignment
    void *block = kei_platform_memory_alloc(size, FALSE);
    kei_platform_memory_zero(block, size);
    return block;
}

void kei_memory_free(void *block, uint64 size, memory_tag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        KEI_WARN("kei_memory_free called using MEMORY_TAG_UNKNOWN. Re-class this allocation.");
    }

    stats.total_allocated -= size;
    stats.tagged_allocations[tag] -= size;

    // TODO: Memory alignment
    kei_platform_memory_free(block, FALSE);
}

void *kei_memory_zero(void *block, uint64 size) {
    return kei_platform_memory_zero(block, size);
}

void *kei_memory_copy(void *dest, const void *source, uint64 size) {
    return kei_platform_memory_copy(dest, source, size);
}

void *kei_memory_set(void *dest, int32 value, uint64 size) {
    return kei_platform_memory_set(dest, value, size);
}

char *kei_memory_get_usage_str() {
    const uint64 gb = 1024 * 1024 * 1024;
    const uint64 mb = 1024 * 1024;
    const uint64 kb = 1024;

    char buffer[8000] = "System memory use (tagged):\n";
    uint64 offset = strlen(buffer);
    for (uint32 i = 0; i < MEMORY_TAG_MAX_TAGS; ++i) {
        char unit[4] = "XiB";
        float amount = 1.0f;
        if (stats.tagged_allocations[i] >= gb) {
            unit[0] = 'G';
            amount = stats.tagged_allocations[i] / (float)gb;
        } else if (stats.tagged_allocations[i] >= mb) {
            unit[0] = 'M';
            amount = stats.tagged_allocations[i] / (float)mb;
        } else if (stats.tagged_allocations[i] >= kb) {
            unit[0] = 'K';
            amount = stats.tagged_allocations[i] / (float)kb;
        } else {
            unit[0] = 'B';
            unit[1] = 0;
            amount = (float)stats.tagged_allocations[i];
        }

        int32 length =
            snprintf(buffer + offset, 8000, "  %s: %.2f%s\n", memory_tag_strings[i], amount, unit);
        offset += length;
    }

    char *out_string = kei_string_duplicate(buffer);
    return out_string;
}