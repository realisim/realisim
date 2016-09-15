#version 410

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec4 vertexColor;
layout(location = 3) in vec2 vertexUV0;

// Output data ; will be interpolated for each fragment.
out vec4 color;
out vec2 UV0;

out vec3 lightDir_eyeSpace;
out vec3 oNormal_eyeSpace;
out vec3 oViewDirection_eyeSpace;

// Values that stay constant for the whole mesh.
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

//defined in utilities.vert
vec4 applyModelViewProjection(mat4 iModelView, mat4 iProjection, vec3 iP);

void computePhong(mat4 iModelView, mat4 iNormal, 
    vec3 iPosition_modelSpace, vec3 iNormal_modelSpace,
    out vec3 oViewDirection, out vec3 oNormal );

void main(){

    // Output position of the vertex, in clip space : MVP * position
    gl_Position =  applyModelViewProjection(modelViewMatrix, projectionMatrix, vertexPosition_modelspace);

    color = vertexColor;
    UV0 = vertexUV0;
    
    computePhong(modelViewMatrix, normalMatrix, 
        vertexPosition_modelspace, vertexNormal_modelspace,
        oViewDirection_eyeSpace, oNormal_eyeSpace);
    lightDir_eyeSpace = normalize(modelViewMatrix * vec4(0.7, 0.4, 0.2, 0) ).xyz;
}