#version 410

uniform sampler2D uColorTex;
uniform sampler2D uPreviousColorTex;

// Interpolated values from the vertex shaders
in vec4 color;
in vec2 UV0;

out vec4 frag_color;

//--- Forward declaration for SMAA.hlsl
vec4 SMAAResolvePS(vec2 texcoord,
                     sampler2D currentColorTex,
                     sampler2D previousColorTex
                     #if SMAA_REPROJECTION
                     , sampler2D velocityTex
                     #endif
                     );

void main()
{
    vec4 c = vec4(0.0, 0.0, 0.0, 0.0);

    c = SMAAResolvePS(UV0, uColorTex, uPreviousColorTex);
    //c = texture(uColorTex, UV0);
    //c = texture(uPreviousColorTex, UV0);

    frag_color = c;
}