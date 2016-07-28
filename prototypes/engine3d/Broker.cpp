
#include "Broker.h"
#include <QDir>
#include <QFileInfo>

using namespace engine3d;
  using namespace core;

//-------------------------------------------------------------------
Broker::Broker()
{}

//-------------------------------------------------------------------
Broker::~Broker()
{}

//-------------------------------------------------------------------
void Broker::parseDirectories()
{
    parseTerrainDirectories();
    parseModelDirectories();
}

//-------------------------------------------------------------------
void Broker::parseModelDirectories()
{

}

//-------------------------------------------------------------------
void Broker::parseTerrainDirectories()
{
    const QDir terrainDir = "F:\\aXion_world\\HeightMaps\\";

    QStringList latfilters;
    latfilters.push_back("lat*");

    QStringList longfilters;
    longfilters.push_back("long*");

    //latitudes
    QFileInfoList latEntries = terrainDir.entryInfoList(latfilters, QDir::Dirs);
    for (int i = 0; i < latEntries.size(); ++i)
    {
        printf("%s\n", latEntries.at(i).absoluteFilePath().toStdString().c_str());

        //longitudes
        QDir latDir = latEntries.at(i).absoluteFilePath();
        QFileInfoList longEntries = latDir.entryInfoList(longfilters, QDir::Dirs);
        for (int i = 0; i < longEntries.size(); ++i)
        {
            printf("\t%s\n", longEntries.at(i).absoluteFilePath().toStdString().c_str());

            //tile content - the images
            QDir longDir = longEntries.at(i).absoluteFilePath();
            QFileInfoList tileContent = longDir.entryInfoList(QDir::Files);
            for (int i = 0; i < tileContent.size(); ++i)
            {
                printf("\t\t%s\n", tileContent.at(i).absoluteFilePath().toStdString().c_str());
            }
        }
    }
        
}
