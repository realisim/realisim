#version 410

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec4 vertexColor;
layout(location = 3) in vec2 vertexUV0;

// Output data ; will be interpolated for each fragment.
out vec4 color;
out vec2 UV0;

// Values that stay constant for the whole mesh.
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

void main(){

    // Output position of the vertex, in clip space : MVP * position
    gl_Position =  projectionMatrix * modelViewMatrix * vec4(vertexPosition_modelspace,1);

    color = vertexColor;
    UV0 = vertexUV0;
}