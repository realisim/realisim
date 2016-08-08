
#include "Broker.h"
#include <math/Point.h>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <utils/Timer.h>

using namespace realisim;
using namespace engine3d;
  using namespace core;

namespace
{
    int floorTo5Deg(double iV)
    {
        return iV >= 0 ? (int)(iV/5) * 5 : (int)((iV - 4.9999)/5)*5;
    }
}

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
    //parseTerrainDirectories();
    parseModelDirectories();
}

//-------------------------------------------------------------------
data::Tile Broker::getTile(double iLat, double iLong)
{
    const int latAt5DegResolution = floorTo5Deg(iLat);
    const int longAt5DegResolution = floorTo5Deg(iLong);

    math::Point2i latLongAt5Deg(latAt5DegResolution, longAt5DegResolution);
    auto it5Deg = m5DegTilesCache.find(latLongAt5Deg);
    if(it5Deg == m5DegTilesCache.end())
    {
        if ( parseTerrainDirectory(latAt5DegResolution, longAt5DegResolution) )
        { m5DegTilesCache[latLongAt5Deg] = 1; }
    }
    else //found it, increment count on the 5deg tile
    {
        it5Deg->second = 1;
    }

    data::Tile r;

    //upper_bound is stricly greater, so we find the tile right after
    //the one we are actually looking for in x, hence the -1. Then we search
    //in Y for the right spot.
    auto it = mPositionToTile.upper_bound( math::Point2d(iLat, iLong) );
    --it;
    if( it != mPositionToTile.begin() && it != mPositionToTile.end() )
    {
        data::Tile t1 = it->second;
        const double searchingInLatitude = t1.getLatitude();

        //printf("asked %f, %f -> got from upper_bound %f, %f\n", iLat, iLong, t1.getLatitude(), t1.getLongitude());

        bool found = false;
        int count = 0;
        while(it != mPositionToTile.begin() && t1.getLatitude() == searchingInLatitude && !found)
        {
            t1 = it->second;
            found = t1.getLongitude() <= iLong;
            --it;
            ++count;
        }
        
        if(found){ r = t1; }
        //printf("found %s in %d iterations; tile %f, %f\n", 
        //    found ? "true" : "false",
        //    count,
        //    t1.getLatitude(), t1.getLongitude() );
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
//parse 5 degs terrain directory
bool Broker::parseTerrainDirectory(int iLat, int iLong)
{
	utils::Timer _timer;    
    bool hasParsedDirectory = false;

    const QString latDir = QString("lat") + QString::number(abs(iLat)) + (iLat >= 0 ? "N" : "S");
    const QString longDir = QString("long") + QString::number(abs(iLong)) + (iLong >= 0 ? "E" : "W");

    const QDir directoryToParse( QString("F:\\aXion_world2\\HeightMaps\\")+latDir+"\\"+longDir+"\\" );

    if(directoryToParse.exists())
    {
        //tile content - the images
        //QDir longDir = longEntries.at(j).absoluteFilePath();
        QFileInfoList tileContent = directoryToParse.entryInfoList(QDir::Files);
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
                    if (f.lastIndexOf("LR-text") != -1)
                    {
                        t.getAlbedoLowResolution().setFilePath(f);
                        t.getAlbedoLowResolution().setType(data::Image::tDds);
                    }
                    //highres
                    else if (f.lastIndexOf("-text") != -1)
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
                    else {}
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

        //print all tiles found in directory specified...
        /*for (auto it = mPositionToTile.begin(); it != mPositionToTile.end(); ++it)
            printf("lat %f, long %f\n", it->second.getLatitude(), it->second.getLongitude());*/

        printf("Time to parse terrain directory: %f(sec)\n", _timer.getElapsed());
        printf("number of tile files parsed %d\n", (int)mPositionToTile.size());

        hasParsedDirectory = true;
    }
    else
    {
        getLog().log("Could not parse terrain directory %s\n", directoryToParse.absolutePath().toStdString().c_str());
    }
    

    return hasParsedDirectory;
}

//-------------------------------------------------------------------
void Broker::update(double iSecElapsed)
{
	//decrease ref count on all tiles

	//cleanup tile with refcount 0
}