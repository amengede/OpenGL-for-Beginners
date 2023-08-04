#version 330 core

layout (location=0) in vec3 vertexPos;
layout (location=1) in vec2 vertexTexCoord;

out vec2 fragmentTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(vertexPos, 1.0);
    fragmentTexCoord = vertexTexCoord;
}