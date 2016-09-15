#version 410


//-----------------------------------------------------------------------------
// Output position of the vertex, in clip space : MVP * position
vec4 applyModelViewProjection(mat4 iModelView, mat4 iProjection, vec3 iP)
{ return iProjection * iModelView * vec4(iP, 1); }

//-----------------------------------------------------------------------------
void computePhong(mat4 iModelView, mat4 iNormal, 
    vec3 iPosition_modelSpace, vec3 iNormal_modelSpace,
    out vec3 oViewDirection, out vec3 oNormal )
{
    oNormal = normalize( iNormal * vec4(iNormal_modelSpace, 1.0) ).xyz;
    oViewDirection = -(iModelView * vec4(iPosition_modelSpace, 1.0)).xyz;
}