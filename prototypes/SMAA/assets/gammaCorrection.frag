#version 410

uniform sampler2D uColorTex;

// Interpolated values from the vertex shaders
in vec4 color;
in vec2 UV0;

// Ouput data
out vec4 frag_color;

// forward declare from utilities.frag
vec4 applyGamma(in vec4 c);

void main()
{
    vec4 c = texture(uColorTex, UV0);
    frag_color = applyGamma(c);
}