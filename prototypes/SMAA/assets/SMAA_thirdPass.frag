#version 410

uniform sampler2D uColorTex;
uniform sampler2D uBlendTex;
uniform float uBlendFactor;

// Interpolated values from the vertex shaders
in vec4 color;
in vec2 UV0;
in vec4 offset;

out vec4 frag_color;

//--- Forward declaration for SMAA.hlsl
vec4 SMAANeighborhoodBlendingPS( vec2 texcoord,
                                  vec4 offset,
                                  sampler2D colorTex,
                                  sampler2D blendTex
                                  #if SMAA_REPROJECTION
                                  , sampler2D velocityTex
                                  #endif
                                  );

//third pass
vec4 neighborhoodBlending()
{
  vec4 c = SMAANeighborhoodBlendingPS( UV0,
        offset,
        uColorTex,
        uBlendTex );

  //c = vec4(1.0, 0.0, 0.0, 1.0);
  return c;
}

void main()
{
    vec4 c = vec4(0.0, 0.0, 0.0, 0.0);

    c = neighborhoodBlending();
    //c = texture(uColorTex, UV0);
    //c = texture(uBlendTex, UV0);

    
    //frag_color = vec4( 0.0, 1.0, 0.0, 1.0 );
    frag_color = c;
    frag_color.a *= uBlendFactor;
}