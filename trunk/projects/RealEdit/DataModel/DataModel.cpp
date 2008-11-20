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
RealEditPolygon::RealEditPolygon( RealEditPoint* ipP1,
                                  RealEditPoint* ipP2,
                                  RealEditPoint* ipP3 ) :
DataModelBase(),
mPoints(),
mNormals()
{
  assert( ipP1 && ipP2 && ipP3 );
  mPoints.push_back(ipP1);
  mPoints.push_back(ipP2);
  mPoints.push_back(ipP3);
  
  Polygon poly( *ipP1, *ipP2, *ipP3 );
  
  mNormals.push_back( poly.getNormal() );
  mNormals.push_back( poly.getNormal() );
  mNormals.push_back( poly.getNormal() );
}

RealEditPolygon::~RealEditPolygon()
{
}

//------------------------RealEditModel--------------------------------------------
RealEditModel::RealEditModel( const std::string& iString ) : 
DataModelBase(),
mPoints(),
mPolygons(),
mName( iString )
{
}

RealEditModel::RealEditModel( const RealEditModel& iModel )
{
}

RealEditModel::~RealEditModel()
{
}
