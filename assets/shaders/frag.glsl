#version 400 core
in vec2 passTexCoords;

out vec4 outColor;
in float passLighting;

uniform sampler2D tex;
uniform bool playerUnderWater;

void main(){
    vec4 texColor = texture(tex, passTexCoords);
    vec3 rgb = vec3(texColor);
    rgb *= passLighting;
    outColor = vec4(rgb, texColor.a);

    if (outColor.a == 0)
    {
        discard;
    }

    if (playerUnderWater)
    {
        outColor *= vec4(0.0, 0.25, 0.75, 1.0);
        outColor += vec4(0.0, 0.1, 0.45, 1.0);
    }
}
