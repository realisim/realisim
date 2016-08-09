/**/

#pragma once
#include "Core.h"
#include "data/Scene.h"
#include "data/Tile.h"
#include <math/Point.h>
#include <map>
#include <QFileInfo>
#include <vector>

namespace engine3d
{
namespace core
{
  class Broker : public Core::Client
  {
  public:
    friend class Hub;
    
    Broker();
    ~Broker();

    void loadDemoScene();
    void parseDirectories();
    
//    int getNumberOfTiles();
//    const Tile& getTile(int) const;//by index
//    Tile getTileWithPayload(int) const;
      data::Tile getTile(double, double);
      //data::Tile getTile(math::Point2d iLatLong);
//    const Tile& getTile(math::Point3d) const;
    //std::vector<data::Tile> getTilesWithinDistanceFrom(Point3d pos, double distanceInMeter) const;
    
    //getNumberOfAirfield()
    //getAirfield(int)
    //getAirfieldsWithinDistanceFrom(Point3d pos, double distanceInMeter);
        
    const data::Scene& getScene() const {return mScene;}
    
  protected:
    void addTile(data::Tile);
    void findResourcesFolderPath();
	QString getResourcesFolderPath() const;
    void parseAirfieldDirectories();
    void parseModelDirectories();
    bool parseTerrainDirectory(int iLat, int iLong);
	virtual void update(double) override; //from Core:Client

    //cache pour le tiles
    //cache pour les airfield
    //thread pour le chargemnet du disque.
    
    QString mResourcesFolderPath;
    data::Scene mScene;
        
	//std::vector<data::Tile> mTileFiles;
	//std::map<int, data::Tile> mTileIndexToTile; //humm... semble superflu...
    std::map<realisim::math::Point2i, int> m5DegTilesCache;
    std::map<realisim::math::Point2d, data::Tile> mPositionToTile;

  };
  
}
}
