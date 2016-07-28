/**/

#pragma once
#include <math/Point.h>

namespace engine3d
{
namespace data
{
  struct Tile
  {
    Tile();
    Tile(int, int);
    ~Tile();
    
    int mLatitude;
    int mLongitude;

    //Image mAlbedo;
    //Image mHeightMap;
    //Image mNormalMap;
    //Image mLightPoint;
    //Mesh mMesh;
  };
  
}
}
