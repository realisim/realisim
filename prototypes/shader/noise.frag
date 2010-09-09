uniform sampler3D Noise;
varying vec3 MCposition;
vec3 skyColor = vec3(0.0, 0.0, 1.0);
vec3 cloudColor = vec3(1.0, 1.0, 1.0);

vec4 directionalLighting();

void main()
{
  vec4 noiseVec = texture3D(Noise, MCposition);
  float intensity = (noiseVec.r + noiseVec.g + noiseVec.b +
    noiseVec.a + 0.03125) * 1.5;
  vec3 color = mix(skyColor, cloudColor, intensity);
  gl_FragColor = vec4(color, 1.0) * directionalLighting(); 
}