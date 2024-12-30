#include "model.hpp"

#include "../console/logger.hpp"

#include <cmath>
#include <fstream>
#include <json/json.h>
#include <stb/stb_image.h>

Logger modelLogger("Model");

Model::Model(const fs::path& model_path, const fs::path& texture_path)
{
    std::ifstream model_file(model_path);
    if (!model_file)
    {
        modelLogger.error("Failed to open model file: {}.", model_path.string());
        exit(-1);
    }
    Json::Value model_json;
    model_file >> model_json;
    model_file.close();

    if (!model_json.isObject())
    {
        modelLogger.error("Model file {} is not a JSON object.", model_path.string());
        exit(-1);
    }
    if (!model_json.isMember("format_version") || !model_json["format_version"].isString())
    {
        modelLogger.error("Model file {} does not have a `format_version` string field.", model_path.string());
        exit(-1);
    }
    std::string format_version = model_json["format_version"].asString();
    if (format_version.compare("1.10.0") == 0)
    {
        if (!model_json.isMember("geometry.model") || !model_json["geometry.model"].isObject())
        {
            modelLogger.error("Model file {} does not have a valid `geometry.model` field.", model_path.string());
            exit(-1);
        }
        const Json::Value& geometry = model_json["geometry.model"];
        if (!check_texture_size(texture_path, geometry, "texturewidth", "textureheight"))
        {
            modelLogger.error("Texture file {}'s texture size mismatch in model file {}.", texture_path.string(), model_path.string());
        }
        if (!geometry.isMember("bones") || !geometry["bones"].isArray())
        {
            modelLogger.error("Model file {} does not have a `bones` array.", model_path.string());
            exit(-1);
        }
        const Json::Value& bones = geometry["bones"];
        read_bones(bones, model_path);
    }
    else if (format_version.compare("1.12.0") == 0)
    {
        if (!model_json.isMember("minecraft:geometry") || !model_json["minecraft:geometry"].isArray())
        {
            modelLogger.error("Model file {} does not have a `minecraft:geometry` array field.", model_path.string());
            exit(-1);
        }
        const Json::Value& geometries = model_json["minecraft:geometry"];
        for (const Json::Value& geometry: geometries)
        {
            if (!geometry.isObject())
            {
                modelLogger.error("Model file {} has a non-object element in `minecraft:geometry` array.", model_path.string());
                exit(-1);
            }
            if (!geometry.isMember("description") || !geometry["description"].isObject())
            {
                modelLogger.error("One `minecraft:geometry` element in model file {} does not have a valid `description` field.", model_path.string());
                exit(-1);
            }
            const Json::Value& description = geometry["description"];
            if (!check_texture_size(texture_path, description, "texture_width", "texture_height"))
            {
                modelLogger.error("Texture file {}'s texture size mismatch in model file {}.", texture_path.string(), model_path.string());
            }
            if (!geometry.isMember("bones") || !geometry["bones"].isArray())
            {
                modelLogger.error("One `minecraft:geometry` element in model file {} does not have a `bones` array.", model_path.string());
                exit(-1);
            }
            const Json::Value& bones = geometry["bones"];
            read_bones(bones, model_path);
        }
    }
    else
    {
        modelLogger.error("Unsupported format version {} in model file {}.", format_version, model_path.string());
        exit(-1);
    }
}

bool Model::check_texture_size(const fs::path& texture_path, const Json::Value& geometry, const std::string& width_field, const std::string& height_field)
{
    int texture_width, texture_height, n;
    if (stbi_info(texture_path.c_str(), &texture_width, &texture_height, &n) == 0)
    {
        modelLogger.error("Failed to read texture file {}: {}", texture_path.string(), stbi_failure_reason());
        exit(-1);
    }
    if (!geometry.isMember(width_field) || !geometry[width_field].isInt() ||
        !geometry.isMember(height_field) || !geometry[height_field].isInt())
    {
        modelLogger.error("Could not find information about texture {}'s size in model file.", texture_path.string());
        exit(-1);
    }
    tex_info.size[0] = texture_width;
    tex_info.size[1] = texture_height;
    tex_info.path = texture_path;
    return texture_width == geometry[width_field].asInt() && texture_height == geometry[height_field].asInt();
}

