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

mat3 diag(vec3 v)
{
    return mat3(
        v.x, 0, 0,
        0, v.y, 0,
        0, 0, v.z
    );
}

void main()
{
    float angle = float(frame) / 1000.f;
    mat3 rot1 = mat3(
        cos(angle), 0, -sin(angle),
        0, 1, 0,
        sin(angle), 0, cos(angle)
    );
    mat3 rot2 = mat3(
        1, 0, 0,
        0, cos(0.4), -sin(0.4),
        0, sin(0.4), cos(0.4)
    );
    vec4 q = vec4(rotation, sqrt(1 - dot(rotation, rotation)));
    mat3 rot0 = 2 * mat3(
        1 - q.y * q.y - q.z * q.z, q.x * q.y + q.z * q.w, q.x * q.z - q.y * q.w,
        q.x * q.y - q.z * q.w, 1 - q.x * q.x - q.z * q.z, q.y * q.z + q.x * q.w,
        q.x * q.z + q.y * q.w, q.y * q.z - q.x * q.w, 1 - q.x * q.x - q.y * q.y
    ) - diag(vec3(1));
    mat3 transform = diag(vec3(-1, 1, 1));
    origin_vs   = rot2 * rot1 * transform * origin + vec3(0, -20.f, 40.f);
    edges_vs    = rot2 * rot1 * transform * rot0 * diag(size);
    uv_vs.east  = uv_east;
    uv_vs.south = uv_south;
    uv_vs.west  = uv_west;
    uv_vs.north = uv_north;
    uv_vs.up    = uv_up;
    uv_vs.down  = uv_down;
}