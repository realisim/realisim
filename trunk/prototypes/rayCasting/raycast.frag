uniform sampler3D texture;
uniform sampler2D hounsfieldLUT;
varying vec3 MCposition;
uniform float stepSize;
uniform float isoSurfaceValue;
uniform vec3 CameraPos;
uniform vec3 volumeSpacing;
varying vec4 diffuse,ambient;
varying vec3 lightDir,halfVector;

float stepSizeToUse = clamp(stepSize, 0.01, 1.0);
vec3 rayDir = normalize(MCposition - CameraPos);

float getTextureValue(vec3 iPos)
{
  return texture3D(texture, iPos).r;
}

vec3 getTissueColor(float iDensity)
{
  return texture2D(hounsfieldLUT, vec2(iDensity, 0.5)).rgb;
}

vec4 shading(vec3 iPos)
{
  vec3 n,halfV;
  float NdotL,NdotHV;
  
  /* The ambient term will always be present */
  vec4 color = ambient;
  
  /* calcul du gradient au point iPos */
  float epsilon = 0.01;
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

float hitPointRefinement(in float iDensity, inout vec3 iPos, in float iDivisor)
{
  if(iDensity > isoSurfaceValue)
    iPos = iPos - (stepSizeToUse * rayDir) / iDivisor;
  else
    iPos = iPos + (stepSizeToUse * rayDir) / iDivisor;
  return getTextureValue(iPos);
}

void main()
{
  vec3 tissueColor = vec3(0.6);
  vec4 shadeFactor = vec4(1.0);

  float density = 0.0;
  vec3 hprPos;

  vec3 pos = vec3(gl_Color);
  while(all(greaterThanEqual(pos, vec3(0.0))) && all(lessThanEqual(pos, vec3(1.0))))
  {
    pos = pos + (stepSizeToUse * rayDir);
    density = getTextureValue(pos);
    
    if(density > isoSurfaceValue)
    {
      density = hitPointRefinement(density, pos, 2.0);
      density = hitPointRefinement(density, pos, 4.0);
      density = hitPointRefinement(density, pos, 8.0);
      density = hitPointRefinement(density, pos, 16.0);
      density = hitPointRefinement(density, pos, 32.0);
      density = hitPointRefinement(density, pos, 64.0);
      
      tissueColor = getTissueColor(density);
      shadeFactor = shading(pos);
      pos = vec3(1.2); //exit
    }
  }
    
  gl_FragColor = vec4(tissueColor, 1.0) * shadeFactor;
}