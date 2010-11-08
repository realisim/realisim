#version 120
uniform sampler2D texture;
uniform bool isBlurVertical;
uniform int kernel;

vec4 blur2D(in sampler2D, in bool, in int); //prototype

void main()
{
  gl_FragColor = blur2D(texture, isBlurVertical, kernel); 
}