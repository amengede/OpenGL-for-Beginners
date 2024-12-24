#pragma once
#include "../config.h"

class MeshFactory {

public:

    void consume(int i);
    Mesh finalize();
    unsigned int load_2d_material(const char* filename);
    std::unordered_map<uint32_t, uint32_t> firstIndices;
    std::unordered_map<uint32_t, uint32_t> indexCounts;
    std::unordered_map<uint32_t, AABB> AABBs;

private:

    glm::vec3 read_vec3(std::vector<std::string> words, float preScale);

    glm::vec2 read_vec2(std::vector<std::string> words);

    void read_face(std::vector<std::string> words,
        std::vector<glm::vec3>& v, std::vector<glm::vec2>& vt,
        std::vector<glm::vec3>& vn, std::vector<float>& vertices,
        std::unordered_map<std::string, uint32_t>& history,
        std::vector<uint32_t>& indices);

    void read_corner(std::string description,
        std::vector<glm::vec3>& v, std::vector<glm::vec2>& vt,
        std::vector<glm::vec3>& vn, std::vector<float>& vertices,
        std::unordered_map<std::string, uint32_t>& history,
        std::vector<uint32_t>& indices);

    std::vector<float> vertexLump;
    std::vector<uint32_t> indexLump;

};