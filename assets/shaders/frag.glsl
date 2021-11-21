#version 330 core
in vec2 passTexCoords;

out vec4 outColor;
in float passLighting;

uniform sampler2D tex;

void main(){
    vec4 texColor = texture(tex, passTexCoords);
    vec3 rgb = vec3(texColor);
    rgb *= passLighting;
    outColor = vec4(rgb, texColor.a);

    if (outColor.a == 0)
    {
        discard;
    }
}
