varying vec3 MCposition;
float scale = 1.0;
	
  void directionalLighting();
  
	void main()
	{
    directionalLighting();
    
    MCposition = vec3(gl_Vertex) * scale;
		gl_Position = ftransform();
	}
  