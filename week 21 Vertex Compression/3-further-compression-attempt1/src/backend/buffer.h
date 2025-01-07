#pragma once
#include "../config.h"

struct Partition {
    uint32_t size, offset, target, binding_index;
};

class Buffer {
public:

    uint32_t size = 0, deviceMemory = 0;
    std::vector<Partition> partitions;

    ~Buffer();

    uint32_t add_partition(uint32_t size, uint32_t target, uint32_t binding_index = 0);

    void build();

    void blit(uint32_t partition_index, void* src);

    void bind(uint32_t partition_index);

    void bind_render(uint32_t partition_index);

    uint32_t get_offset(uint32_t partition_index);
};