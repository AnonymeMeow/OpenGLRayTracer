#include "scene.hpp"

#include "../console/logger.hpp"

#include <cmath>
#include <fstream>
#include <json/json.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

extern Logger modelLogger;

const Json::Value& aquire_array(const Json::Value& json, const char* const name, int size, const fs::path& path)
{
    if (!json.isMember(name) || !json[name].isArray() || (size != 0 && json[name].size() != size))
    {
        modelLogger.error("Scene file {} does not have a valid `{}` array field.", path.string(), name);
        exit(-1);
    }
    return json[name];
}

const Json::Value& aquire_double(const Json::Value& json, const char* const name, const fs::path& path)
{
    if (!json.isMember(name) || !json[name].isDouble())
    {
        modelLogger.error("Scene file {} does not have a valid `{}` field.", path.string(), name);
        exit(-1);
    }
    return json[name];
}

Scene::Scene(const fs::path& scene_path)
{
    std::ifstream scene_file(scene_path);
    if (!scene_file)
    {
        modelLogger.error("Failed to open scene file: {}.", scene_path.string());
        exit(-1);
    }
    Json::Value scene_json;
    scene_file >> scene_json;
    scene_file.close();

    if (!scene_json.isObject())
    {
        modelLogger.error("Scene file {} is not a JSON object.", scene_path.string());
        exit(-1);
    }
    const Json::Value& window_size_json = aquire_array(scene_json, "window_size", 2, scene_path);
    window_size[0] = window_size_json[0].asInt();
    window_size[1] = window_size_json[1].asInt();
    if (scene_json.isMember("window_name"))
    {
        if (!scene_json["window_name"].isString())
        {
            modelLogger.error("Scene file {} have an invalid `window_name` field.", scene_path.string());
            exit(-1);
        }
        window_name = scene_json["window_name"].asString();
    }
    else
    {
        window_name = "RayTrace";
    }
    if (!scene_json.isMember("camera") || !scene_json["camera"].isObject())
    {
        modelLogger.error("Scene file {} does not have a valid `camera` field.", scene_path.string());
        exit(-1);
    }
    const Json::Value& camera_json = scene_json["camera"];
    const Json::Value& camera_position_json = aquire_array(camera_json, "position", 3, scene_path);
    camera.position[0] = camera_position_json[0].asDouble();
    camera.position[1] = camera_position_json[1].asDouble();
    camera.position[2] = camera_position_json[2].asDouble();
    const Json::Value& camera_orien_json = aquire_array(camera_json, "orientation", 2, scene_path);
    camera.orientation[0] = camera_orien_json[0].asDouble();
    camera.orientation[1] = camera_orien_json[1].asDouble();
    camera.fov = aquire_double(camera_json, "fov", scene_path).asDouble();
    camera.d = aquire_double(camera_json, "d", scene_path).asDouble();
    camera.keyboard_sensitivity = aquire_double(camera_json, "keyboard_sensitivity", scene_path).asDouble();
    camera.mouse_rotation_sensitivity = aquire_double(camera_json, "mouse_rotation_sensitivity", scene_path).asDouble();
    camera.mouse_move_sensitivity = aquire_double(camera_json, "mouse_move_sensitivity", scene_path).asDouble();
    camera.mouse_zoom_sensitivity = aquire_double(camera_json, "mouse_zoom_sensitivity", scene_path).asDouble();
    camera.ctrl_sensitivity_modifier = aquire_double(camera_json, "ctrl_sensitivity_modifier", scene_path).asDouble();
    if (scene_json.isMember("screenshot_save_path"))
    {
        if (!scene_json["screenshot_save_path"].isString())
        {
            modelLogger.error("Scene file {} have an invalid `screenshot_save_path` field.", scene_path.string());
            exit(-1);
        }
        screenshot_save_path = scene_path.parent_path() / scene_json["screenshot_save_path"].asString();
    }
    else
    {
        screenshot_save_path = scene_path.parent_path();
    }
    if (!scene_json.isMember("objects") || !scene_json["objects"].isArray())
    {
        modelLogger.error("Scene file {} does not have a valid `objects` field.", scene_path.string());
        exit(-1);
    }
    for (const Json::Value& object_json: scene_json["objects"])
    {
        if (!object_json.isObject())
        {
            modelLogger.error("Scene file {} have a non-object `objects` element.", scene_path.string());
            exit(-1);
        }
        const Json::Value& position_json = aquire_array(object_json, "position", 3, scene_path);
        const Json::Value& rotation_json = aquire_array(object_json, "rotation", 3, scene_path);
        const Json::Value& zoom_json = aquire_double(object_json, "zoom", scene_path);
        if (!object_json.isMember("model") || !object_json["model"].isString())
        {
            modelLogger.error("Scene file {} does not have a valid `model` field.", scene_path.string());
            exit(-1);
        }
        const Json::Value& model_json = object_json["model"];
        if (!object_json.isMember("texture") || !object_json["texture"].isString())
        {
            modelLogger.error("Scene file {} does not have a valid `texture` field.", scene_path.string());
            exit(-1);
        }
        const Json::Value& texture_json = object_json["texture"];
        objects.emplace_back(position_json, rotation_json, zoom_json, model_json.asString(), texture_json.asString()
        );
    }
}

Scene::Object::Object(
    const Json::Value& position_json,
    const Json::Value& rotation_json,
    const Json::Value& zoom_json,
    const fs::path& model,
    const fs::path& texture
):
    model(model, texture)
{
    position[0] = position_json[0].asDouble();
    position[1] = position_json[1].asDouble();
    position[2] = position_json[2].asDouble();
    rotation[0] = rotation_json[0].asDouble();
    rotation[1] = rotation_json[1].asDouble();
    rotation[2] = rotation_json[2].asDouble();
    zoom = zoom_json.asDouble();
}

void Scene::gen_altas(const Texture& altas)
{
    int max_width = 0, max_height = 0;
    for (const auto& object: objects)
    {
        if (object.model.tex_info.size[0] > max_width)
        {
            max_width = object.model.tex_info.size[0];
            if (max_width > altas_max_width)
            {
                modelLogger.error("Texture {} too wide.", object.model.tex_info.path.string());
                exit(-1);
            }
        }
        if (object.model.tex_info.size[1] > max_height)
        {
            max_height = object.model.tex_info.size[1];
        }
    }
    int rows = altas_max_width / max_width;
    altas_width = rows * max_width;
    altas_height = std::ceil((double)objects.size() / rows) * max_height;
    altas.allocate(altas_width, altas_height, GL_RGBA8);
    int i = 0, j = 0;
    for (auto& object: objects)
    {
        object.model.tex_info.location[0] = i * max_width;
        object.model.tex_info.location[1] = j * max_height;

        int width, height, n;
        unsigned char* tex = stbi_load(object.model.tex_info.path.c_str(), &width, &height, &n, 0);
        if (tex == NULL)
        {
            modelLogger.error("Failed to load texture {}: {}.", object.model.tex_info.path.string(), stbi_failure_reason());
            exit(-1);
        }

        GLenum format;
        switch (n)
        {
        case 4:
            format = GL_RGBA;
            break;
        case 3:
            format = GL_RGB;
            break;
        default:
            modelLogger.error("Unsupported texture channel size in texture {}.", object.model.tex_info.path.string());
            exit(-1);
        }

        altas.buffer(i * max_width, j * max_height, width, height, format, tex);

        stbi_image_free(tex);

        i++;
        if (i >= rows)
        {
            j++;
            i = 0;
        }
    }
}