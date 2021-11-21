#version 330 core
layout (location = 0) in vec2 vPos;
layout (location = 1) in vec2 vTex;

out vec2 passTex;

uniform mat4 proj;
uniform mat4 model;

void main()
{
    gl_Position = proj * model * vec4(vPos, -1.0, 1.0);
    passTex = vTex;
}
