#version 120

uniform sampler2D uTexture;
uniform float uCutoff;

void main()
{
    vec4 c = texture2D(uTexture, gl_TexCoord[0].xy);
    float luma = 0.299*c.r + 0.587*c.g + 0.114*c.b;
    if( luma < uCutoff )
    { discard; }
    gl_FragColor = c;
}