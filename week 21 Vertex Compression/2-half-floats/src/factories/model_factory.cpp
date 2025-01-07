#include "model_factory.h"
#include "../stb_image.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <iostream>

//https://stackoverflow.com/questions/1659440/32-bit-to-16-bit-floating-point-conversion

uint32_t as_uint(const float x) {
    return *(uint32_t*)&x;
}

float as_float(const uint32_t x) {
    return *(float*)&x;
}

float half_to_float(const uint16_t x) {
    // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
    const uint32_t e = (x & 0x7C00) >> 10; // exponent
    const uint32_t m = (x & 0x03FF) << 13; // mantissa
    const uint32_t v = as_uint((float)m) >> 23; // evil log2 bit hack to count leading zeros in denormalized format
    return as_float((x & 0x8000) << 16 | (e != 0) * ((e + 112) << 23 | m) | ((e == 0) & (m != 0)) * ((v - 37) << 23 | ((m << (150 - v)) & 0x007FE000))); // sign : normalized : denormalized
}

uint16_t float_to_half(const float x) { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
    const uint32_t b = as_uint(x) + 0x00001000; // round-to-nearest-even: add last bit after truncated mantissa
    const uint32_t e = (b & 0x7F800000) >> 23; // exponent
    const uint32_t m = b & 0x007FFFFF; // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
    return (b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) & 0x7C00) | m >> 13) | ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) | (e > 143) * 0x7FFF; // sign : normalized : denormalized : saturate
}

unsigned char float_to_ubyte(const float x) {
    return 255 * x;
}

char float_to_byte(const float x) {
    return 127 * x;
}

void MeshFactory::consume(int i) {

    std::vector<glm::vec3> v;
    std::vector<glm::vec2> vt;
    std::vector<glm::vec3> vn;
    std::vector<unsigned char> vertices;
    std::vector<uint32_t> indices;
    std::unordered_map<std::string, uint32_t> history;

    glm::vec3 center = glm::vec3(0.0f);
    float radius = 0.0f;

    size_t vertexCount = 0;
    size_t texcoordCount = 0;
    size_t normalCount = 0;
    size_t triangleCount = 0;

    std::string line;
    std::vector<std::string> words;

    std::ifstream file;

    file.open(modelNames[i]);
    while (std::getline(file, line)) {

        words = split(line, " ");

        if (!words[0].compare("v")) {
            ++vertexCount;
        }

        else if (!words[0].compare("vt")) {
            ++texcoordCount;
        }

        else if (!words[0].compare("vn")) {
            ++normalCount;
        }

        else if (!words[0].compare("f")) {
            triangleCount += words.size() - 3;
        }
    }
    file.close();

    v.reserve(vertexCount);
    vt.reserve(texcoordCount);
    vn.reserve(normalCount);
    //three corners per triangle, 8 floats per corner
    vertices.reserve(triangleCount * 3 * 11);
    indices.reserve(triangleCount * 3);
    AABBs[i] = AABB();

    float coefficient = 1.0f / vertexCount;

    file.open(modelNames[i]);
    while (std::getline(file, line)) {

        words = split(line, " ");

        if (!words[0].compare("v")) {
            glm::vec3 pos = read_vec3(words, scales[i]);
            AABBs[i].min = glm::min(AABBs[i].min, glm::vec4(pos, 1.0f));
            AABBs[i].max = glm::max(AABBs[i].max, glm::vec4(pos, 1.0f));
            center += coefficient * pos;
            v.push_back(pos);
        }

        else if (!words[0].compare("vt")) {
            vt.push_back(read_vec2(words));
        }

        else if (!words[0].compare("vn")) {
            vn.push_back(read_vec3(words, 1.0f));
        }

        else if (!words[0].compare("f")) {
            read_face(words, v, vt, vn, vertices, history, indices);
        }
    }
    file.close();

    for (size_t j = 0; j < vertexCount; ++j) {
        radius = std::max(radius, glm::length(v[j] - center));
    }

    boundingSpheres[i] = { center, radius };

    indexCounts[i] = static_cast<uint32_t>(indices.size());
    firstIndices[i] = static_cast<uint32_t>(indexLump.size());

    vertexCount = static_cast<uint32_t>(vertexLump.size() / 11);

    for (unsigned char attribute : vertices) {
        vertexLump.push_back(attribute);
    }

    for (uint32_t index : indices) {
        indexLump.push_back(index + vertexCount);
    }
}

