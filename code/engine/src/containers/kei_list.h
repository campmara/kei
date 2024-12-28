#ifndef KEI_LIST_H
#define KEI_LIST_H

#include "defines.h"

/*
kei_list is a dynamically-allocated array implementation with useful pushing/popping functionality

Memory layout:
    uint64 capacity = number of elements that can be held
    uint64 length = number of elements currently contained
    uint64 stride = size of each element in bytes
    void *elements
*/

enum {
    KEI_LIST_CAPACITY,
    KEI_LIST_LENGTH,
    KEI_LIST_STRIDE,
    KEI_LIST_FIELD_LENGTH
};

#define KEI_LIST_DEFAULT_CAPACITY 1
#define KEI_LIST_RESIZE_FACTOR 2

#define kei_list_create(type) _kei_list_create(KEI_LIST_DEFAULT_CAPACITY, sizeof(type))
#define kei_list_create_with_capacity(type, capacity) _kei_list_create(capacity, sizeof(type))
#define kei_list_destroy(list) _kei_list_destroy(list)

#define kei_list_push(list, value)                                                                 \
    {                                                                                              \
        typeof(value) temp = value;                                                                \
        list = _kei_list_push(list, &temp);                                                        \
    }

#define kei_list_pop(list, dest) _kei_list_pop(list, dest)

#define kei_list_pop_at(list, index, dest) _kei_list_pop_at(list, index, dest)
#define kei_list_insert_at(list, index, value)                                                     \
    {                                                                                              \
        typeof(value) temp = value;                                                                \
        list = _kei_list_insert_at(list, index, &temp);                                            \
    }

#define kei_list_clear(list) _kei_list_field_set(list, KEI_LIST_LENGTH, 0)
#define kei_list_set_length(list, value) _kei_list_field_set(list, KEI_LIST_LENGTH, value)

#define kei_list_get_capacity(list) _kei_list_field_get(list, KEI_LIST_CAPACITY)
#define kei_list_get_length(list) _kei_list_field_get(list, KEI_LIST_LENGTH)
#define kei_list_get_stride(list) _kei_list_field_get(list, KEI_LIST_STRIDE)

KEI_API void *_kei_list_create(uint64 length, uint64 stride);
KEI_API void _kei_list_destroy(void *list);

KEI_API uint64 _kei_list_field_get(void *list, uint64 field);
KEI_API void _kei_list_field_set(void *list, uint64 field, uint64 value);

KEI_API void *_kei_list_resize(void *list);

KEI_API void *_kei_list_push(void *list, const void *value_ptr);
KEI_API void _kei_list_pop(void *list, void *dest);

KEI_API void *_kei_list_pop_at(void *list, uint64 index, void *dest);
KEI_API void *_kei_list_insert_at(void *list, uint64 index, void *value_ptr);

#endif