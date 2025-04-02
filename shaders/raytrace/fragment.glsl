#version 330 core

in Ray_VS
{
    vec3 origin;
    vec3 direction;
} initial_ray;

struct Cube
{
    sampler2D origin_size;
    sampler2D rotation;
    sampler2D uv;
    sampler2D material;
};

uniform Cube cube;
uniform sampler2D altas;
uniform int count;

struct Ray
{
    vec3 origin;
    vec3 direction;
    vec4 color;
};

#define INF_F 114514.f
#define EPSILON 1e-3f

float seed;

mat3 diag(vec3 v)
{
    return mat3(
        v.x, 0, 0,
        0, v.y, 0,
        0, 0, v.z
    );
}

float random()
{
    seed += 5.14114;
    if (seed > 100) seed *= -1;
    return fract(sin(seed) * 43758.5453);
}

void check_hit(inout Ray ray, out bool is_hit)
{
    float k_min = INF_F;
    vec4 color;
    vec3 normal;
    float glow;
    float metallic;
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
        float cube_glow = texelFetch(cube.material, ivec2(i, j), 0).r;
        float cube_metallic = texelFetch(cube.material, ivec2(i, j), 0).g;

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

        if (k.z > EPSILON && k.z < k_min)
        {
            vec2 colli = ori_rel.xy + k.z * dir_rel.xy;
            if (all(greaterThan(colli, vec2(0))) && all(lessThan(colli, cube_size.xy)))
            {
                vec2 tex_coord = colli / cube_size.xy;
                tex_coord = vec2(1. - tex_coord.x, 1. - tex_coord.y);
                color = texture(altas, cube_uv_north.xy + tex_coord * cube_uv_north.zw);
                k_min = k.z;
                normal = vec3(0., 0., -1.) * rot_cube;
                glow = cube_glow;
                metallic = cube_metallic;
            }
        }
        if (k1.z > EPSILON && k1.z < k_min)
        {
            vec2 colli = ori_rel.xy + k1.z * dir_rel.xy;
            if (all(greaterThan(colli, vec2(0))) && all(lessThan(colli, cube_size.xy)))
            {
                vec2 tex_coord = colli / cube_size.xy;
                tex_coord = vec2(tex_coord.x, 1. - tex_coord.y);
                color = texture(altas, cube_uv_south.xy + tex_coord * cube_uv_south.zw);
                k_min = k1.z;
                normal = vec3(0., 0., 1.) * rot_cube;
                glow = cube_glow;
                metallic = cube_metallic;
            }
        }
        if (k.y > EPSILON && k.y < k_min)
        {
            vec2 colli = ori_rel.xz + k.y * dir_rel.xz;
            if (all(greaterThan(colli, vec2(0))) && all(lessThan(colli, cube_size.xz)))
            {
                vec2 tex_coord = colli / cube_size.xz;
                tex_coord = vec2(1. - tex_coord.x, tex_coord.y);
                color = texture(altas, cube_uv_down.xy + tex_coord * cube_uv_down.zw);
                k_min = k.y;
                normal = vec3(0., -1., 0.) * rot_cube;
                glow = cube_glow;
                metallic = cube_metallic;
            }
        }
        if (k1.y > EPSILON && k1.y < k_min)
        {
            vec2 colli = ori_rel.xz + k1.y * dir_rel.xz;
            if (all(greaterThan(colli, vec2(0))) && all(lessThan(colli, cube_size.xz)))
            {
                vec2 tex_coord = colli / cube_size.xz;
                tex_coord = vec2(1. - tex_coord.x, 1. - tex_coord.y);
                color = texture(altas, cube_uv_up.xy + tex_coord * cube_uv_up.zw);
                k_min = k1.y;
                normal = vec3(0., 1., 0.) * rot_cube;
                glow = cube_glow;
                metallic = cube_metallic;
            }
        }
        if (k.x > EPSILON && k.x < k_min)
        {
            vec2 colli = ori_rel.yz + k.x * dir_rel.yz;
            if (all(greaterThan(colli, vec2(0))) && all(lessThan(colli, cube_size.yz)))
            {
                vec2 tex_coord = colli / cube_size.yz;
                tex_coord = vec2(tex_coord.y, 1. - tex_coord.x);
                color = texture(altas, cube_uv_west.xy + tex_coord * cube_uv_west.zw);
                k_min = k.x;
                normal = vec3(-1., 0., 0.) * rot_cube;
                glow = cube_glow;
                metallic = cube_metallic;
            }
        }
        if (k1.x > EPSILON && k1.x < k_min)
        {
            vec2 colli = ori_rel.yz + k1.x * dir_rel.yz;
            if (all(greaterThan(colli, vec2(0))) && all(lessThan(colli, cube_size.yz)))
            {
                vec2 tex_coord = colli / cube_size.yz;
                tex_coord = vec2(1. - tex_coord.y, 1. - tex_coord.x);
                color = texture(altas, cube_uv_east.xy + tex_coord * cube_uv_east.zw);
                k_min = k1.x;
                normal = vec3(1., 0., 0.) * rot_cube;
                glow = cube_glow;
                metallic = cube_metallic;
            }
        }
    }
    if (k_min == INF_F)
    {
        is_hit = false;
        // float cos_angle = dot(normalize(ray.direction), vec3(-0.6, 0.8, 0.));
        // float light = cos_angle * 20. - 10.; // * cos_angle * cos_angle * 2. - 1.;
        // if (light < 0) light = 0;
        // ray.color *= vec4(light, light, light, 1.);
        ray.color *= vec4(0., 0., 0., 1.);
    }
    else
    {
        if (color.a == 0)
        {
            is_hit = true;
            ray.origin = ray.origin + k_min * ray.direction;
            return;
        }
        if (random() < glow)
        {
            is_hit = false;
            if (glow > 1) color *= glow;
            ray.color *= color;
            return;
        }
        is_hit = true;
        ray.origin = ray.origin + k_min * ray.direction;
        if (random() < metallic)
        {
            ray.direction = ray.direction - 2. * dot(normal, ray.direction) * normal;
            if (metallic > 1) color = mix(color, vec4(1.), 1 / metallic);
            ray.color *= color;
            return;
        }
        ray.color *= color;
        float direction_normal = random();
        vec3 x = vec3(normal.z, 0., -normal.x);
        vec3 y = vec3(0., normal.z, -normal.y);
        vec3 tan1 = normalize(length(x) > length(y)? x: y);
        vec3 tan2 = cross(normal, tan1);
        float angle = random() * 3.14159265358979 * 2;
        if (dot(normal, ray.direction) > 0) direction_normal = - direction_normal;
        ray.direction = direction_normal * normal + cos(angle) * tan1 + sin(angle) * tan2;
    }
}

#define SAMPLE_COUNT 10

void main()
{
    seed = dot(initial_ray.direction, vec3(1.14, 5.14, 19.19));
    seed = random();
    Ray ray;
    vec4 final_color = vec4(0.);
    for (int j = 0; j < SAMPLE_COUNT; j++)
    {
        ray.origin = initial_ray.origin;
        ray.direction = initial_ray.direction;
        ray.color = vec4(1.0);


        bool is_hit = true;
        for (int i = 0; i < 5 && is_hit; i++)
        {
            check_hit(ray, is_hit);
            float prob = max(max(ray.color.r, ray.color.g), ray.color.b);
            if (random() > prob)
            {
                break;
            }
            ray.color /= prob;
        }
        if (is_hit)
        {
            ray.color *= vec4(0., 0., 0., 1.);
        }
        final_color += ray.color;
    }
    final_color /= SAMPLE_COUNT;
    gl_FragColor = final_color;
}