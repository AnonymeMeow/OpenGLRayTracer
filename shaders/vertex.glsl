#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;

uniform int frame;

void main()
{
    float angle = float(frame) / 1000.f;
    mat3 rot = mat3(
        cos(angle), 0, sin(angle),
        0, 1, 0,
        -sin(angle), 0, cos(angle)
    );
    mat3 rot1 = mat3(
        1, 0, 0,
        0, cos(0.6), -sin(0.6),
        0, sin(0.6), cos(0.6)
    );
    vec3 newPos = rot1 * rot * aPos;
    gl_Position = vec4(newPos, 3.f + newPos.z);
    TexCoord = texCoord;
}