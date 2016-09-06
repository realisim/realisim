#version 440 compatibility

uniform sampler2D uColorTex;

// Interpolated values from the vertex shaders
in vec4 color;
in vec2 UV0;

out vec4 frag_color;

//forward declaration for SMAA.hlsl
vec2 SMAALumaEdgeDetectionPS(vec2 texcoord,
                               vec4 offset[3],
                               sampler2D colorTex
                               #if SMAA_PREDICATION
                               , sampler2D predicationTex
                               #endif
                               );

//first pass
vec2 lumaEdgeDetection()
{
    vec2 tc = UV0;
    ivec2 texSize = textureSize(uColorTex, 0);

    vec4 offset[3];
    offset[0] = vec4(tc.x - 0.5/texSize.x, tc.y, tc.x,  tc.y + 0.5/texSize.y);
    offset[1] = vec4(tc.x + 0.5/texSize.x, tc.y, tc.x,  tc.y - 0.5/texSize.y);
    offset[2] = vec4(tc.x - 1.5/texSize.x, tc.y, tc.x,  tc.y + 1.5/texSize.y);

    vec2 edge = SMAALumaEdgeDetectionPS( tc, 
        offset,
        uColorTex );

    return edge;
}

void main()
{
    vec4 c = vec4(0.0, 0.0, 0.0, 0.0);

    vec2 edge = lumaEdgeDetection();
    c = vec4( edge, 0.0, 1.0 );

    //c = texture(uColorTex, UV0);
    //frag_color = vec4( 0.0, 1.0, 0.0, 1.0 );
    frag_color = c;
}