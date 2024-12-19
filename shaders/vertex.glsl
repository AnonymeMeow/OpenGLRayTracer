#version 330 core

layout (location = 0) in vec3 origin;
layout (location = 1) in vec3 size;
layout (location = 2) in vec3 rotation;
layout (location = 3) in vec4 uv_east;
layout (location = 4) in vec4 uv_south;
layout (location = 5) in vec4 uv_west;
layout (location = 6) in vec4 uv_north;
layout (location = 7) in vec4 uv_up;
layout (location = 8) in vec4 uv_down;

out vec3 origin_vs;
out mat3 edges_vs;
out UV
{
    vec4 east;
    vec4 south;
    vec4 west;
    vec4 north;
    vec4 up;
    vec4 down;
} uv_vs;

uniform int frame;

void main()
{
    float angle = float(frame) / 1000.f;
    mat3 rot1 = mat3(
        cos(angle), 0, sin(angle),
        0, 1, 0,
        -sin(angle), 0, cos(angle)
    );
    mat3 rot2 = mat3(
        1, 0, 0,
        0, cos(0.6), -sin(0.6),
        0, sin(0.6), cos(0.6)
    );
    mat3 rot0 = mat3(
        1 - rotation.y * rotation.y - rotation.z * rotation.z,  rotation.z, -rotation.y,
        -rotation.z, 1 - rotation.x * rotation.x - rotation.z * rotation.z,  rotation.x,
         rotation.y, -rotation.x, 1 - rotation.x * rotation.x - rotation.y * rotation.y
    );
    mat3 transform = mat3(
        -1, 0, 0,
        0, 1, 0,
        0, 0, 1
    );
    origin_vs   = rot2 * rot1 * transform * origin;
    edges_vs    = rot2 * rot1 * transform * rot0 * mat3(size.x, 0, 0, 0, size.y, 0, 0, 0, size.z);
    uv_vs.east  = uv_east;
    uv_vs.south = uv_south;
    uv_vs.west  = uv_west;
    uv_vs.north = uv_north;
    uv_vs.up    = uv_up;
    uv_vs.down  = uv_down;
}