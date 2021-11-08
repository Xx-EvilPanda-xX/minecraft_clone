#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTexCoords;
layout (location = 2) in float vLighting;

out vec2 passTexCoords;
out float passLighting;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main(){
    gl_Position = proj * view * model * vec4(vPos, 1.0);

    passTexCoords = vTexCoords;
    passLighting = vLighting;
}
