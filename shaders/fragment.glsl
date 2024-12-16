#version 330 core

in float d;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2DArray altas;

void main()
{
    vec4 color1 = vec4(0.5f + d / 5.f, 0.5f - d / 2.f, 0.5f, 1.f);
    vec4 color2 = texture(altas, vec3(texCoord, 0));
    FragColor = mix(color1, color2, color2.a);
}