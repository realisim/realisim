#version 440 compatibility

layout(binding=0) uniform sampler2D uColorTex;
layout(binding=1) uniform sampler2D uAreaTex;
layout(binding=2) uniform sampler2D uSearchTex;
uniform int uFirstPass;
uniform int uSecondPass;
uniform int uThirdPass;

out vec4 color;

//forward declaration for SMAA.hlsl
vec2 SMAALumaEdgeDetectionPS(vec2 texcoord,
                               vec4 offset[3],
                               sampler2D colorTex
                               #if SMAA_PREDICATION
                               , sampler2D predicationTex
                               #endif
                               );

//vec4 SMAABlendingWeightCalculationPS(vec2 texcoord,
//                                       vec2 pixcoord,
//                                       vec4 offset[3],
//                                       sampler2D edgesTex,
//                                       sampler2D areaTex,
//                                       sampler2D searchTex,
//                                       vec4 subsampleIndices);

//first pass
vec2 lumaEdgeDetection()
{
    vec2 tc = gl_TexCoord[0].xy;
    ivec2 texSize = textureSize(uColorTex, 0);

    vec4 offset[3];
    offset[0] = vec4(tc.x - 0.5/texSize.x, tc.y, tc.x,  tc.y + 0.5/texSize.y);
    offset[1] = vec4(tc.x + 0.5/texSize.x, tc.y, tc.x,  tc.y - 0.5/texSize.y);
    offset[2] = vec4(tc.x - 1.5/texSize.x, tc.y, tc.x,  tc.y + 1.5/texSize.y);

    vec2 edge = SMAALumaEdgeDetectionPS( gl_TexCoord[0].xy, 
        offset,
        uColorTex );

    return edge;
}

//second pass
//vec4 blendWeightCalculation()
//{
//    vec2 tc = gl_TexCoord[0].xy;
//    ivec2 texSize = textureSize(uColorTex, 0);
//
////HACK should be in vertex shader
//vec2 SMAA_RT_METRICS = texSize;
//int SMAA_MAX_SEARCH_STEPS = 16;
//
//    vec4 offset[3];
//    offset[0] = fma(SMAA_RT_METRICS.xyxy, vec4(-0.25, -0.125,  1.25, -0.125), tc.xyxy);
//    offset[1] = fma(SMAA_RT_METRICS.xyxy, vec4(-0.125, -0.25, -0.125,  1.25), tc.xyxy);
//
//    // And these for the searches, they indicate the ends of the loops:
//    offset[2] = fma(SMAA_RT_METRICS.xxyy,
//                    vec4(-2.0, 2.0, -2.0, 2.0) * float(SMAA_MAX_SEARCH_STEPS),
//                    vec4(offset[0].xz, offset[1].yw));
//
//    vec2 pixCoord = tc.xy * texSize;
//    vec4 subSampleIndices = vec4(0.0, 0.0, 0.0, 0.0);
//
//    return SMAABlendingWeightCalculationPS( tc.xy,
//        pixCoord,
//        offset,
//        uColorTex,
//        uAreaTex,
//        uSearchTex,
//        subSampleIndices );
//
//    //return texture(uColorTex, gl_TexCoord[0].xy);
//    //return texture(uAreaTex, gl_TexCoord[0].xy);
//    //return vec4(1.0, 0.0, 0.0, 1.0);
//}

void main()
{
    vec4 c = vec4(0.0, 0.0, 0.0, 0.0);

    if(uFirstPass > 0)
    {
        vec2 edge = lumaEdgeDetection();
        c = vec4( edge, 0.0, 1.0 );
    }
    if(uSecondPass > 0)
    {
       // c = blendWeightCalculation();
       //c = texture(uAreaTex, gl_TexCoord[0].xy);
    }
    if(uThirdPass > 0)
    {}

    color = c;

    //vec4 c = texture(uColorTex, gl_TexCoord[0].xy);
    //gl_FragColor = c;
    //gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
}