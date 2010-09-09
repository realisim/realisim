uniform sampler3D Noise;
varying vec3 MCposition;
vec3 color1 = vec3(0.8, 0.7, 0.0);
vec3 color2 = vec3(0.6, 0.1, 0.0);
float noiseScale = 1.2;

varying vec4 diffuse,ambient;
varying vec3 normal,lightDir,halfVector;

float getIntensity(vec3 pos)
{
  vec4 noiseVec = texture3D(Noise, pos * noiseScale);
  float intensity = (noiseVec.r - 0.25 + 
                     noiseVec.g - 0.125 +
                     noiseVec.b - 0.0625 +
                     noiseVec.a - 0.03125);
  return intensity;
}

vec4 bumpMapping()
{
  vec3 n,halfV;
  float NdotL,NdotHV;
  
  /* The ambient term will always be present */
  vec4 color = ambient;
  
  /* a fragment shader can't write a varying variable, hence we need
  a new variable to store the normalized interpolated normal */
  float epsilon = 0.5;
  float f0 = getIntensity(MCposition);
  float fx = getIntensity(MCposition + vec3(epsilon, 0.0, 0.0));
  float fy = getIntensity(MCposition + vec3(0.0, epsilon, 0.0));
  float fz = getIntensity(MCposition + vec3(0.0, 0.0, epsilon));
  vec3 df = vec3(fx - f0 / epsilon,
    fy - f0 / epsilon,
    fz - f0 / epsilon);
  n = normalize(normal - df);
  
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

  return color;// * vec4(myInput, 1.0);

}

void main()
{
  
  float intensity = getIntensity(MCposition);                     
  intensity = clamp(intensity * 6.0, 0.0, 1.0);
  vec3 color = mix(color1, color2, intensity);
  gl_FragColor = vec4(color, 1.0) * bumpMapping(); 
}