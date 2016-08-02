#version 120

uniform sampler2D uTexture;
uniform float uCutoff;

void main()
{
    vec4 c = texture2D(uTexture, gl_TexCoord[0].xy);
    if( (c.x * c.x + c.y * c.y + c.z * c.z) < uCutoff * uCutoff )
    { discard; }
    gl_FragColor = c;
}