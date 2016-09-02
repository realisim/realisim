#version 410

// Output position of the vertex, in clip space : MVP * position
vec4 applyModelViewProjection(mat4 iModelView, mat4 iProjection, vec3 iP)
{ return iProjection * iModelView * vec4(iP, 1); }