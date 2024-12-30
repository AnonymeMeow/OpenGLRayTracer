#pragma once

#include "cube.hpp"
#include "model.hpp"
#include "pose.hpp"
#include <memory>

class Scene
{
public:
    struct Object
    {
        double position[3];
        double rotation[3];
        double zoom;
        Model model;
        Object(const Json::Value&, const Json::Value&, const Json::Value&, const fs::path&, const fs::path&);
    };

    int altas_max_width = 1024;
    int altas_width, altas_height;

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
    void gen_altas(const Texture&);

    template <gl_floating_point P, gl_floating_point T>
    void push_cubes(CubeArray<P, T>& cubes, const std::unique_ptr<Model::Bone>& bone, const PoseTransform& model_pose, const PoseTransform& bone_pose, double zoom, const Model::TexInfo& tex_info) const
    {
        PoseTransform poseBone = bone_pose * PoseTransform(bone->rotation, bone->pivot);
        for (const auto& cube: bone->cubes)
        {
            PoseTransform poseCube = poseBone * PoseTransform(cube->rotation, cube->pivot);
            cubes.emplace_back(*cube, poseCube, model_pose, zoom, tex_info, altas_width, altas_height);
        }
        for (const auto& child: bone->children)
        {
            push_cubes(cubes, child, model_pose, poseBone, zoom, tex_info);
        }
    }
    template <gl_floating_point P = GLfloat, gl_floating_point T = GLfloat>
    CubeArray<P, T> build_cube_array() const
    {
        CubeArray<P, T> cubes;
        PoseTransform id_pose(Quaternion(1, 0, 0, 0), Quaternion(0, 0, 0, 0));
        for (const auto& object: objects)
        {
            PoseTransform pose(object.rotation);
            pose.translation = Quaternion(0, object.position[0], object.position[1], object.position[2]);
            for (const auto& bone: object.model.bones)
            {
                push_cubes<P, T>(cubes, bone, pose, id_pose, object.zoom, object.model.tex_info);
            }
        }
        return cubes;
    }
};