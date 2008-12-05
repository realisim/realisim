/*
 
 */

#include "DataModel.h"
#include "Polygon.h"

using namespace Realisim;
using namespace RealEdit;

//initialisation du membre static
unsigned int DataModelBase::mIdCounter = 0;

DataModelBase::DataModelBase() : mId( 0 )
{
  ++mIdCounter;
  mId = mIdCounter;
}

DataModelBase::~DataModelBase()
{}

//-------------------------Point--------------------------------------------
RealEditPoint::RealEditPoint(const Point3f& iPos) : 
DataModelBase(),
Point3f( iPos )
{
}

RealEditPoint::~RealEditPoint()
{}

//--------------------------RealEditPolygon-----------------------------------------
RealEditPolygon::RealEditPolygon( const std::vector<RealEditPoint*>& iP ) :
DataModelBase(),
mPoints(),
mNormals()
{
  mPoints = iP;
  
  Polygon poly( *mPoints[0], *mPoints[1], *mPoints[2] );
  
  mNormals.push_back( poly.getNormal() );
  mNormals.push_back( poly.getNormal() );
  mNormals.push_back( poly.getNormal() );
}

RealEditPolygon::~RealEditPolygon()
{
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

void
RealEditModel::addPoint( const RealEditPoint* ipPoint )
{
  mPoints.push_back( ipPoint );
}

void
RealEditModel::addPolygon( const RealEditPolygon* ipPoly )
{
  mPolygons.push_back( ipPoly );
}

unsigned int
RealEditModel::getPointCount() const
{
  return mPoints.size();
}

const RealEditPoint*
RealEditModel::getPoint( int iIndex ) const
{
  return mPoints[iIndex];
}
