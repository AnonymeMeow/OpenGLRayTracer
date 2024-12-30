#include "camera.hpp"

#include "../opengl/shader.hpp"

void Camera::move_relative_yaw(float step_x, float step_y, float step_z, float modifier)
{
    x += (step_x * std::cos(yaw) + step_z * std::sin(yaw)) * modifier;
    y += step_y * modifier;
    z += (step_z * std::cos(yaw) - step_x * std::sin(yaw)) * modifier;
}

void Camera::move_relative(float step_x, float step_y, float step_z, float modifier)
{
    return move_relative_yaw(
        step_x,
        step_y * std::cos(pitch) + step_z * std::sin(pitch),
        step_z * std::cos(pitch) - step_y * std::sin(pitch),
        modifier
    );
}

void Camera::turn_head(float d_yaw, float d_pitch, float modifier)
{
    yaw += d_yaw * std::sqrt(hfov) * modifier;
    pitch += d_pitch * std::sqrt(hfov) * modifier;
    pitch = std::clamp(pitch, - std::numbers::pi_v<float> / 2, std::numbers::pi_v<float> / 2);
}

void Camera::zoom(float value, float modifier)
{
    hfov = std::clamp(hfov * (1 + value * modifier), hfov_min, hfov_max);
}

void Camera::reset_fov()
{
    hfov = hfov_default;
}

template <>
void Program::set(const GLchar* name, const Camera& camera)
{
    std::string name_str = name;
    set((name_str + ".position").c_str(), camera.x, camera.y, camera.z);
    set((name_str + ".orientation").c_str(), camera.yaw, camera.pitch);
    set((name_str + ".hfov").c_str(), camera.hfov);
    set((name_str + ".d").c_str(), camera.d);
}