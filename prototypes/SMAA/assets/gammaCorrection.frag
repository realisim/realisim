#version 440 compatibility

uniform sampler2D uOffScreen;

void main()
{
    // apply gamma correction
    float gamma = 2.1;
    vec4 c = texture(uOffScreen, gl_TexCoord[0].xy);
    gl_FragColor = vec4(pow(c.rgb, vec3(1.0/gamma)), c.a);
}