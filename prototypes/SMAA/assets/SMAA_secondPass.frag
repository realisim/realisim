#version 440 compatibility

uniform sampler2D uEdgeTex;
uniform sampler2D uAreaTex;
uniform sampler2D uSearchTex;

// Interpolated values from the vertex shaders
in vec4 color;
in vec2 UV0;
in vec2 pixCoord;
in vec4 offset[3];

out vec4 frag_color;

//--- Forward declaration for SMAA.hlsl
vec4 SMAABlendingWeightCalculationPS(vec2 texcoord,
                                     vec2 pixcoord,
                                     vec4 offset[3],
                                     sampler2D edgesTex,
                                     sampler2D areaTex,
                                     sampler2D searchTex,
                                     vec4 subsampleIndices);

//second pass
vec4 blendWeightCalculation()
{
  vec4 subSampleIndices = vec4(0.0);
  vec4 c = SMAABlendingWeightCalculationPS( UV0,
        pixCoord,
        offset,
        uEdgeTex,
        uAreaTex,
        uSearchTex,
        subSampleIndices );

  //c = vec4(1.0, 0.0, 0.0, 1.0);
  return c;
}

void main()
{
    vec4 c = vec4(0.0, 0.0, 0.0, 0.0);

    c = blendWeightCalculation();
    //c = texture(uEdgeTex, UV0);
    //c = texture(uAreaTex, UV0);
    //c = texture(uSearchTex, UV0);

    
    //frag_color = vec4( 0.0, 1.0, 0.0, 1.0 );
    frag_color = c;
}