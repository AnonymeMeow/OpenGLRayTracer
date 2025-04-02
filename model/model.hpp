#pragma once

#include <filesystem>
#include <json/value.h>

namespace fs = std::filesystem;

class Model
{
    bool check_texture_size(const fs::path&, const Json::Value&, const std::string&, const std::string&);
    void read_bones(const Json::Value&, const fs::path&);
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
        std::vector<std::unique_ptr<Bone>> children;
        std::vector<std::unique_ptr<Cube>> cubes;
    };

    std::vector<std::unique_ptr<Bone>> bones;
    struct TexInfo
    {
        fs::path path;
        int location[2];
        int size[2];
    } tex_info;
    double glow = 0.;
    double metallic = 0.;

    Model(const fs::path&, const fs::path&);
};