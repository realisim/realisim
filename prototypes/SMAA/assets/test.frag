#version 440 compatibility

layout(binding=0) uniform sampler2D uSampler0;
layout(binding=1) uniform sampler2D uSampler1;
layout(binding=2) uniform sampler2D uSampler2;

void main()
{
    gl_FragColor = texture(uSampler1, gl_TexCoord[0].xy);
    //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}