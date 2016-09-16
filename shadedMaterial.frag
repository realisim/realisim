#version 410 

uniform bool uApplyShading;

// Interpolated values from the vertex shaders
in vec4 color;
in vec2 UV0;

in vec3 lightDir_eyeSpace;
in vec3 oNormal_eyeSpace;
in vec3 oViewDirection_eyeSpace;

// Ouput data
out vec4 frag_color;

struct material
{
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};

struct light
{
    vec3 position; //eye_space
    vec4 diffuse;
    vec4 specular;
};

void main()
{
    material mat;
    mat.diffuse = color;
    mat.ambient = color/5.0;
    mat.specular = vec4(1.0, 1.0, 1.0, 1.0);
    mat.shininess = 5;
    
    vec3 light_dir = lightDir_eyeSpace;



    // set the specular term to black
    vec4 spec = vec4(0.0);
 
    // normalize both input vectors
    vec3 n = normalize(oNormal_eyeSpace);
    vec3 e = normalize(oViewDirection_eyeSpace);

    float intensity = max(dot(n,light_dir), 0.0);
 
    // if the vertex is lit compute the specular color
    if (intensity > 0.0) {
        // compute the half vector
        vec3 h = normalize(light_dir + e);
        // compute the specular term into spec
        float intSpec = max(dot(h,n), 0.0);
        spec = mat.specular * pow(intSpec, mat.shininess);
    }
    

    vec4 finalColor = uApplyShading ? max(intensity * mat.diffuse + spec, mat.ambient) : mat.diffuse;
    frag_color = finalColor;
}