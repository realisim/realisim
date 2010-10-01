#version 120
uniform sampler3D texture;
uniform float base;
uniform float treshold;

void main()
{
  vec4 color = texture3D(texture, gl_TexCoord[0].xyz);
  float baseColor = color.r;
  if(base - treshold <= baseColor && baseColor <= base + treshold)
    color = vec4(1.0, 1.0, 0.0, 0.15) + vec4(vec3(color), 0.5);
  gl_FragColor = color;
}