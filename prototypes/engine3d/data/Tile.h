/**/

#pragma once
#include "Image.h"
#include <math/Vect.h>

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
	double getLatitude() const;
	//Image getLightPointMap();
	double getLongitude() const;	
	//Image getNormalMap();
	realisim::math::Vector2d getSize() const;
	void setLatitude(double);
	void setLongitude(double);
    void setSize(realisim::math::Vector2d);

  private:
	  struct Guts
	  {
		  Guts();

		  int mRefCount;
		  double mLatitude;
		  double mLongitude;
          realisim::math::Vector2d mSize;
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
