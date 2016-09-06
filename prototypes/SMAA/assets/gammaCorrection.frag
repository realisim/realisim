#version 440

uniform sampler2D uOffScreen;

// Interpolated values from the vertex shaders
in vec4 color;
in vec2 UV0;

// Ouput data
out vec4 frag_color;

void main()
{
    // apply gamma correction
    float gamma = 2.1;
    vec4 c = texture(uOffScreen, UV0);
    frag_color = vec4(pow(c.rgb, vec3(1.0/gamma)), c.a);
}