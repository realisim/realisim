#version 120

uniform sampler2D uTexture;
uniform ivec2 uTextureSize;
uniform int uHalfKernel;
uniform int uHorizontal;
uniform int uVertical;

void main()
{
    int count = 0;
    vec4 c = vec4(0.0, 0.0, 0.0, 0.0);
    for(int i = -uHalfKernel; i < uHalfKernel; ++i, ++count)
    {
        c += texture2D(uTexture, gl_TexCoord[0].xy + vec2(i*uHorizontal, i*uVertical)/vec2(uTextureSize) );
    }
    c /= float(count);

    gl_FragColor = c;
}