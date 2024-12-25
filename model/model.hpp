#pragma once

#include <filesystem>
#include <json/value.h>

class Model
{
    bool check_texture_size(const std::filesystem::path&, const Json::Value&, const std::string&, const std::string&);
    void read_bones(const Json::Value&, const std::filesystem::path&);
public:
    struct Cube
    {
        double origin[3];
        double size[3];
        double pivot[3];
        double rotation[3];
        struct
        {
            double east[4];
            double south[4];
            double west[4];
            double north[4];
            double up[4];
            double down[4];
        } uv;
    };

    struct Bone
    {
        double pivot[3];
        double rotation[3];
        bool mirror;
        std::vector<Bone*> children;
        std::vector<Cube*> cubes;
        ~Bone();
    };

    std::vector<Bone*> bones;

    Model(const std::filesystem::path&, const std::filesystem::path&);
    ~Model();
};