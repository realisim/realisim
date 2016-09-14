#version 410

uniform sampler2D uColorTex;

// Interpolated values from the vertex shaders
in vec4 color;
in vec2 UV0;
in vec4 offset[3];

out vec4 frag_color;

//forward declaration for SMAA.hlsl
vec2 SMAALumaEdgeDetectionPS(vec2 texcoord,
                               vec4 offset[3],
                               sampler2D colorTex
                               #if SMAA_PREDICATION
                               , sampler2D predicationTex
                               #endif
                               );

void main()
{
    vec4 c = vec4(0.0, 0.0, 0.0, 0.0);

    vec2 edge = SMAALumaEdgeDetectionPS( UV0, offset, uColorTex );
    c = vec4( edge, 0.0, 1.0 );

    //c = texture(uColorTex, UV0);
    frag_color = c;
}