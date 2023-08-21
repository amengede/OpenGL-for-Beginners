#include "mesh_factory.h"

void MeshFactory::start_obj_mesh() {
    v.clear();
    vt.clear();
    vn.clear();
    vertices.clear();
    v_loaded = 0; 
    vt_loaded = 0; 
    vn_loaded = 0;
}

void MeshFactory::append_obj_mesh(
    const char* filename, glm::mat4 preTransform,
    float layer) {

    current_layer = layer;

    this->preTransform = preTransform;

    reserve_space(filename);

    read_file(filename);

    v_loaded = v.size(); 
    vt_loaded = vt.size(); 
    vn_loaded = vn.size();
}

void MeshFactory::append_cube_mesh(glm::vec3 size, float layer) {

    float l = size.x;
    float w = size.y;
    float h = size.z;

    vertices = {
         l,  w, -h, 1.0f, 1.0f, layer,  0.0f,  0.0f, -1.0f,
         l, -w, -h, 1.0f, 0.0f, layer,  0.0f,  0.0f, -1.0f,
        -l, -w, -h, 0.0f, 0.0f, layer,  0.0f,  0.0f, -1.0f,
        -l, -w, -h, 0.0f, 0.0f, layer,  0.0f,  0.0f, -1.0f,
        -l,  w, -h, 0.0f, 1.0f, layer,  0.0f,  0.0f, -1.0f,
         l,  w, -h, 1.0f, 1.0f, layer, 0.0f,  0.0f, -1.0f,

        -l, -w,  h, 0.0f, 0.0f, layer,  0.0f,  0.0f,  1.0f,
         l, -w,  h, 1.0f, 0.0f, layer,  0.0f,  0.0f,  1.0f,
         l,  w,  h, 1.0f, 1.0f, layer,  0.0f,  0.0f,  1.0f,
         l,  w,  h, 1.0f, 1.0f, layer,  0.0f,  0.0f,  1.0f,
        -l,  w,  h, 0.0f, 1.0f, layer,  0.0f,  0.0f,  1.0f,
        -l, -w,  h, 0.0f, 0.0f, layer,  0.0f,  0.0f,  1.0f,

        -l,  w,  h, 1.0f, 1.0f, layer, -1.0f,  0.0f,  0.0f,
        -l,  w, -h, 1.0f, 0.0f, layer, -1.0f,  0.0f,  0.0f,
        -l, -w, -h, 0.0f, 0.0f, layer, -1.0f,  0.0f,  0.0f,
        -l, -w, -h, 0.0f, 0.0f, layer, -1.0f,  0.0f,  0.0f,
        -l, -w,  h, 0.0f, 1.0f, layer, -1.0f,  0.0f,  0.0f,
        -l,  w,  h, 1.0f, 1.0f, layer, -1.0f,  0.0f,  0.0f,

         l, -w, -h, 0.0f, 0.0f, layer,  1.0f,  0.0f,  0.0f,
         l,  w, -h, 1.0f, 0.0f, layer,  1.0f,  0.0f,  0.0f,
         l,  w,  h, 1.0f, 1.0f, layer,  1.0f,  0.0f,  0.0f,
         l,  w,  h, 1.0f, 1.0f, layer,  1.0f,  0.0f,  0.0f,
         l, -w,  h, 0.0f, 1.0f, layer,  1.0f,  0.0f,  0.0f,
         l, -w, -h, 0.0f, 0.0f, layer,  1.0f,  0.0f,  0.0f,

        -l, -w, -h, 0.0f, 0.0f, layer,  0.0f, -1.0f,  0.0f,
         l, -w, -h, 1.0f, 0.0f, layer,  0.0f, -1.0f,  0.0f,
         l, -w,  h, 1.0f, 1.0f, layer,  0.0f, -1.0f,  0.0f,
         l, -w,  h, 1.0f, 1.0f, layer,  0.0f, -1.0f,  0.0f,
        -l, -w,  h, 0.0f, 1.0f, layer,  0.0f, -1.0f,  0.0f,
        -l, -w, -h, 0.0f, 0.0f, layer,  0.0f, -1.0f,  0.0f,

         l,  w,  h, 1.0f, 1.0f, layer,  0.0f,  1.0f,  0.0f,
         l,  w, -h, 1.0f, 0.0f, layer,  0.0f,  1.0f,  0.0f,
        -l,  w, -h, 0.0f, 0.0f, layer,  0.0f,  1.0f,  0.0f,
        -l,  w, -h, 0.0f, 0.0f, layer,  0.0f,  1.0f,  0.0f,
        -l,  w,  h, 0.0f, 1.0f, layer,  0.0f,  1.0f,  0.0f,
         l,  w,  h, 1.0f, 1.0f, layer,  0.0f,  1.0f,  0.0f
    };
}

