#pragma once

#include "model.hpp"

class Scene
{
public:
    struct Object
    {
        double position[3];
        double rotation[3];
        double zoom;
        Model model;
        fs::path texture;
        Object(const Json::Value&, const Json::Value&, const Json::Value&, const fs::path&, const fs::path&);
    };

    int window_size[2];
    std::string window_name;
    struct
    {
        double position[3];
        double orientation[2];
        double fov, d;
        double keyboard_sensitivity;
        double mouse_rotation_sensitivity;
        double mouse_move_sensitivity;
        double mouse_zoom_sensitivity;
        double ctrl_sensitivity_modifier;
    } camera;
    fs::path screenshot_save_path;
    std::vector<Object> objects;

    Scene(const fs::path&);
};