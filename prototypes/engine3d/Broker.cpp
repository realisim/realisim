
#include "Broker.h"
#include <math/Point.h>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <utils/Timer.h>

using namespace realisim;
using namespace engine3d;
  using namespace core;

//-------------------------------------------------------------------
Broker::Broker() : Core::Client()
{
  findResourcesFolderPath();
}

//-------------------------------------------------------------------
Broker::~Broker()
{}

//-------------------------------------------------------------------
void Broker::addTile(data::Tile iT)
{
    mPositionToTile[ math::Point2d(iT.getLatitude(), iT.getLongitude()) ] = iT;
}
//-------------------------------------------------------------------
void Broker::findResourcesFolderPath()
{
  QDir appDirPath(QCoreApplication::applicationDirPath());
  bool found = false;
  while( !found && appDirPath.cdUp()  )
  {
    QStringList childs = appDirPath.entryList(QDir::Dirs);
    found = childs.indexOf("resources") != -1;
  }
  mResourcesFolderPath = appDirPath.absolutePath() + "/resources";
  
  getLog().log("Resources folder path found at: %s", mResourcesFolderPath.toStdString().c_str());
}

//-------------------------------------------------------------------
//return the resources folder, where the resources for the app are
//located. the path is the form some/folder/bin
//There is no trailing "/"
QString Broker::getResourcesFolderPath() const
{
  return mResourcesFolderPath;
}

//-------------------------------------------------------------------
void Broker::parseDirectories()
{
    parseAirfieldDirectories();
    parseTerrainDirectories();
    parseModelDirectories();
}

//-------------------------------------------------------------------
data::Tile Broker::getTile(double iLat, double iLong) const
{
    data::Tile r;

    auto it = mPositionToTile.upper_bound( math::Point2d(iLat, iLong) );
    if( it != mPositionToTile.end() )
    {
        data::Tile t = it->second;
        printf("asked %f, %f -> got %f, %f\n", iLat, iLong, t.getLatitude(), t.getLongitude());

        math::Point2d p1(iLat, iLong);
        math::Point2d p2(t.getLatitude(), t.getLongitude());

        23434564567
    }
    else
    {
        //tile is not present, we need to parse the folder where
        //that tile could be....
    }
    return r;
}

//-------------------------------------------------------------------
void Broker::parseAirfieldDirectories()
{
  
}

//-------------------------------------------------------------------
void Broker::parseModelDirectories()
{
  const QDir modelsDirPath( getResourcesFolderPath() + "/models" );
  
  QFileInfoList modelEntries = modelsDirPath.entryInfoList(QDir::Files);
  for(int i = 0; i < modelEntries.size(); ++i)
  {
    //printf("\t%s\n", modelEntries.at(i).absoluteFilePath().toStdString().c_str() );
  }
}

//-------------------------------------------------------------------
void Broker::parseTerrainDirectories()
{
	utils::Timer _timer;

    const QDir terrainDir( "F:\\aXion_world2\\HeightMaps\\" );

    QStringList latfilters;
    latfilters.push_back("lat*");

    QStringList longfilters;
    longfilters.push_back("long*");

	int tileIndex = 0;
    //latitudes
    QFileInfoList latEntries = terrainDir.entryInfoList(latfilters, QDir::Dirs);
    for (int i = 0; i < latEntries.size(); ++i)
    {
        //printf("%s\n", latEntries.at(i).absoluteFilePath().toStdString().c_str());

        //longitudes
        QDir latDir = latEntries.at(i).absoluteFilePath();
        QFileInfoList longEntries = latDir.entryInfoList(longfilters, QDir::Dirs);
        for (int j = 0; j < longEntries.size(); ++j)
        {
            //printf("\t%s\n", longEntries.at(j).absoluteFilePath().toStdString().c_str());

            //tile content - the images
            QDir longDir = longEntries.at(j).absoluteFilePath();
            QFileInfoList tileContent = longDir.entryInfoList(QDir::Files);
            for (int k = 0; k < tileContent.size(); ++k)
            {
				QString f = tileContent.at(k).absoluteFilePath();
				//printf("\t\t%s\n", tileContent.at(k).absoluteFilePath().toStdString().c_str());

				//find lat
				//group0: captured string: LatXX[NS]-LONGXX[EW]
				//group1: Latitude
				//group2: [NS]
				//group3: Longitude
				//group4: [EW]
				QRegExp latLongRe("Lat([0-9]+\\.[0-9]+)([NS])-Long([0-9]+\\.[0-9]+)([EW])");
				latLongRe.indexIn(f);
				QStringList captured = latLongRe.capturedTexts();
				if (captured.size() == 5)
				{
					double latitude = captured[1].toDouble();
					latitude *= captured[2] == "N" ? 1 : -1;
					double longitude = captured[3].toDouble();
					longitude *= captured[4] == "E" ? 1 : -1;

					//check if tile already exists. if so, grab that tile
					//if not, create the tile and insert it in data structure
					const math::Point2d latLong(latitude, longitude);
					auto itTile = mPositionToTile.find(latLong);
					data::Tile t;
					if (itTile != mPositionToTile.end())
					{
						t = itTile->second;
					}
					else
					{
						t.setLatitude(latitude);
						t.setLongitude(longitude);
                        t.setSize(math::Vector2d(0.11, 0.11));
                        addTile(t);
					}

					//get file extension
					const QString extension = (tileContent.at(k).suffix());
					if (extension == "dds")
					{
						//check low res first since it also contains -text as in the
						//highres file.
						//lowres
						if(f.lastIndexOf("LR-text") != -1)
						{ 
							t.getAlbedoLowResolution().setFilePath(f);
							t.getAlbedoLowResolution().setType(data::Image::tDds);
						}
						//highres
						else if(f.lastIndexOf("-text") != -1)
						{
							t.getAlbedoHighResolution().setFilePath(f);
							t.getAlbedoHighResolution().setType(data::Image::tDds);
						}
						//normal
						else if (f.lastIndexOf("-norm") != -1)
						{
							//t.getNormalMap().setFilePath(f);
							//t.getNormalMap().setType(Image::tDds);
						}
						else{}
					}
					else if (extension == "HM")
					{
						//t.getHeightMap().setFilePath(f);
						//t.getHeightMap().setType(Image::tHeightMap);
					}
					else if (extension == "LP")
					{
						//t.getLightPointMap().setFilePath(f);
						//t.getLightPointMap().setType(Image::tLightMap);
					}
					else
					{
						getLog().log("Unknown file extension while parsing terrain file %s\n", 
							f.toStdString().c_str());
					}
				}
				else
				{
					getLog().log("Tile file %s was found but does not match naming convention.",
						f.toStdString().c_str());
				}
            }
        }
    }

    for(auto it = mPositionToTile.begin(); it != mPositionToTile.end(); ++it)
        printf("lat %f, long %f\n", it->second.getLatitude(), it->second.getLongitude());

	printf("Time to parse terrain directory: %f(sec)\n", _timer.getElapsed());
	printf("number of tile files parsed %d\n", (int)mPositionToTile.size());
}

//-------------------------------------------------------------------
void Broker::update(double iSecElapsed)
{
	//decrease ref count on all tiles

	//cleanup tile with refcount 0
}