#version 440 compatibility

uniform sampler2D uOffScreen;

vec2 SMAALumaEdgeDetectionPS(vec2 texcoord,
                               vec4 offset[3],
                               sampler2D colorTex
                               #if SMAA_PREDICATION
                               , sampler2D predicationTex
                               #endif
                               );

void main()
{
    vec2 tc = gl_TexCoord[0].xy;
    ivec2 texSize = textureSize(uOffScreen, 0);

    vec4 offset[3];
    offset[0] = vec4(tc.x - 0.5/texSize.x, tc.y, tc.x,  tc.y + 0.5/texSize.y);
    offset[1] = vec4(tc.x + 0.5/texSize.x, tc.y, tc.x,  tc.y - 0.5/texSize.y);
    offset[2] = vec4(tc.x - 1.5/texSize.x, tc.y, tc.x,  tc.y + 1.5/texSize.y);

    vec2 edge = SMAALumaEdgeDetectionPS( gl_TexCoord[0].xy, 
        offset,
        uOffScreen );

    gl_FragColor = vec4( edge, 0.0, 1.0 );

    //vec4 c = texture(uOffScreen, gl_TexCoord[0].xy);
    //gl_FragColor = c;
    //gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
}