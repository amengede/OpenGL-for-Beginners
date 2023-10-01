#version 330 core

in vec2 fragmentTexCoord;
in vec3 fragmentNormal;

out vec4 screenColor;

uniform sampler2D material;

const vec3 sunDirection = normalize(vec3(-1.0, 1.0, 1.0));

void main()
{
    float lightStrength = max(0.2, dot(fragmentNormal, sunDirection));
    screenColor = vec4(lightStrength * texture(material, fragmentTexCoord).rgb, 1.0);
}