#version 120

uniform sampler2D uTexture0;
uniform sampler2D uTexture1;

void main()
{
    vec4 c0 = texture2D(uTexture0, gl_TexCoord[0].xy);
    vec4 c1 = texture2D(uTexture1, gl_TexCoord[0].xy);

    gl_FragColor = c0+c1;
}