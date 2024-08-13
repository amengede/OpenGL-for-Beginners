#include "model_factory.h"
#include "../stb_image.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <iostream>

Mesh MeshFactory::make_mesh(int i) {

    Mesh mesh;

    std::vector<glm::vec3> v;
    std::vector<glm::vec2> vt;
    std::vector<glm::vec3> vn;
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    std::unordered_map<std::string, uint32_t> history;

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
    vertices.reserve(triangleCount * 3 * 8);
    indices.reserve(triangleCount * 3 * 8);

    file.open(modelNames[i]);
    while (std::getline(file, line)) {

        words = split(line, " ");

        if (!words[0].compare("v")) {
            v.push_back(read_vec3(words, scales[i]));
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

    glGenVertexArrays(1, &mesh.VAO);
    glBindVertexArray(mesh.VAO);

    glGenBuffers(1, &mesh.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
        vertices.data(), GL_STATIC_DRAW);
    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, (void*)0);
    glEnableVertexAttribArray(0);
    //texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 32, (void*)12);
    glEnableVertexAttribArray(1);
    //normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 32, (void*)20);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &mesh.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(uint32_t),
        indices.data(), GL_STATIC_DRAW);


    mesh.elementCount = indices.size();
    mesh.material = load_2d_material(imageNames[i]);

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
    std::vector<glm::vec3>& vn, std::vector<float>& vertices,
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
    std::vector<glm::vec3>& vn, std::vector<float>& vertices, 
    std::unordered_map<std::string, uint32_t>& history,
    std::vector<uint32_t>& indices) {

    if (!history.contains(description)) {
        history[description] = static_cast<uint32_t>(vertices.size() / 8);

        std::vector<std::string> v_vt_vn = split(description, "/");

        //position
        glm::vec3 pos = v[std::stol(v_vt_vn[0]) - 1];
        vertices.push_back(pos[0]);
        vertices.push_back(pos[1]);
        vertices.push_back(pos[2]);

        glm::vec2 texcoord = glm::vec2(0.5f, 0.5f);
        if (v_vt_vn[1].compare("") != 0) {

            //tex coord
            texcoord = vt[std::stol(v_vt_vn[1]) - 1];
            
        }
        vertices.push_back(texcoord[0]);
        vertices.push_back(texcoord[1]);

        //normal
        if (v_vt_vn.size() == 3) {
            glm::vec3 normal = vn[std::stol(v_vt_vn[2]) - 1];
            vertices.push_back(normal[0]);
            vertices.push_back(normal[1]);
            vertices.push_back(normal[2]);
        }
        else {
            vertices.push_back(1.0f);
            vertices.push_back(1.0f);
            vertices.push_back(1.0f);
        }
    }

    indices.push_back(history[description]);
}

unsigned int MeshFactory::load_2d_material(const char* filename) {

    unsigned int material;

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);

    glGenTextures(1, &material);
    glBindTexture(GL_TEXTURE_2D, material);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1024, 1024);

    glTexSubImage2D(GL_TEXTURE_2D,
        0, 0, 0, 1024, 1024, GL_RGBA, GL_UNSIGNED_BYTE, data);

    //free data
    stbi_image_free(data);

    //Configure sampler
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return material;
}