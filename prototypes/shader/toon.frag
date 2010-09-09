varying vec3 normal;
varying vec3 lightDir;
vec3 defaultColor = vec3(1.0, 0.0, 0.0);

void main()
{
  float intensity;
  vec4 color;
  vec3 n = normalize(normal);
  intensity = dot(lightDir, n);
  
  if (intensity > 0.95)
			color = vec4(defaultColor,1.0) * 1.0;
		else if (intensity > 0.5)
			color = vec4(defaultColor,1.0) * 0.5;
		else if (intensity > 0.25)
			color = vec4(defaultColor,1.0) * 0.25;
		else
			color = vec4(defaultColor,1.0) * 0.05;
  
  gl_FragColor = color;
}