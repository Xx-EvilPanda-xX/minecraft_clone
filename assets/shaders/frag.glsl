#version 330 core
in vec2 passTexCoords;

out vec4 outColor;

uniform float green;

void main(){
    outColor = vec4(0.0, green, 0.0, 1.0);
}
