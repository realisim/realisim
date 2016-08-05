/**/

#pragma once
#include "Image.h"

namespace engine3d
{
namespace data
{
  class Tile
  {
  public:
    Tile();
	Tile(const Tile&);
	Tile& operator=(const Tile&);
    ~Tile();
    
	Image getAlbedoHighResolution();
	Image getAlbedoLowResolution();
	//Image getHeightMap();
	//Image getNormalMap();
	//Image getLightPointMap();
	void setLatitude(double);
	void setLongitude(double);

  private:
	  struct Guts
	  {
		  Guts();

		  int mRefCount;
		  double mLatitude;
		  double mLongitude;
		  Image mAlbedoHighResolution; //image doit avoir du partage implicite...
		  Image mAlbedoLowResolution; //image doit avoir du partage implicite...
		  Image mHeightMap;
		  Image mNormalMap;
		  Image mLightPoint;
		  //Mesh mMesh;
	  };

	  void makeGuts();
	  void shareGuts(Guts*);
	  void deleteGuts();

	  Guts* mpGuts;
  };
  
}
}
