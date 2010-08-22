varying vec3 normal;
varying vec3 lightDir;
	
	void main()
	{
		normal = normalize(gl_NormalMatrix * gl_Normal);
    lightDir = normalize(vec3(gl_LightSource[0].position));
	
		gl_Position = ftransform();
	}
  