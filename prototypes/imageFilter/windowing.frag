#version 120
uniform float windowingMin;
uniform float windowingMax;

vec4 applyWindowing(in sampler3D iTex, in vec3 iPos)
{
  vec4 color = texture3D(iTex, iPos);
  vec3 normalizedColor = (color.rgb - vec3(windowingMin)) / (windowingMax - windowingMin);
  normalizedColor = clamp(normalizedColor, vec3(0.0), vec3(1.0) );
  return vec4(normalizedColor, 1.0);
 
 //return texture3D(iTex, iPos);
}