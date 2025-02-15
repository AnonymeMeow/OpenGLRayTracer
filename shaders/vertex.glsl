#version 330 core

layout (location = 0) in vec3 origin;
layout (location = 1) in vec3 size;
layout (location = 2) in vec4 rotation;
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

#include camera.glsl

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
    mat3 rot_cube = 2 * mat3(
        1 - rotation.y * rotation.y - rotation.z * rotation.z, rotation.x * rotation.y + rotation.z * rotation.w, rotation.x * rotation.z - rotation.y * rotation.w,
        rotation.x * rotation.y - rotation.z * rotation.w, 1 - rotation.x * rotation.x - rotation.z * rotation.z, rotation.y * rotation.z + rotation.x * rotation.w,
        rotation.x * rotation.z + rotation.y * rotation.w, rotation.y * rotation.z - rotation.x * rotation.w, 1 - rotation.x * rotation.x - rotation.y * rotation.y
    ) - diag(vec3(1));
    mat3 rot_camera_x = mat3(
        cos(camera.orientation.x), 0, sin(camera.orientation.x),
        0, 1, 0,
        -sin(camera.orientation.x), 0, cos(camera.orientation.x)
    );
    mat3 rot_camera_y = mat3(
        1, 0, 0,
        0, cos(camera.orientation.y), sin(camera.orientation.y),
        0, -sin(camera.orientation.y), cos(camera.orientation.y)
    );
    mat3 rot_camera = rot_camera_y * rot_camera_x;
    mat3 transform = diag(vec3(-1, 1, 1));
    origin_vs   = transform * rot_camera * (origin - camera.position);
    edges_vs    = transform * rot_camera * rot_cube * diag(size);
    uv_vs.east  = uv_east;
    uv_vs.south = uv_south;
    uv_vs.west  = uv_west;
    uv_vs.north = uv_north;
    uv_vs.up    = uv_up;
    uv_vs.down  = uv_down;
}