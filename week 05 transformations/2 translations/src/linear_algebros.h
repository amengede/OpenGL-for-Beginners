struct mat4 {
    float entries[16];
};

struct vec3 {
    float entries[3];
};

mat4 create_translation_transform(vec3 translation);