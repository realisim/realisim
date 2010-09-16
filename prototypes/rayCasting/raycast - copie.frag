uniform sampler3D texture;
varying vec3 MCposition;
uniform float stepSize;
uniform float colorThreshold;
uniform vec3 CameraPos;


vec4 getTextureValue(vec3 iPos)
{
  return texture3D(texture, iPos);
}


void main()
{
  float stepSizeToUse = clamp(stepSize, 0.01, 1.0);

  float alphaAcc = 0.0;
  vec4 color = vec4(0.0);
  vec4 colorSample = vec4(0.0);
  float alphaSample = 0.0;
  vec3 rayDir = normalize(MCposition - CameraPos);


   vec3 minValue = vec3(colorThreshold);

  vec3 pos = vec3(gl_Color);
  while(all(greaterThanEqual(pos, vec3(0.0))) && all(lessThanEqual(pos, vec3(1.0))))
  {
  	colorSample = getTextureValue(pos);
    if (all(greaterThanEqual( colorSample.rgb, minValue)))
    {
      alphaSample = stepSizeToUse;
      color.rgb += colorSample.rgb * alphaSample * 4.0;
      color.a += stepSizeToUse;
      alphaAcc += alphaSample;
    }
    
    pos = pos + (stepSizeToUse * normalize(rayDir));
    if(alphaAcc >= 0.8)
      break;
  }
  



//  vec3 pos = vec3(gl_Color);
//  while(all(greaterThanEqual(pos, vec3(0.0))) && all(lessThanEqual(pos, vec3(1.0))))
//  {
//  	color = getTextureValue(pos);
//    pos = pos + (stepSizeToUse * rayDir);
//    if(any(greaterThanEqual(color, vec4(0.0))))
//      break;
//  }

  gl_FragColor = color;
}