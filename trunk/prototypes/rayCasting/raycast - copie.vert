varying vec3 MCposition;
uniform mat4 WCmatrix;

	void main()
	{
    vec4 vertex = vec4(WCmatrix * gl_Vertex);
    MCposition = vec3(vertex) / vertex.w;
        
    gl_FrontColor = gl_Color;
		gl_Position = ftransform();
	}
  