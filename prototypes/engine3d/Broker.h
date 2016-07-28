/**/

#pragma once

#include "data/Scene.h"

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

    loadDemoScene();
    
//    getNumberOfTiles()
//    getTile(int) const;
//    getTilesWithinDistanceFrom(Point3d pos, double distanceInMeter)
    
    //getNumberOfAirfield()
    //getAirfield(int)
    //getAirfieldsWithinDistanceFrom(Point3d pos, double distanceInMeter);
    
    const data::Scene& getScene() const {return mScene;}
    
  protected:
    
    //cache pour le tiles
    //cache pour les airfield
    //thread pour le chargemnet du disque.
    
    data::Scene mScene;
  };
  
}
}
