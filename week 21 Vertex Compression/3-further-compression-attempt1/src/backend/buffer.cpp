#include "buffer.h"
#include <glad/glad.h>
#include <iostream>

Buffer::~Buffer() {
    glDeleteBuffers(1, &deviceMemory);
}
    
uint32_t Buffer::add_partition(uint32_t size, uint32_t target, uint32_t binding_index) {
    uint32_t offset = this->size;
    //uint32_t padding = (256 - (offset & 255)) & 255;
    uint32_t padding = 0;
    this->size += size + padding;

    Partition partition;
    partition.binding_index = binding_index;
    partition.offset = offset + padding;
    partition.target = target;
    partition.size = size;

    partitions.push_back(partition);

    return partitions.size() - 1;
}
    
void Buffer::build() {

    glGenBuffers(1, &deviceMemory);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, deviceMemory);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_DYNAMIC_STORAGE_BIT);
}

void Buffer::blit(uint32_t partition_index, void* src) {
    Partition partition = partitions[partition_index];
    glBindBuffer(partition.target, deviceMemory);
    glBufferSubData(partition.target, partition.offset, partition.size, src);
}

void Buffer::bind(uint32_t partition_index) {
    Partition partition = partitions[partition_index];
    glBindBufferRange(partition.target, partition.binding_index, deviceMemory, partition.offset, partition.size);
    //std::cout << partition.target << ", " << partition.binding_index << ", " << deviceMemory << ", " << partition.offset << ", " << partition.size << std::endl;
    //std::cout << glGetError() << std::endl;
}

void Buffer::bind_render(uint32_t partition_index) {
    Partition partition = partitions[partition_index];
    glBindBuffer(partition.target, deviceMemory);
}

uint32_t Buffer::get_offset(uint32_t partition_index) {
    return partitions[partition_index].offset;
}