Mesh MeshFactory::finalize() {

    Mesh mesh;

    glGenVertexArrays(1, &mesh.VAO);
    glBindVertexArray(mesh.VAO);

    glGenBuffers(1, &mesh.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexLump.size(),
        vertexLump.data(), GL_STATIC_DRAW);
    //position
    glVertexAttribPointer(0, 3, GL_HALF_FLOAT, GL_FALSE, 11, (void*)0);
    glEnableVertexAttribArray(0);
    //texture coordinates
    glVertexAttribPointer(1, 2, GL_UNSIGNED_BYTE, GL_TRUE, 11, (void*)6);
    glEnableVertexAttribArray(1);
    //normal
    glVertexAttribPointer(2, 3, GL_BYTE, GL_TRUE, 11, (void*)8);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &mesh.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indexLump.size() * sizeof(uint32_t),
        indexLump.data(), GL_STATIC_DRAW);

    return mesh;
}

glm::vec3 MeshFactory::read_vec3(std::vector<std::string> words, float preScale) {
    return preScale * glm::vec3(std::stof(words[1]), std::stof(words[2]), std::stof(words[3]));
}

glm::vec2 MeshFactory::read_vec2(std::vector<std::string> words) {
    return glm::vec2(std::stof(words[1]), std::stof(words[2]));
}

void MeshFactory::read_face(std::vector<std::string> words,
    std::vector<glm::vec3>& v, std::vector<glm::vec2>& vt,
    std::vector<glm::vec3>& vn, std::vector<unsigned char>& vertices,
    std::unordered_map<std::string, uint32_t>& history,
    std::vector<uint32_t>& indices) {

    size_t triangleCount = words.size() - 3;

    for (size_t i = 0; i < triangleCount; ++i) {
        read_corner(words[1], v, vt, vn, vertices, history, indices);
        read_corner(words[2 + i], v, vt, vn, vertices, history, indices);
        read_corner(words[3 + i], v, vt, vn, vertices, history, indices);
    }

}

void MeshFactory::read_corner(std::string description,
    std::vector<glm::vec3>& v, std::vector<glm::vec2>& vt,
    std::vector<glm::vec3>& vn, std::vector<unsigned char>& vertices,
    std::unordered_map<std::string, uint32_t>& history,
    std::vector<uint32_t>& indices) {

    if (!history.contains(description)) {
        history[description] = static_cast<uint32_t>(vertices.size() / 11);

        std::vector<std::string> v_vt_vn = split(description, "/");

        //position
        glm::vec3 pos = v[std::stol(v_vt_vn[0]) - 1];
        uint16_t x = float_to_half(pos[0]);
        unsigned char lower = x & 0xff;
        unsigned char upper = (x >> 8) & 0xff;
        vertices.push_back(lower);
        vertices.push_back(upper);
        x = float_to_half(pos[1]);
        lower = x & 0xff;
        upper = (x >> 8) & 0xff;
        vertices.push_back(lower);
        vertices.push_back(upper);
        x = float_to_half(pos[2]);
        lower = x & 0xff;
        upper = (x >> 8) & 0xff;
        vertices.push_back(lower);
        vertices.push_back(upper);

        glm::vec2 texcoord = glm::vec2(0.5f, 0.5f);
        if (v_vt_vn[1].compare("") != 0) {

            //tex coord
            texcoord = vt[std::stol(v_vt_vn[1]) - 1];

        }
        vertices.push_back(float_to_ubyte(texcoord[0]));
        vertices.push_back(float_to_ubyte(texcoord[1]));

        //normal
        if (v_vt_vn.size() == 3) {
            glm::vec3 normal = vn[std::stol(v_vt_vn[2]) - 1];
            vertices.push_back(float_to_byte(normal[0]));
            vertices.push_back(float_to_byte(normal[1]));
            vertices.push_back(float_to_byte(normal[2]));
        }
        else {
            vertices.push_back(255);
            vertices.push_back(255);
            vertices.push_back(255);
        }
    }

    indices.push_back(history[description]);
}

unsigned int MeshFactory::load_2d_material(const char* filename) {

    unsigned int material;

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename, &width, &height, &channels, STBI_rgb);

    glGenTextures(1, &material);
    glBindTexture(GL_TEXTURE_2D, material);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, width, height);

    glTexSubImage2D(GL_TEXTURE_2D,
        0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

    //free data
    stbi_image_free(data);

    //Configure sampler
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    return material;
}