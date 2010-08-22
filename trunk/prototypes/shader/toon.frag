varying vec3 normal;
varying vec3 lightDir;
uniform vec3 myInput;

void main()
{
  float intensity;
  vec4 color;
  vec3 n = normalize(normal);
  intensity = dot(lightDir, n);
  
  if (intensity > 0.95)
			color = vec4(myInput,1.0) * 1.0;
		else if (intensity > 0.5)
			color = vec4(myInput,1.0) * 0.5;
		else if (intensity > 0.25)
			color = vec4(myInput,1.0) * 0.25;
		else
			color = vec4(myInput,1.0) * 0.05;
  
  gl_FragColor = color;
}