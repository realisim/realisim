#version 120
uniform sampler3D texture;
const int maxKernelSize = 27;
uniform vec3 offset[maxKernelSize];
uniform int kernelSize; //size of kernel (width * height) for this execution
uniform float kernelValue[maxKernelSize];
uniform float scaleFactor;
uniform bool addOnBase;
uniform vec3 volumeSize;

vec4 applyWindowing(in sampler3D iTex, in vec3 iPos);

void main()
{
  int i;
  vec4 sum = vec4(0.0);
  
  for(i=0; i < kernelSize; i++)
  {
    vec4 tmp = applyWindowing(texture, gl_TexCoord[0].xyz + offset[i] / volumeSize);
    //vec4 tmp = texture3D(texture, gl_TexCoord[0].xyz + offset[i] / volumeSize);
    sum += tmp * kernelValue[i];
  }
  
  vec4 baseColor = vec4(0.0);
  
  if(addOnBase)
    baseColor = texture3D(texture, gl_TexCoord[0].xyz);
  gl_FragColor = sum * scaleFactor + baseColor;
}