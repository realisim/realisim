#version 120
uniform float kernelValues[25];
uniform float scale;

//gaussian blur - normal distribution
vec4 blur2D(in sampler2D iTex, in bool iIsVertical, in int iKernel)
{
  vec2 step = fwidth(gl_TexCoord[0].xy);
	vec4 result = vec4(0.0);
  vec2 offset;
  int j = 0;
  
  for(int i = -iKernel / 2; i <= iKernel / 2; ++i)
  {    
    if(iIsVertical)
      offset = vec2(0.0, step.y * i);
    else
      offset = vec2(step.x * i, 0.0);

    result += texture2D(iTex, gl_TexCoord[0].xy + offset) * kernelValues[j] * scale;
    j++;
  }
  return result;
}

//moyenne separate convolution
//vec4 blur2D(in sampler2D iTex, in bool iIsVertical, in int iKernel)
//{
//  vec2 step = fwidth(gl_TexCoord[0].xy);
//	vec4 result;
//  vec2 offset;
//  
//  for(int i = -iKernel / 2; i <= iKernel / 2; ++i)
//  {    
//    if(iIsVertical)
//      offset = vec2(0.0, step.y * i);
//    else
//      offset = vec2(step.x * i, 0.0);
//
//    result += texture2D(iTex, gl_TexCoord[0].xy + offset);
//  }
//  return result / iKernel;
//}

// mean O^2
//vec4 blur2D(in sampler2D iTex, in bool i, in int iKernel)
//{
//  float step = dFdx(gl_TexCoord[0].x);
//	vec4 result;
//  vec2 offset;
//  float sampleAlpha = texture2D(iTex, gl_TexCoord[0].xy).a;
//  for(int i = -iKernel / 2; i <= iKernel / 2; ++i)
//    for(int j = -iKernel / 2; j <= iKernel / 2; ++j)
//    {
//      offset = vec2(step * i, step * j);
//      result += texture2D(iTex, gl_TexCoord[0].xy + offset);
//    }
//  return result / (iKernel * iKernel);//vec4(result.rgb / (iKernel * iKernel), sampleAlpha);
//}
