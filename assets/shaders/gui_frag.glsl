#version 330 core

in vec2 passTex;

out vec4 outColor;

uniform sampler2D tex;

void main()
{
    outColor = texture(tex, passTex);

    if (outColor.a == 0.0)
    {
        discard;
    }
}