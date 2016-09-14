#version 410

uniform sampler2DMS uColorTexMS;

// Interpolated values from the vertex shaders
in vec4 position;
in vec4 color;
in vec2 UV0;

out vec4 frag_color0;
out vec4 frag_color1;

//--- Forward declaration for SMAA.hlsl
void SMAASeparatePS(vec4 position,
                    vec2 texcoord,
                    out vec4 target0,
                    out vec4 target1,
                    sampler2DMS colorTexMS);

void main()
{
    //SMAASeparatePS(position, UV0, frag_color0, frag_color1, uColorTexMS);

    //frag_color0 = texelFetch(uColorTexMS, ivec2(UV0), 0);
    //frag_color0 = texelFetch(uColorTexMS, ivec2(UV0*ivec2(1017,716)), 0);
    //frag_color1 = texelFetch(uColorTexMS, ivec2(UV0*ivec2(1017,716)), 1);

    frag_color0 = texelFetch(uColorTexMS, ivec2(gl_FragCoord.xy), 0);
    frag_color1 = texelFetch(uColorTexMS, ivec2(gl_FragCoord.xy), 1);

    //frag_color0 = vec4(1.0, 0.0, 0.0, 1.0);
    //frag_color1 = vec4(0.0, 1.0, 0.0, 1.0);
}