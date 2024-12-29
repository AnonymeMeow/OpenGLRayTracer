#pragma once

struct Camera
{
    float x, y, z;
    float pitch, yaw;
    float hfov, d;
    const float keyboard_sensitivity;
    const float mouse_rotation_sensitivity;
    const float mouse_move_sensitivity;
    const float mouse_zoom_sensitivity;
    const float ctrl_sensitivity_modifier;
    inline static const float hfov_max = 2.f;
    inline static const float hfov_min = 0.01f;
    inline static const float hfov_default = 1.f;
    void move_relative_yaw(float, float, float, float);
    void move_relative(float, float, float, float);
    void turn_head(float, float, float);
    void zoom(float, float);
    void reset_fov();
};