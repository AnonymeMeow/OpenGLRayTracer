#include "scene.hpp"

#include "../console/logger.hpp"

#include <fstream>
#include <json/json.h>

extern Logger modelLogger;

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
    if (!scene_json.isMember("window_size") || !scene_json["window_size"].isArray() || scene_json["window_size"].size() != 2)
    {
        modelLogger.error("Scene file {} does not have a valid `window_size` field.", scene_path.string());
        exit(-1);
    }
    const Json::Value& window_size_json = scene_json["window_size"];
    window_size[0] = window_size_json[0].asInt();
    window_size[1] = window_size_json[1].asInt();
    if (!scene_json.isMember("window_name") || !scene_json["window_name"].isString())
    {
        modelLogger.error("Scene file {} does not have a valid `window_name` field.", scene_path.string());
        exit(-1);
    }
    window_name = scene_json["window_name"].asString();
    if (!scene_json.isMember("camera") || !scene_json["camera"].isObject())
    {
        modelLogger.error("Scene file {} does not have a valid `camera` field.", scene_path.string());
        exit(-1);
    }
    const Json::Value& camera_json = scene_json["camera"];
    camera.position[0] = camera_json["position"][0].asDouble();
    camera.position[1] = camera_json["position"][1].asDouble();
    camera.position[2] = camera_json["position"][2].asDouble();
    camera.orientation[0] = camera_json["orientation"][0].asDouble();
    camera.orientation[1] = camera_json["orientation"][1].asDouble();
    camera.fov = camera_json["fov"].asDouble();
    camera.d = camera_json["d"].asDouble();
    camera.keyboard_sensitivity = camera_json["keyboard_sensitivity"].asDouble();
    camera.mouse_rotation_sensitivity = camera_json["mouse_rotation_sensitivity"].asDouble();
    camera.mouse_move_sensitivity = camera_json["mouse_move_sensitivity"].asDouble();
    camera.mouse_zoom_sensitivity = camera_json["mouse_zoom_sensitivity"].asDouble();
    camera.ctrl_sensitivity_modifier = camera_json["ctrl_sensitivity_modifier"].asDouble();
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
        if (!object_json.isMember("position") || !object_json["position"].isArray() || object_json["position"].size() != 3)
        {
            modelLogger.error("Scene file {} does not have a valid `position` field.", scene_path.string());
            exit(-1);
        }
        const Json::Value& position_json = object_json["position"];
        if (!object_json.isMember("rotation") || !object_json["rotation"].isArray() || object_json["rotation"].size() != 3)
        {
            modelLogger.error("Scene file {} does not have a valid `rotation` field.", scene_path.string());
            exit(-1);
        }
        const Json::Value& rotation_json = object_json["rotation"];
        if (!object_json.isMember("zoom") || !object_json["zoom"].isDouble())
        {
            modelLogger.error("Scene file {} does not have a valid `zoom` field.", scene_path.string());
            exit(-1);
        }
        const Json::Value& zoom_json = object_json["zoom"];
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
        objects.emplace_back(
            position_json,
            rotation_json,
            zoom_json,
            model_json.asString(),
            texture_json.asString()
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
    model(model, texture),
    texture(texture)
{
    position[0] = position_json[0].asDouble();
    position[1] = position_json[1].asDouble();
    position[2] = position_json[2].asDouble();
    rotation[0] = rotation_json[0].asDouble();
    rotation[1] = rotation_json[1].asDouble();
    rotation[2] = rotation_json[2].asDouble();
    zoom = zoom_json.asDouble();
}