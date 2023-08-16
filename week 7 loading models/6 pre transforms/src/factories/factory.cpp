#include "factory.h"
#include "../stb_image.h"

Factory::Factory(
    std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents,
    std::unordered_map<unsigned int, RenderComponent>& renderComponents,
    std::unordered_map<unsigned int, TransformComponent>& transformComponents):
physicsComponents(physicsComponents),
renderComponents(renderComponents),
transformComponents(transformComponents) {
}

Factory::~Factory() {
    glDeleteBuffers(VBOs.size(), VBOs.data());
    glDeleteVertexArrays(VAOs.size(), VAOs.data());
    glDeleteTextures(textures.size(), textures.data());
}

unsigned int Factory::make_camera(glm::vec3 position, glm::vec3 eulers) {

    TransformComponent transform;
    transform.position = position;
    transform.eulers = eulers;

    transformComponents[entities_made] = transform;

    return entities_made++;
}

void Factory::make_cube(glm::vec3 position, glm::vec3 eulers, 
    glm::vec3 eulerVelocity) {

	TransformComponent transform;
	transform.position = position;
	transform.eulers = eulers;
	transformComponents[entities_made] = transform;

	PhysicsComponent physics;
	physics.velocity = {0.0f, 0.0f, 0.0f};
	physics.eulerVelocity = eulerVelocity;
	physicsComponents[entities_made] = physics;
	
	RenderComponent render = make_cube_mesh({0.25f, 0.25f, 0.25f});
	render.material = make_texture("../img/paper.jpg");
	renderComponents[entities_made++] = render;
}

void Factory::make_girl(glm::vec3 position, glm::vec3 eulers) {

	TransformComponent transform;
	transform.position = position;
	transform.eulers = eulers;
	transformComponents[entities_made] = transform;
	
    glm::mat4 preTransform = glm::mat4(1.0f);
	preTransform = glm::rotate(preTransform, 
        glm::radians(90.0f), { 1.0f, 0.0f, 0.0f });
    preTransform = glm::rotate(preTransform, 
        glm::radians(90.0f), { 0.0f, 1.0f, 0.0f });
	RenderComponent render = make_obj_mesh(
        "../models/girl.obj", preTransform);
	render.material = make_texture("../img/stargirl.png");
	renderComponents[entities_made++] = render;
}

RenderComponent Factory::make_cube_mesh(glm::vec3 size) {

    float l = size.x;
    float w = size.y;
    float h = size.z;

    std::vector<float> vertices = {
         l,  w, -h, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
         l, -w, -h, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
        -l, -w, -h, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
        -l, -w, -h, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
        -l,  w, -h, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
         l,  w, -h, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,

        -l, -w,  h, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
         l, -w,  h, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
         l,  w,  h, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
         l,  w,  h, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
        -l,  w,  h, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
        -l, -w,  h, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

        -l,  w,  h, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -l,  w, -h, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -l, -w, -h, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -l, -w, -h, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -l, -w,  h, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -l,  w,  h, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,

         l, -w, -h, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
         l,  w, -h, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
         l,  w,  h, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         l,  w,  h, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         l, -w,  h, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         l, -w, -h, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,

        -l, -w, -h, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
         l, -w, -h, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
         l, -w,  h, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
         l, -w,  h, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
        -l, -w,  h, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
        -l, -w, -h, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,

         l,  w,  h, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
         l,  w, -h, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
        -l,  w, -h, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
        -l,  w, -h, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
        -l,  w,  h, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
         l,  w,  h, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    VAOs.push_back(VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    VBOs.push_back(VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
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

    RenderComponent record;
    record.VAO = VAO;
    record.vertexCount = 36;
    return record;
}

RenderComponent Factory::make_obj_mesh(
    const char* filepath, glm::mat4 preTransform) {

    std::vector<glm::vec3> v;
    std::vector<glm::vec2> vt;
    std::vector<glm::vec3> vn;
    std::vector<float> vertices;

    size_t vertexCount = 0;
    size_t texcoordCount = 0;
    size_t normalCount = 0;
    size_t triangleCount = 0;

    std::string line;
    std::vector<std::string> words;

    std::ifstream file;

    file.open(filepath);
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

    file.open(filepath);
    while (std::getline(file, line)) {

        words = split(line, " ");

        if (!words[0].compare("v")) {
            v.push_back(read_vec3(words, preTransform, 1.0f));
        }

        else if (!words[0].compare("vt")) {
            vt.push_back(read_vec2(words));
        }

        else if (!words[0].compare("vn")) {
            vn.push_back(read_vec3(words, preTransform, 0.0f));
        }

        else if (!words[0].compare("f")) {
            read_face(words, v, vt, vn, vertices);
        }
    }
    file.close();

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    VAOs.push_back(VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    VBOs.push_back(VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
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

    RenderComponent render;
    render.VAO = VAO;
    render.vertexCount = vertices.size() / 8;
    return render;
}

glm::vec3 Factory::read_vec3(std::vector<std::string> words,
    glm::mat4 preTransform, float w) {
    return glm::vec3(
        preTransform 
        * glm::vec4(std::stof(words[1]), std::stof(words[2]), std::stof(words[3]), w)
    );
}

glm::vec2 Factory::read_vec2(std::vector<std::string> words) {
    return glm::vec2(std::stof(words[1]), std::stof(words[2]));
}

void Factory::read_face(std::vector<std::string> words, 
    std::vector<glm::vec3>& v, std::vector<glm::vec2>& vt, 
    std::vector<glm::vec3>& vn, std::vector<float>& vertices) {
    
    size_t triangleCount = words.size() - 3;

    for (size_t i = 0; i < triangleCount; ++i) {
        read_corner(words[1], v, vt, vn, vertices);
        read_corner(words[2 + i], v, vt, vn, vertices);
        read_corner(words[3 + i], v, vt, vn, vertices);
    }

}

void Factory::read_corner(std::string description, 
    std::vector<glm::vec3>& v, std::vector<glm::vec2>& vt, 
    std::vector<glm::vec3>& vn, std::vector<float>& vertices) {
    
    std::vector<std::string> v_vt_vn = split(description, "/");

    //position
    glm::vec3 pos = v[std::stol(v_vt_vn[0]) - 1];
    vertices.push_back(pos[0]);
    vertices.push_back(pos[1]);
    vertices.push_back(pos[2]);

    //tex coord
    glm::vec2 texcoord = vt[std::stol(v_vt_vn[1]) - 1];
    vertices.push_back(texcoord[0]);
    vertices.push_back(texcoord[1]);

    //normal
    glm::vec3 normal = vn[std::stol(v_vt_vn[2]) - 1];
    vertices.push_back(normal[0]);
    vertices.push_back(normal[1]);
    vertices.push_back(normal[2]);

}

unsigned int Factory::make_texture(const char* filename) {

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(
        filename, &width, &height, &channels, STBI_rgb_alpha);

	//make the texture
    unsigned int texture;
	glGenTextures(1, &texture);
    textures.push_back(texture);
    glBindTexture(GL_TEXTURE_2D, texture);
	
    //load data
    glTexImage2D(GL_TEXTURE_2D, 
        0, GL_RGBA, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, data);

    //free data
	stbi_image_free(data);

    //Configure sampler
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    return texture;
}