StaticMesh MeshFactory::build_obj_mesh() {

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 
        vertices.data(), GL_STATIC_DRAW);
    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 36, (void*)0);
    glEnableVertexAttribArray(0);
    //texture coordinates
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 36, (void*)12);
    glEnableVertexAttribArray(1);
    //normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 36, (void*)24);
    glEnableVertexAttribArray(2);

    StaticMesh mesh;
    mesh.VAO = VAO;
    mesh.vertexCount = vertices.size() / 9;
    mesh.VBO = VBO;
    v.clear();
    vt.clear();
    vn.clear();
    vertices.clear();
    return mesh;
}

void MeshFactory::reserve_space(const char* filename) {

    size_t vertexCount = v.size();
    size_t texcoordCount = vt.size();
    size_t normalCount = vn.size();
    size_t dataCount = vertices.size();

    std::string line;
    std::vector<std::string> words;

    std::ifstream file;

    file.open(filename);
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
            dataCount += 3 * 9 * (words.size() - 3);
        }
    }
    file.close();

    v.reserve(vertexCount);
    vt.reserve(texcoordCount);
    vn.reserve(normalCount);
    vertices.reserve(dataCount);
}

void MeshFactory::read_file(const char* filename) {

    std::string line;
    std::vector<std::string> words;

    std::ifstream file;

    file.open(filename);
    while (std::getline(file, line)) {

        words = split(line, " ");

        if (!words[0].compare("v")) {
            v.push_back(read_vec3(words, 1.0f));
        }

        else if (!words[0].compare("vt")) {
            vt.push_back(read_vec2(words));
        }

        else if (!words[0].compare("vn")) {
            vn.push_back(glm::normalize(read_vec3(words, 0.0f)));
        }

        else if (!words[0].compare("f")) {
            read_face(words);
        }
    }
    file.close();
}

glm::vec3 MeshFactory::read_vec3(std::vector<std::string> words, float w) {
    return glm::vec3(
        preTransform 
        * glm::vec4(std::stof(words[1]), std::stof(words[2]), std::stof(words[3]), w)
    );
}

glm::vec2 MeshFactory::read_vec2(std::vector<std::string> words) {
    
    return glm::vec2(std::stof(words[1]), std::stof(words[2]));
}

void MeshFactory::read_face(std::vector<std::string> words) {
    
    size_t triangleCount = words.size() - 3;

    for (size_t i = 0; i < triangleCount; ++i) {
        read_corner(words[1]);
        read_corner(words[2 + i]);
        read_corner(words[3 + i]);
    }

}

void MeshFactory::read_corner(std::string description) {
    
    std::vector<std::string> v_vt_vn = split(description, "/");

    //position
    glm::vec3 pos = v[v_loaded + std::stol(v_vt_vn[0]) - 1];
    vertices.push_back(pos[0]);
    vertices.push_back(pos[1]);
    vertices.push_back(pos[2]);

    //tex coord
    glm::vec2 texcoord = vt[vt_loaded + std::stol(v_vt_vn[1]) - 1];
    vertices.push_back(texcoord[0]);
    vertices.push_back(texcoord[1]);
    vertices.push_back(current_layer);

    //normal
    glm::vec3 normal = vn[vn_loaded + std::stol(v_vt_vn[2]) - 1];
    vertices.push_back(normal[0]);
    vertices.push_back(normal[1]);
    vertices.push_back(normal[2]);

}
