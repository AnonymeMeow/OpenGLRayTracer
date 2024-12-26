#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 36) out;

in vec3 origin_vs[];
in mat3 edges_vs[];
in UV
{
    vec4 east;
    vec4 south;
    vec4 west;
    vec4 north;
    vec4 up;
    vec4 down;
} uv_vs[];

out vec2 texCoord;

struct Camera
{
    vec3 position;
    vec2 orientation;
    float hfov, d;
};

uniform Camera camera;

vec4 position(vec3 pos)
{
    return vec4(pos.xy, (pos.z - camera.d) * camera.hfov, (pos.z + camera.d) * camera.hfov);
}

void triangle(vec3 v1, vec3 v2, vec3 v3, vec2 t1, vec2 t2, vec2 t3)
{
    gl_Position = position(v1);
    texCoord = t1;
    EmitVertex();
    gl_Position = position(v2);
    texCoord = t2;
    EmitVertex();
    gl_Position = position(v3);
    texCoord = t3;
    EmitVertex();
    EndPrimitive();
}

void face(vec3 origin, vec3 edge_x, vec3 edge_y, vec4 uv)
{
    triangle(origin, origin + edge_x, origin + edge_x + edge_y,
        uv.xy, uv.xy + vec2(uv.z, 0), uv.xy + uv.zw);
    triangle(origin + edge_x + edge_y, origin + edge_y, origin,
        uv.xy + uv.zw, uv.xy + vec2(0, uv.w), uv.xy);
}

void main()
{
    vec3 origin = origin_vs[0];
    mat3 edges = edges_vs[0];

    face(origin + edges[0] + edges[1], -edges[0], -edges[1], uv_vs[0].north);
    face(origin + edges[1],  edges[2], -edges[1], uv_vs[0].west);
    face(origin + edges[1] + edges[2],  edges[0], -edges[1], uv_vs[0].south);
    face(origin + edges[0] + edges[1] + edges[2], -edges[2], -edges[1], uv_vs[0].east);
    face(origin + edges[0] + edges[1] + edges[2], -edges[0], -edges[2], uv_vs[0].up);
    face(origin + edges[0], -edges[0],  edges[2], uv_vs[0].down);
}