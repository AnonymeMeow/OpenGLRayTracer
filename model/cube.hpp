#pragma once

#include "../opengl/shader.hpp"
#include "model.hpp"
#include "pose.hpp"

#include <concepts>

template <typename T>
concept gl_floating_point = is_gl_type<T> && std::floating_point<T>;

template <gl_floating_point T>
struct _cube_origin_size
{
    T origin[3];
    T size[3];
};

template <gl_floating_point T>
struct _cube_rotation
{
    T rotation[4];
};

template <gl_floating_point T>
struct _cube_uv
{
    T east[4];
    T south[4];
    T west[4];
    T north[4];
    T up[4];
    T down[4];
};

template <
    gl_floating_point PositionDataType = GLfloat,
    gl_floating_point TextureDataType = GLfloat
>
struct Cube: _cube_origin_size<PositionDataType>, _cube_rotation<PositionDataType>, _cube_uv<TextureDataType>
{
    Cube(const Model::Cube& cube, const Quaternion& position, const Quaternion& rotation, int tex_width, int tex_height):
        _cube_origin_size<PositionDataType>{
            (PositionDataType)position.x, (PositionDataType)position.y, (PositionDataType)position.z,
            (PositionDataType)cube.size[0], (PositionDataType)cube.size[1], (PositionDataType)cube.size[2]
        },
        _cube_rotation<PositionDataType>{(PositionDataType)rotation.x, (PositionDataType)rotation.y, (PositionDataType)rotation.z, (PositionDataType)rotation.w},
        _cube_uv<TextureDataType>{
            (TextureDataType)(cube.uv.east[0] / tex_width), (TextureDataType)(cube.uv.east[1] / tex_height), (TextureDataType)(cube.uv.east[2] / tex_width), (TextureDataType)(cube.uv.east[3] / tex_height),
            (TextureDataType)(cube.uv.south[0] / tex_width), (TextureDataType)(cube.uv.south[1] / tex_height), (TextureDataType)(cube.uv.south[2] / tex_width), (TextureDataType)(cube.uv.south[3] / tex_height),
            (TextureDataType)(cube.uv.west[0] / tex_width), (TextureDataType)(cube.uv.west[1] / tex_height), (TextureDataType)(cube.uv.west[2] / tex_width), (TextureDataType)(cube.uv.west[3] / tex_height),
            (TextureDataType)(cube.uv.north[0] / tex_width), (TextureDataType)(cube.uv.north[1] / tex_height), (TextureDataType)(cube.uv.north[2] / tex_width), (TextureDataType)(cube.uv.north[3] / tex_height),
            (TextureDataType)(cube.uv.up[0] / tex_width), (TextureDataType)(cube.uv.up[1] / tex_height), (TextureDataType)(cube.uv.up[2] / tex_width), (TextureDataType)(cube.uv.up[3] / tex_height),
            (TextureDataType)(cube.uv.down[0] / tex_width), (TextureDataType)(cube.uv.down[1] / tex_height), (TextureDataType)(cube.uv.down[2] / tex_width), (TextureDataType)(cube.uv.down[3] / tex_height)
        }
    {}
};

template <gl_floating_point P = GLfloat, gl_floating_point T = GLfloat>
struct CubeArray: std::vector<Cube<P, T>>
{
    void set_input(Program& program) const
    {
        program.input.loadMemoryModel<Cube<P, T>>(
            &Cube<P, T>::origin,
            &Cube<P, T>::size,
            &Cube<P, T>::rotation,
            &Cube<P, T>::east,
            &Cube<P, T>::south,
            &Cube<P, T>::west,
            &Cube<P, T>::north,
            &Cube<P, T>::up,
            &Cube<P, T>::down
        );
        program.input.setVertices(*this);
    }
};

template <
    gl_floating_point PositionDataType = GLfloat,
    gl_floating_point TextureDataType = GLfloat
>
    requires (sizeof(PositionDataType) == 32 && sizeof(TextureDataType) == 32)
struct TextureCube
{
    int cubes_per_row = 128;
    std::vector<_cube_origin_size<PositionDataType>> origin_size;
    std::vector<_cube_rotation<PositionDataType>> rotation;
    std::vector<_cube_uv<TextureDataType>> uv;

    TextureCube(const CubeArray<PositionDataType, TextureDataType>& cube_array)
    {
        for (const auto& cube: cube_array)
        {
            origin_size.push_back(static_cast<_cube_origin_size<PositionDataType>>(cube));
            rotation.push_back(static_cast<_cube_rotation<PositionDataType>>(cube));
            uv.push_back(static_cast<_cube_uv<TextureDataType>>(cube));
        }
    }
    void buffer_to_texture(const Texture& origin_size_tex, const Texture& rotation_tex, const Texture& uv_tex) const
    {
        int columns = origin_size.size() / cubes_per_row;
        origin_size_tex.allocate(2 * cubes_per_row, columns, GL_RGB32F);
        origin_size_tex.buffer(0, 0, 2 * cubes_per_row, columns, GL_RGB, origin_size.data());
        rotation_tex.allocate(cubes_per_row, columns, GL_RGBA32F);
        rotation_tex.buffer(0, 0, cubes_per_row, columns, GL_RGBA, rotation.data());
        uv_tex.allocate(6 * cubes_per_row, columns, GL_RGBA32F);
        uv_tex.buffer(0, 0, 6 * cubes_per_row, columns, GL_RGBA, uv.data());
    }
};