#version 410 

// Interpolated values from the vertex shaders
in vec4 color;
in vec2 UV0;

// Ouput data
out vec4 frag_color;

// Values that stay constant for the whole mesh.
uniform sampler2D texture0;

void main()
{
    // Output color = color of the texture at the specified UV
    frag_color = color;
}