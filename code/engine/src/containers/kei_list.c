#include "containers/kei_list.h"

#include "core/kei_memory.h"
#include "core/kei_logger.h"

void *_kei_list_create(uint64 length, uint64 stride) {
    uint64 header_size = KEI_LIST_FIELD_LENGTH * sizeof(uint64);
    uint64 list_size = length * stride;
    uint64 *new_list = kei_memory_alloc(header_size + list_size, MEMORY_TAG_KEI_LIST);
    kei_memory_set(new_list, 0, header_size + list_size);
    new_list[KEI_LIST_CAPACITY] = length;
    new_list[KEI_LIST_LENGTH] = 0;
    new_list[KEI_LIST_STRIDE] = stride;
    return (void *)(new_list + KEI_LIST_FIELD_LENGTH);
}

void _kei_list_destroy(void *list) {
    uint64 *header = (uint64 *)list - KEI_LIST_FIELD_LENGTH;
    uint64 header_size = KEI_LIST_FIELD_LENGTH * sizeof(uint64);
    uint64 total_size = header_size + header[KEI_LIST_CAPACITY] * header[KEI_LIST_STRIDE];
    kei_memory_free(header, total_size, MEMORY_TAG_KEI_LIST);
}

uint64 _kei_list_field_get(void *list, uint64 field) {
    uint64 *header = (uint64 *)list - KEI_LIST_FIELD_LENGTH;
    return header[field];
}

void _kei_list_field_set(void *list, uint64 field, uint64 value) {
    uint64 *header = (uint64 *)list - KEI_LIST_FIELD_LENGTH;
    header[field] = value;
}

void *_kei_list_resize(void *list) {
    uint64 length = kei_list_get_length(list);
    uint64 stride = kei_list_get_stride(list);
    void *temp = _kei_list_create((KEI_LIST_RESIZE_FACTOR * kei_list_get_capacity(list)), stride);
    kei_memory_copy(temp, list, length * stride);

    _kei_list_field_set(temp, KEI_LIST_LENGTH, length);
    _kei_list_destroy(list);
    return temp;
}

void *_kei_list_push(void *list, const void *value_ptr) {
    // Check for resize
    uint64 length = kei_list_get_length(list);
    uint64 stride = kei_list_get_stride(list);
    if (length >= kei_list_get_capacity(list)) {
        list = _kei_list_resize(list);
    }

    uint64 address = (uint64)list;
    address += (length * stride);
    kei_memory_copy((void *)address, value_ptr, stride);
    _kei_list_field_set(list, KEI_LIST_LENGTH, length + 1);
    return list;
}

void _kei_list_pop(void *list, void *dest) {
    uint64 length = kei_list_get_length(list);
    uint64 stride = kei_list_get_stride(list);
    uint64 address = (uint64)list;
    address += ((length - 1) * stride);
    kei_memory_copy(dest, (void *)address, stride);
    _kei_list_field_set(list, KEI_LIST_LENGTH, length - 1);
}

void *_kei_list_pop_at(void *list, uint64 index, void *dest) {
    uint64 length = kei_list_get_length(list);
    uint64 stride = kei_list_get_stride(list);
    if (index >= length) {
        KEI_ERROR("Index outside the bounds of this array! Length: %i, index: %index",
                  length,
                  index);
        return list;
    }

    uint64 address = (uint64)list;
    kei_memory_copy(dest, (void *)(address + (index * stride)), stride);

    // If not on the last element, snip out the entry and copy the rest inward.
    if (index != length - 1) {
        kei_memory_copy((void *)(address + (index * stride)),
                        (void *)(address + ((index + 1) * stride)),
                        stride * (length - index));
    }

    _kei_list_field_set(list, KEI_LIST_LENGTH, length - 1);
    return list;
}

void *_kei_list_insert_at(void *list, uint64 index, void *value_ptr) {
    uint64 length = kei_list_get_length(list);
    uint64 stride = kei_list_get_stride(list);
    if (index >= length) {
        KEI_ERROR("Index outside the bounds of this array! Length: %i, index: %index",
                  length,
                  index);
        return list;
    }

    if (length >= kei_list_get_capacity(list)) {
        list = _kei_list_resize(list);
    }

    uint64 address = (uint64)list;

    // If not on the last element, copy the rest outward.
    if (index != length - 1) {
        kei_memory_copy((void *)(address + ((index + 1) * stride)),
                        (void *)(address + (index * stride)),
                        stride * (length - index));
    }

    // Set the value at the index.
    kei_memory_copy((void *)(address + (index * stride)), value_ptr, stride);

    _kei_list_field_set(list, KEI_LIST_LENGTH, length + 1);
    return list;
}
