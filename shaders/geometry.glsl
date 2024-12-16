#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 100) out;

in vec2 TexCoord[];

out float d;
out vec2 texCoord;

void main()
{
    gl_Position = gl_in[0].gl_Position + vec4(-1.5f, -0.5f, 1.f, 1.f);
    d = gl_in[0].gl_Position.z / 2.f;
    texCoord = TexCoord[0];
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + vec4(-1.5f, -0.5f, 1.f, 1.f);
    d = gl_in[1].gl_Position.z / 2.f;
    texCoord = TexCoord[1];
    EmitVertex();
    gl_Position = gl_in[2].gl_Position + vec4(-1.5f, -0.5f, 1.f, 1.f);
    d = gl_in[2].gl_Position.z / 2.f;
    texCoord = TexCoord[2];
    EmitVertex();

    EndPrimitive();

    gl_Position = gl_in[0].gl_Position + vec4(1.5f, -0.5f, 1.f, 1.f);
    d = gl_in[0].gl_Position.z / 2.f;
    texCoord = TexCoord[0];
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + vec4(1.5f, -0.5f, 1.f, 1.f);
    d = gl_in[1].gl_Position.z / 2.f;
    texCoord = TexCoord[1];
    EmitVertex();
    gl_Position = gl_in[2].gl_Position + vec4(1.5f, -0.5f, 1.f, 1.f);
    d = gl_in[2].gl_Position.z / 2.f;
    texCoord = TexCoord[2];
    EmitVertex();

    EndPrimitive();
}