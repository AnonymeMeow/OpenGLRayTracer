#version 330 core

layout (location = 0) in vec2 coord;

out Ray
{
    vec3 origin;
    vec3 direction;
} ray;

#include ../camera.glsl

void main()
{
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
    ray.origin = camera.position - vec3(0., 0., camera.d) * rot_camera;
    ray.direction = vec3(coord.x * camera.hfov, coord.y * camera.hfov, 1.) * rot_camera;
    gl_Position = vec4(-coord.x, coord.y, 0., 1.);
}