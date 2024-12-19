#version 330 core

in vec2 texCoord;

uniform sampler2D altas;

void main()
{
    vec4 color = texture(altas, texCoord);
    if (color.a == 0)
    {
        discard;
    }
    gl_FragColor = color;
}