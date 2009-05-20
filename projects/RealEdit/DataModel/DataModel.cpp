/*
 
 */

#include "DataModel.h"
#include "Polygon.h"
#include <iostream>

using namespace Realisim;
using namespace RealEdit;

//initialisation du membre static
unsigned int DataModelBase::mIdCounter = 0;

DataModelBase::DataModelBase() : mId( 0 )
{
  ++mIdCounter;
  mId = mIdCounter;
  std::cout<<"id: "<<mId<<std::endl;
}

DataModelBase::~DataModelBase()
{}

//-------------------------Point--------------------------------------------
RealEditPoint::RealEditPoint(const Point3d& iPos) : 
DataModelBase(),
Point3d( iPos )
{
}

RealEditPoint::~RealEditPoint()
{}

//--------------------------RealEditPolygon-------------------------------------
/*
Les Polygones sont temporairement limité a 3 points parce que la classe 
mathématique de Polygon est limité a 3 points.... il faudrait arranger
ca!
*/
RealEditPolygon::RealEditPolygon( const std::vector<RealEditPoint*>& iP ) :
DataModelBase(),
mPoints(iP),
mNormals()
{
  Polygon poly( *mPoints[0], *mPoints[1], *mPoints[2] );
  
  mNormals.push_back( poly.getNormal() );
  mNormals.push_back( poly.getNormal() );
  mNormals.push_back( poly.getNormal() );
}

RealEditPolygon::~RealEditPolygon()
{
}

const std::vector<RealEditPoint*>&
RealEditPolygon::getPoints() const
{
  return mPoints;
}

const std::vector<Realisim::Vector3d>&
RealEditPolygon::getNormals() const
{
  return mNormals;
}

//------------------------RealEditModel--------------------------------------------
RealEditModel::RealEditModel() : 
DataModelBase(),
mPoints(),
mPolygons()
{
}

RealEditModel::~RealEditModel()
{
}

//------------------------------------------------------------------------------
void
RealEditModel::addPoint( const RealEditPoint* ipPoint )
{
  mPoints.push_back( ipPoint );
}

//------------------------------------------------------------------------------
void
RealEditModel::addPolygon( const RealEditPolygon* ipPoly )
{
  mPolygons.push_back(ipPoly);
}

//------------------------------------------------------------------------------
unsigned int
RealEditModel::getPointCount() const
{
  return mPoints.size();
}

//------------------------------------------------------------------------------
const RealEditPoint*
RealEditModel::getPoint(unsigned int iIndex) const
{
  return mPoints[iIndex];
}

//------------------------------------------------------------------------------
unsigned int
RealEditModel::getPolygonCount() const
{
  return mPolygons.size();
}

//------------------------------------------------------------------------------
const RealEditPolygon*
RealEditModel::getPolygon(unsigned int iIndex) const
{
  return mPolygons[iIndex];
}
