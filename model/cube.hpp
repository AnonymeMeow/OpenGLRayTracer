#pragma once

#include <concepts>

template <
    std::floating_point PositionDataType = float,
    std::floating_point TextureDataType = float
>
struct Cube
{
    PositionDataType origin[3];
    PositionDataType size[3];
    PositionDataType rotation[3];
    TextureDataType uv_east[4];
    TextureDataType uv_south[4];
    TextureDataType uv_west[4];
    TextureDataType uv_north[4];
    TextureDataType uv_up[4];
    TextureDataType uv_down[4];
};