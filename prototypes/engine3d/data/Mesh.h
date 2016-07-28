/**/

#pragma once

#include <math/Point.h>
#include <math/Vect.h>
#include <QString>
#include <vector>

namespace engine3d
{
namespace data
{
  struct Mesh
  {
    Mesh();
    ~Mesh();
    
    //void makeFromObj(QString);
    
    struct face
    {
      std::vector<unsigned int> mVertexIndices;
      std::vector<unsigned int> mTexture2dIndices;
      //std::vector<unsigned int> mTexture3dIndices;
      std::vector<unsigned int> mNormalIndices;
    };
    
    std::vector<realisim::math::Point3d> mVertices;
    std::vector<realisim::math::Vector2d> mTexture2dCoordinates;
    //std::vector<realisim::math::Vector3d> mTexture3dCoordinates;
    std::vector<realisim::math::Vector3d> mNormals;
    
    std::vector<face> mFaces;
  };
  
}
}