void Model::read_bones(const Json::Value& bones, const fs::path& model_path)
{
    std::map<std::string, Bone*> bone_map;
    for (const Json::Value& bone: bones)
    {
        if (!bone.isObject())
        {
            modelLogger.error("Model file {} has a non-object element in `bones` array.", model_path.string());
            exit(-1);
        }
        Bone* new_bone = new Bone();
        if (!bone.isMember("name") || !bone["name"].isString())
        {
            modelLogger.error("Model file {} has a bone without a `name` string field.", model_path.string());
            exit(-1);
        }
        std::string name = bone["name"].asString();
        std::optional<const Bone*> parent;
        if (bone.isMember("parent"))
        {
            if (!bone["parent"].isString())
            {
                modelLogger.error("Bone {} in model file {} has a non-string field `parent`.", name, model_path.string());
                exit(-1);
            }
            std::string parent_name = bone["parent"].asString();
            if (bone_map.contains(parent_name))
            {
                parent = bone_map[parent_name];
                bone_map[parent_name]->children.push_back(std::unique_ptr<Bone>(new_bone));
            }
            else
            {
                modelLogger.error("Bone {} in model file {} has a parent {} that does not exist.", name, model_path.string(), parent_name);
                exit(-1);
            }
        }
        else
        {
            this->bones.push_back(std::unique_ptr<Bone>(new_bone));
        }
        bone_map[name] = new_bone;
        if (!bone.isMember("pivot") || !bone["pivot"].isArray() || bone["pivot"].size() != 3)
        {
            modelLogger.error("Bone {} in model file {} does not have a valid `pivot` array field.", name, model_path.string());
            exit(-1);
        }
        const Json::Value& pivot = bone["pivot"];
        new_bone->pivot[0] = - pivot[0].asDouble();
        new_bone->pivot[1] = pivot[1].asDouble();
        new_bone->pivot[2] = pivot[2].asDouble();
        if (bone.isMember("rotation"))
        {
            const Json::Value& rotation = bone["rotation"];
            if (!rotation.isArray() || rotation.size() != 3)
            {
                modelLogger.error("Bone {} in model file {} has an invalid `rotation` field.", name, model_path.string());
                exit(-1);
            }
            new_bone->rotation[0] = - rotation[0].asDouble();
            new_bone->rotation[1] = - rotation[1].asDouble();
            new_bone->rotation[2] = rotation[2].asDouble();
        }
        else
        {
            new_bone->rotation[0] = 0;
            new_bone->rotation[1] = 0;
            new_bone->rotation[2] = 0;
        }
        if (bone.isMember("mirror"))
        {
            const Json::Value& mirror = bone["mirror"];
            if (!mirror.isBool())
            {
                modelLogger.error("Bone {} in model file {} has an invalid `mirror` field.", name, model_path.string());
                exit(-1);
            }
            new_bone->mirror = mirror.asBool();
        }
        else
        {
            new_bone->mirror = parent.has_value()? parent.value()->mirror: false;
        }
        if (bone.isMember("cubes"))
        {
            const Json::Value& cubes = bone["cubes"];
            if (!cubes.isArray())
            {
                modelLogger.error("Bone {} in model file {} has a non-array `cubes` field.", name, model_path.string());
                exit(-1);
            }
            for (const Json::Value& cube: cubes)
            {
                if (!cube.isObject())
                {
                    modelLogger.error("Model file {} has a non-object element in cubes array.", model_path.string());
                    exit(-1);
                }
                Cube* new_cube = new Cube();
                if (!cube.isMember("origin") || !cube["origin"].isArray() || cube["origin"].size() != 3)
                {
                    modelLogger.error("Bone {} in model file {} has a cube without a valid `origin` array field.", name, model_path.string());
                    exit(-1);
                }
                if (!cube.isMember("size") || !cube["size"].isArray() || cube["size"].size() != 3)
                {
                    modelLogger.error("Bone {} in model file {} has a cube without a valid `size` array field.", name, model_path.string());
                    exit(-1);
                }
                const Json::Value& origin = cube["origin"];
                const Json::Value& size = cube["size"];
                new_cube->size[0] = size[0].asDouble();
                new_cube->size[1] = size[1].asDouble();
                new_cube->size[2] = size[2].asDouble();
                new_cube->origin[0] = - origin[0].asDouble() - new_cube->size[0];
                new_cube->origin[1] = origin[1].asDouble();
                new_cube->origin[2] = origin[2].asDouble();
                if (cube.isMember("pivot"))
                {
                    const Json::Value& pivot = cube["pivot"];
                    if (!pivot.isArray() || pivot.size() != 3)
                    {
                        modelLogger.error("Bone {} in model file {} has a cube with an invalid `pivot` field.", name, model_path.string());
                        exit(-1);
                    }
                    new_cube->pivot[0] = - pivot[0].asDouble();
                    new_cube->pivot[1] = pivot[1].asDouble();
                    new_cube->pivot[2] = pivot[2].asDouble();
                }
                else
                {
                    new_cube->pivot[0] = 0;
                    new_cube->pivot[1] = 0;
                    new_cube->pivot[2] = 0;
                }
                if (cube.isMember("rotation"))
                {
                    const Json::Value& rotation = cube["rotation"];
                    if (!rotation.isArray() || rotation.size() != 3)
                    {
                        modelLogger.error("Bone {} in model file {} has a cube with an invalid `rotation` field.", name, model_path.string());
                        exit(-1);
                    }
                    new_cube->rotation[0] = - rotation[0].asDouble();
                    new_cube->rotation[1] = - rotation[1].asDouble();
                    new_cube->rotation[2] = rotation[2].asDouble();
                }
                else
                {
                    new_cube->rotation[0] = 0;
                    new_cube->rotation[1] = 0;
                    new_cube->rotation[2] = 0;
                }
                if (cube.isMember("inflate"))
                {
                    const Json::Value& inflate_json = cube["inflate"];
                    if (!inflate_json.isDouble())
                    {
                        modelLogger.error("Bone {} in model file {} has a cube with an invalid `inflate` field.", name, model_path.string());
                        exit(-1);
                    }
                    double inflate = inflate_json.asDouble();
                    for (int i = 0; i < 3; i++)
                    {
                        new_cube->origin[i] -= inflate;
                        new_cube->size[i] += inflate * 2;
                    }
                }
                bool mirror;
                if (cube.isMember("mirror"))
                {
                    const Json::Value& mirror_json = cube["mirror"];
                    if (!mirror_json.isBool())
                    {
                        modelLogger.error("Bone {} in model file {} has a cube with an invalid `mirror` field.", name, model_path.string());
                        exit(-1);
                    }
                    mirror = mirror_json.asBool();
                }
                else
                {
                    mirror = new_bone->mirror;
                }
                if (!cube.isMember("uv"))
                {
                    modelLogger.error("Bone {} in model file {} has a cube without `uv` field.", name, model_path.string());
                    exit(-1);
                }
                const Json::Value& uv = cube["uv"];
                if (uv.isObject())
                {
                    new_cube->uv.north[0] = uv["north"]["uv"][0].asDouble();
                    new_cube->uv.north[1] = uv["north"]["uv"][1].asDouble();
                    new_cube->uv.north[2] = uv["north"]["uv_size"][0].asDouble();
                    new_cube->uv.north[3] = uv["north"]["uv_size"][1].asDouble();
                    new_cube->uv.west[0] = uv["west"]["uv"][0].asDouble();
                    new_cube->uv.west[1] = uv["west"]["uv"][1].asDouble();
                    new_cube->uv.west[2] = uv["west"]["uv_size"][0].asDouble();
                    new_cube->uv.west[3] = uv["west"]["uv_size"][1].asDouble();
                    new_cube->uv.south[0] = uv["south"]["uv"][0].asDouble();
                    new_cube->uv.south[1] = uv["south"]["uv"][1].asDouble();
                    new_cube->uv.south[2] = uv["south"]["uv_size"][0].asDouble();
                    new_cube->uv.south[3] = uv["south"]["uv_size"][1].asDouble();
                    new_cube->uv.east[0] = uv["east"]["uv"][0].asDouble();
                    new_cube->uv.east[1] = uv["east"]["uv"][1].asDouble();
                    new_cube->uv.east[2] = uv["east"]["uv_size"][0].asDouble();
                    new_cube->uv.east[3] = uv["east"]["uv_size"][1].asDouble();
                    new_cube->uv.up[0] = uv["up"]["uv"][0].asDouble();
                    new_cube->uv.up[1] = uv["up"]["uv"][1].asDouble();
                    new_cube->uv.up[2] = uv["up"]["uv_size"][0].asDouble();
                    new_cube->uv.up[3] = uv["up"]["uv_size"][1].asDouble();
                    new_cube->uv.down[0] = uv["down"]["uv"][0].asDouble();
                    new_cube->uv.down[1] = uv["down"]["uv"][1].asDouble();
                    new_cube->uv.down[2] = uv["down"]["uv_size"][0].asDouble();
                    new_cube->uv.down[3] = uv["down"]["uv_size"][1].asDouble();
                }
                else if (uv.isArray() && uv.size() == 2)
                {
                    double x = uv[0].asDouble(), y = uv[1].asDouble();
                    double dx = floor(size[0].asDouble()), dy = floor(size[1].asDouble()), dz = floor(size[2].asDouble());
                    new_cube->uv.north[0] = x + dz;
                    new_cube->uv.north[1] = y + dz;
                    new_cube->uv.north[2] = dx;
                    new_cube->uv.north[3] = dy;
                    new_cube->uv.west[0] = x + dz + dx;
                    new_cube->uv.west[1] = y + dz;
                    new_cube->uv.west[2] = dz;
                    new_cube->uv.west[3] = dy;
                    new_cube->uv.south[0] = x + dz + dx + dz;
                    new_cube->uv.south[1] = y + dz;
                    new_cube->uv.south[2] = dx;
                    new_cube->uv.south[3] = dy;
                    new_cube->uv.east[0] = x;
                    new_cube->uv.east[1] = y + dz;
                    new_cube->uv.east[2] = dz;
                    new_cube->uv.east[3] = dy;
                    new_cube->uv.up[0] = x + dz;
                    new_cube->uv.up[1] = y;
                    new_cube->uv.up[2] = dx;
                    new_cube->uv.up[3] = dz;
                    new_cube->uv.down[0] = x + dz + dx;
                    new_cube->uv.down[1] = y + dz;
                    new_cube->uv.down[2] = dx;
                    new_cube->uv.down[3] = - dz;
                }
                else
                {
                    modelLogger.error("Bone {} in model file {} has a cube with an invalid `uv` field.", name, model_path.string());
                    exit(-1);
                }
                if (mirror)
                {
                    new_cube->uv.north[0] = new_cube->uv.north[0] + new_cube->uv.north[2];
                    new_cube->uv.north[2] = - new_cube->uv.north[2];
                    new_cube->uv.west[0] = new_cube->uv.east[0] + new_cube->uv.east[2];
                    new_cube->uv.west[2] = - new_cube->uv.east[2];
                    new_cube->uv.south[0] = new_cube->uv.south[0] + new_cube->uv.south[2];
                    new_cube->uv.south[2] = - new_cube->uv.south[2];
                    new_cube->uv.east[0] = new_cube->uv.west[0] + new_cube->uv.west[2];
                    new_cube->uv.east[2] = - new_cube->uv.west[2];
                    new_cube->uv.up[0] = new_cube->uv.up[0] + new_cube->uv.up[2];
                    new_cube->uv.up[2] = - new_cube->uv.up[2];
                    new_cube->uv.down[0] = new_cube->uv.down[0] + new_cube->uv.down[2];
                    new_cube->uv.down[2] = - new_cube->uv.down[2];
                }
                new_bone->cubes.push_back(std::unique_ptr<Cube>(new_cube));
            }
        }
    }
}