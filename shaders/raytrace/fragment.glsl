#version 330 core

in Ray
{
    vec3 origin;
    vec3 direction;
} ray;

struct Cube
{
    sampler2D origin_size;
    sampler2D rotation;
    sampler2D uv;
};

uniform Cube cube;
uniform sampler2D altas;
uniform int count;

mat3 diag(vec3 v)
{
    return mat3(
        v.x, 0, 0,
        0, v.y, 0,
        0, 0, v.z
    );
}

void main()
{
    float k_min = 114.f;
    vec4 color_tmp = vec4(1.0);

    for (int index = 0; index < count; index++)
    {
        int i = index % 128, j = index / 128;
        vec3 cube_origin = texelFetch(cube.origin_size, ivec2(i * 2, j), 0).rgb;
        vec3 cube_size = texelFetch(cube.origin_size, ivec2(i * 2 + 1, j), 0).rgb;
        vec4 cube_rotation = texelFetch(cube.rotation, ivec2(i, j), 0);
        vec4 cube_uv_east = texelFetch(cube.uv, ivec2(i * 6, j), 0);
        vec4 cube_uv_south = texelFetch(cube.uv, ivec2(i * 6 + 1, j), 0);
        vec4 cube_uv_west = texelFetch(cube.uv, ivec2(i * 6 + 2, j), 0);
        vec4 cube_uv_north = texelFetch(cube.uv, ivec2(i * 6 + 3, j), 0);
        vec4 cube_uv_up = texelFetch(cube.uv, ivec2(i * 6 + 4, j), 0);
        vec4 cube_uv_down = texelFetch(cube.uv, ivec2(i * 6 + 5, j), 0);

        vec4 q = cube_rotation;
        mat3 rot_cube = 2 * mat3(
            1 - q.y * q.y - q.z * q.z, q.x * q.y + q.z * q.w, q.x * q.z - q.y * q.w,
            q.x * q.y - q.z * q.w, 1 - q.x * q.x - q.z * q.z, q.y * q.z + q.x * q.w,
            q.x * q.z + q.y * q.w, q.y * q.z - q.x * q.w, 1 - q.x * q.x - q.y * q.y
        ) - diag(vec3(1));
        
        vec3 ori_rel = (ray.origin - cube_origin) * rot_cube;
        vec3 dir_rel = ray.direction * rot_cube;

        vec3 k = - ori_rel / dir_rel;
        vec3 k1 = k + cube_size / dir_rel;

        if (k.z > 0 && k.z < k_min)
        {
            vec2 colli = ori_rel.xy + k.z * dir_rel.xy;
            if (all(greaterThan(colli, vec2(0))) && all(lessThan(colli, cube_size.xy)))
            {
                color_tmp = texture(altas, cube_uv_north.xy + colli / cube_size.xy * cube_uv_north.zw);
                k_min = k.z;
            }
        }
        if (k1.z > 0 && k1.z < k_min)
        {
            vec2 colli = ori_rel.xy + k1.z * dir_rel.xy;
            if (all(greaterThan(colli, vec2(0))) && all(lessThan(colli, cube_size.xy)))
            {
                color_tmp = texture(altas, cube_uv_south.xy + colli / cube_size.xy * cube_uv_south.zw);
                k_min = k1.z;
            }
        }
        if (k.y > 0 && k.y < k_min)
        {
            vec2 colli = ori_rel.xz + k.y * dir_rel.xz;
            if (all(greaterThan(colli, vec2(0))) && all(lessThan(colli, cube_size.xz)))
            {
                color_tmp = texture(altas, cube_uv_down.xy + colli / cube_size.xz * cube_uv_down.zw);
                k_min = k.y;
            }
        }
        if (k1.y > 0 && k1.y < k_min)
        {
            vec2 colli = ori_rel.xz + k1.y * dir_rel.xz;
            if (all(greaterThan(colli, vec2(0))) && all(lessThan(colli, cube_size.xz)))
            {
                color_tmp = texture(altas, cube_uv_up.xy + colli / cube_size.xz * cube_uv_up.zw);
                k_min = k1.y;
            }
        }
        if (k.x > 0 && k.x < k_min)
        {
            vec2 colli = ori_rel.yz + k.x * dir_rel.yz;
            if (all(greaterThan(colli, vec2(0))) && all(lessThan(colli, cube_size.yz)))
            {
                color_tmp = texture(altas, cube_uv_east.xy + colli / cube_size.yz * cube_uv_east.zw);
                k_min = k.x;
            }
        }
        if (k1.x > 0 && k1.x < k_min)
        {
            vec2 colli = ori_rel.yz + k1.x * dir_rel.yz;
            if (all(greaterThan(colli, vec2(0))) && all(lessThan(colli, cube_size.yz)))
            {
                color_tmp = texture(altas, cube_uv_west.xy + colli / cube_size.yz * cube_uv_west.zw);
                k_min = k1.x;
            }
        }
    }
    if (k_min == 114.f)
    {
        discard;
    }
    gl_FragColor = color_tmp;
}