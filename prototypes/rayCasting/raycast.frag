uniform sampler3D texture;
varying vec3 MCposition;
uniform float stepSize;
uniform float colorThreshold;
uniform vec3 CameraPos;
varying vec4 diffuse,ambient;
varying vec3 lightDir,halfVector;

float getTextureValue(vec3 iPos)
{
  return texture3D(texture, iPos).r;
}

vec4 shading(vec3 iPos)
{
  vec3 n,halfV;
  float NdotL,NdotHV;
  
  /* The ambient term will always be present */
  vec4 color = ambient;
  
  /* calcul du gradient au point iPos */
  float epsilon = 0.03;
  float fx1 = getTextureValue(iPos + vec3(-epsilon, 0.0, 0.0));
  float fy1 = getTextureValue(iPos + vec3(0.0, -epsilon, 0.0));
  float fz1 = getTextureValue(iPos + vec3(0.0, 0.0, -epsilon));
  
  float fx2 = getTextureValue(iPos + vec3(epsilon, 0.0, 0.0));
  float fy2 = getTextureValue(iPos + vec3(0.0, epsilon, 0.0));
  float fz2 = getTextureValue(iPos + vec3(0.0, 0.0, epsilon));
  vec3 df = vec3((fx2 - fx1) / 2.0,
    (fy2 - fx1) / 2.0,
    (fz2 - fx1) / 2.0);
  n = normalize(df);
  
  /* compute the dot product between normal and ldir */
  NdotL = max(dot(n,lightDir),0.0);

  if (NdotL > 0.0) 
  {
    color += diffuse * NdotL;
    halfV = normalize(halfVector);
    NdotHV = max(dot(n,halfV),0.0);
    color += gl_FrontMaterial.specular * 
        gl_LightSource[0].specular * 
        pow(NdotHV, gl_FrontMaterial.shininess);
  }

  return color;

}

void main()
{
  float stepSizeToUse = clamp(stepSize, 0.01, 1.0);
  vec3 rayDir = normalize(MCposition - CameraPos);

  float density = 0.0;
  vec3 hprPos;

  vec3 pos = vec3(gl_Color);
  while(all(greaterThanEqual(pos, vec3(0.0))) && all(lessThanEqual(pos, vec3(1.0))))
  {
    density = getTextureValue(pos);
    
    if(density > colorThreshold)
    {
      //hprPos = pos - (stepSizeToUse * rayDir) / 2.0;
      //density = getTextureValue(hprPos);
      break;
    }
      
    pos = pos + (stepSizeToUse * rayDir);
  }
    
  gl_FragColor = vec4(vec3(density), 1.0) * shading(pos);
}