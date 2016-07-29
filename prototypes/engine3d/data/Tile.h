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

    //Image mAlbedo; //image doit avoir du partage implicite...
    //Image mHeightMap;
    //Image mNormalMap;
    //Image mLightPoint;
    //Mesh mMesh;
  };
  
}
}
