/**/

#pragma once

#include <data/Mesh.h>
#include <vector>

namespace engine3d
{
namespace data
{
  struct Model
  {
    Model();
    ~Model();
    
    Mesh mMesh;
//    Material mMaterial;
//  Shader mShader;
  };
  
}
}
