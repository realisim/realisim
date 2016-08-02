
#include "Broker.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

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
    const QDir terrainDir( "F:\\aXion_world\\HeightMaps\\" );

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
        for (int j = 0; j < longEntries.size(); ++j)
        {
            printf("\t%s\n", longEntries.at(j).absoluteFilePath().toStdString().c_str());

            //tile content - the images
            QDir longDir = longEntries.at(j).absoluteFilePath();
            QFileInfoList tileContent = longDir.entryInfoList(QDir::Files);
            for (int k = 0; k < tileContent.size(); ++k)
            {
                printf("\t\t%s\n", tileContent.at(k).absoluteFilePath().toStdString().c_str());
            }
        }
    }
        
}
