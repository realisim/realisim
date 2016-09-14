#version 410

// Output position of the vertex, in clip space : MVP * position
vec4 applyGamma(in vec4 c)
{ 
    float gamma = 2.08;
    return vec4(pow(c.rgb, vec3(1.0/gamma)), c.a);
}