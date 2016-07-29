/**/

#pragma once

#include "data/Scene.h"
#include "data/Tile.h"
#include <math/Point.h>

namespace engine3d
{
namespace core
{
  class Broker
  {
  public:
    friend class Hub;
    
    Broker();
    ~Broker();

    void loadDemoScene();
    void parseDirectories();
    
//    int getNumberOfTiles();
//    const Tile& getTile(int) const;//by index
//    const Tile& getTile(int latitude, int longitude) const;
//    const Tile& getTile(math::Point3d) const;
//    std::vector<int> getTilesWithinDistanceFrom(Point3d pos, double distanceInMeter) const;
    
    //getNumberOfAirfield()
    //getAirfield(int)
    //getAirfieldsWithinDistanceFrom(Point3d pos, double distanceInMeter);
    
    const data::Scene& getScene() const {return mScene;}
    
  protected:
    void parseAirfieldDirectories();
    void parseModelDirectories();
    void parseTerrainDirectories();

    //cache pour le tiles
    //cache pour les airfield
    //thread pour le chargemnet du disque.
    
    data::Scene mScene;
    std::map<realisim::math::Point2d, data::Tile> mTiles;
  };
  
}
}
