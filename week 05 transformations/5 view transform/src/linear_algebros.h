#include <cmath>

#define PI 3.1415962

struct mat4 {
    float entries[16];
};

struct vec3 {
    float entries[3];
};

mat4 create_translation_transform(vec3 translation);

mat4 create_z_rotation(float theta);

mat4 create_model_transform(vec3 pos, float angle);

mat4 create_look_at(vec3 from, vec3 to);

float dot(vec3 u, vec3 v);

vec3 normalize(vec3 v);

vec3 cross(vec3 u, vec3 v);