/*
 
 */

#include "DataModel.h"
#include "Polygon.h"
#include <iostream>

using namespace std;
using namespace realisim;
	using namespace realisim::math;
using namespace realEdit;

//initialisation du membre static de DataModelBase
unsigned int DataModelBase::mIdCounter = 0;

//dummy pour les retour de fonction &
static RealEditPoint mDummyPoint = RealEditPoint();
static RealEditPolygon mDummyPolygon = RealEditPolygon();
  
#ifndef NDEBUG
static int newGuts = 0;
static int deleteGuts = 0;
static int newGutsPoly = 0;
static int deleteGutsPoly = 0;
static int newGutsModel = 0;
static int deleteGutsModel = 0;
#endif

//-------------------------DataModelBase--------------------------------------------
DataModelBase::DataModelBase() : mId( 0 )
{}

DataModelBase::DataModelBase (const DataModelBase& iD) : mId (iD.mId)
{}

DataModelBase::~DataModelBase()
{}

void DataModelBase::assign()
{
  ++mIdCounter;
  mId = mIdCounter;
  
#ifndef NDEBUG
std::cout<<"id: "<<mId<<std::endl;
#endif
}

//-------------------------RealEditPoint::Guts----------------------------------
RealEditPoint::Guts::Guts (const Point3d& iP) : mPoint (iP),
  mRefCount (1)
{
#ifndef NDEBUG
  ++newGuts;
  std::cout<<"new Guts Point: "<<newGuts<<std::endl;
#endif
}

RealEditPoint::Guts::~Guts ()
{
#ifndef NDEBUG
  ++deleteGuts;
  std::cout<<"delete Guts Point: "<<deleteGuts<<std::endl;
#endif
}

//-------------------------RealEditPoint----------------------------------------
/* Object dummy... rien n'est assigner et le ID est 0 */
RealEditPoint::RealEditPoint() : DataModelBase(),
  mpGuts (new Guts (Point3d (0.0)))
{}

RealEditPoint::RealEditPoint (const Point3d& iPos) : DataModelBase(),
  mpGuts (new Guts (iPos))
{ assign(); }

RealEditPoint::RealEditPoint (const RealEditPoint& iP) : DataModelBase (iP),
  mpGuts(iP.mpGuts)
{
  ++mpGuts->mRefCount;
}

RealEditPoint&
RealEditPoint::operator= (const RealEditPoint& iP)
{
  if (mpGuts == iP.mpGuts)
    return *this;

  if (--mpGuts->mRefCount == 0)
    delete mpGuts;
    
  mId = iP.getId();
  mpGuts = iP.mpGuts;
  ++mpGuts->mRefCount;
  return *this;
}

RealEditPoint::~RealEditPoint ()
{
  if (--mpGuts->mRefCount == 0)
    delete mpGuts;
}

void RealEditPoint::set(const Point3d& iP)
{
  mpGuts->mPoint.set(iP);
}
//--------------------------RealEditPolygon::Guts-------------------------------
RealEditPolygon::Guts::Guts() : mRefCount (1),
  mPoints (),
  mNormals ()
{
#ifndef NDEBUG
  ++newGutsPoly;
  std::cout<<"new Guts Poly: "<<newGutsPoly<<std::endl;
#endif
}

RealEditPolygon::Guts::Guts (const std::vector<RealEditPoint>& iP) :
  mRefCount (1),
  mPoints (iP),
  mNormals ()
{
  computeNormals();
  
#ifndef NDEBUG
  ++newGutsPoly;
  std::cout<<"new Guts Poly: "<<newGutsPoly<<std::endl;
#endif
}

RealEditPolygon::Guts::~Guts()
{
#ifndef NDEBUG
  ++deleteGutsPoly;
  std::cout<<"delete Guts Poly: "<<deleteGutsPoly<<std::endl;
#endif
}

//------------------------------------------------------------------------------
void RealEditPolygon::Guts::computeNormals()
{
  assert (mPoints. size () == 3);
  math::Polygon poly (mPoints[0].pos(), mPoints[1].pos(), mPoints[2].pos());
  
  mNormals.clear();
  mNormals. push_back (poly.getNormal ());
  mNormals. push_back (poly.getNormal ());
  mNormals. push_back (poly.getNormal ());
}

//--------------------------RealEditPolygon-------------------------------------
/*Object dummy... rien n'est assigné et le ID est 0 */
RealEditPolygon::RealEditPolygon () : DataModelBase(),
  mpGuts (new Guts ())
{}

/*Les Polygones sont temporairement limité a 3 points parce que la classe 
mathématique de Polygon est limité a 3 points.... il faudrait arranger
ca! */
RealEditPolygon::RealEditPolygon (const std::vector<RealEditPoint>& iP) :
  DataModelBase (),
  mpGuts (new Guts (iP))
{ assign (); }

RealEditPolygon::RealEditPolygon (const RealEditPolygon& iP) :
  DataModelBase (iP),
  mpGuts (iP.mpGuts)
{ ++mpGuts->mRefCount; }

RealEditPolygon& RealEditPolygon::operator= (const RealEditPolygon& iP)
{
  if (mpGuts == iP.mpGuts)
    return * this;
    
  if (--mpGuts->mRefCount == 0)
    delete mpGuts;
    
  mId = iP.getId ();
  mpGuts = iP.mpGuts;
  ++mpGuts->mRefCount;
  return *this;
}

RealEditPolygon::~RealEditPolygon()
{
  if (--mpGuts->mRefCount == 0)
    delete mpGuts;
}

//------------------------------------------------------------------------------
const RealEditPoint& RealEditPolygon::getPoint(unsigned int iIndex) const
{
  if(iIndex < getPoints().size())
    return getPoints()[iIndex];
  else
    return mDummyPoint;
}

//------------------------------------------------------------------------------
const std::vector<RealEditPoint>& RealEditPolygon::getPoints () const
{ return mpGuts->mPoints; }

//------------------------------------------------------------------------------
unsigned int RealEditPolygon::getPointCount () const
{ return mpGuts->mPoints.size(); }

//------------------------------------------------------------------------------
const std::vector<Vector3d>& RealEditPolygon::getNormals () const
{ return mpGuts->mNormals;}

//------------------------------------------------------------------------------
void RealEditPolygon::computeNormals()
{ mpGuts->computeNormals(); }

//------------------------RealEditModel::Guts-----------------------------------
RealEditModel::Guts::Guts () : mBoundingBox (),
  mPoints (),
  //mLineSegements(),
  mPolygons (),
  mRefCount (1),
  mCentroid(0.0)
{
#ifndef NDEBUG
  ++newGutsModel;
  std::cout<<"new Guts Model: "<<newGutsModel<<std::endl;
#endif
}

RealEditModel::Guts::~Guts ()
{
#ifndef NDEBUG
  ++deleteGutsModel;
  std::cout<<"delete Guts Model: "<<deleteGutsModel<<std::endl;
#endif
}

//------------------------RealEditModel-----------------------------------------
RealEditModel::RealEditModel() : DataModelBase(),
  mpGuts (new Guts ())
{
  assign();
}

RealEditModel::RealEditModel (const RealEditModel& iM) : DataModelBase(iM),
 mpGuts (iM.mpGuts)
{
  ++mpGuts->mRefCount;
}

RealEditModel& RealEditModel::operator= (const RealEditModel& iM)
{
  if (mpGuts == iM.mpGuts)
    return *this;
    
  if (--mpGuts->mRefCount == 0)
    delete mpGuts;
    
  mId = iM.getId ();
  mpGuts = iM.mpGuts;
  ++mpGuts->mRefCount;
  return *this;
}


RealEditModel::~RealEditModel ()
{
  if (--mpGuts->mRefCount == 0)
    delete mpGuts;
}

//------------------------------------------------------------------------------
void RealEditModel::addPoint (const RealEditPoint iP)
{
  mpGuts->mPoints.insert(make_pair(iP.getId(), iP));
  mpGuts->mBoundingBox.add(iP.pos());
}

//------------------------------------------------------------------------------
void RealEditModel::addPolygon (const RealEditPolygon iP)
{ mpGuts->mPolygons.insert(make_pair(iP.getId(), iP)); }

//------------------------------------------------------------------------------
const BB3d& RealEditModel::getBoundingBox () const
{ return mpGuts->mBoundingBox; }

const Point3d& RealEditModel::getCentroid () const
{
  map<unsigned int, RealEditPoint>::const_iterator it = getPoints().begin();
  for(; it != getPoints().end(); ++it)
    mpGuts->mCentroid += it->second.pos();
  mpGuts->mCentroid /= getPointCount();
  
  return mpGuts->mCentroid;
}

//------------------------------------------------------------------------------
unsigned int RealEditModel::getPointCount () const
{ return mpGuts->mPoints.size(); }

//------------------------------------------------------------------------------
const RealEditPoint& RealEditModel::getPoint(unsigned int iId) const
{
  map<unsigned int, RealEditPoint>::iterator it = mpGuts->mPoints.find(iId);
  if(it != mpGuts->mPoints.end())
    return it->second;
  return mDummyPoint;
}

//------------------------------------------------------------------------------
const map<unsigned int, RealEditPoint>& RealEditModel::getPoints () const
{ return mpGuts->mPoints; }

//------------------------------------------------------------------------------
unsigned int RealEditModel::getPolygonCount () const
{ return mpGuts->mPolygons.size(); }

//------------------------------------------------------------------------------
const RealEditPolygon& RealEditModel::getPolygon(unsigned int iId) const
{
  map<unsigned int, RealEditPolygon>::iterator it = mpGuts->mPolygons.find(iId);
  if(it != mpGuts->mPolygons.end())
    return it->second;
  return mDummyPolygon;
}

//------------------------------------------------------------------------------
const map<unsigned int, RealEditPolygon>& RealEditModel::getPolygons() const
{ return mpGuts->mPolygons; }

//------------------------------------------------------------------------------
bool RealEditModel::hasPoint (unsigned int iId) const
{
  map<unsigned int, RealEditPoint>::const_iterator it;
  it = mpGuts->mPoints.find(iId);
  if(it != mpGuts->mPoints.end())
    return true;
  return false;
}

//------------------------------------------------------------------------------
bool RealEditModel::hasPolygon (unsigned int iId) const
{
  map<unsigned int, RealEditPolygon>::const_iterator it;
  it = mpGuts->mPolygons.find(iId);
  if(it != mpGuts->mPolygons.end())
      return true;
  return false;
}

//------------------------------------------------------------------------------
void RealEditModel::updateBoundingBox()
{
  mpGuts->mBoundingBox.clear();
  
  map<unsigned int, RealEditPoint>::const_iterator it = mpGuts->mPoints.begin();
  while(it != mpGuts->mPoints.end())
  {
    mpGuts->mBoundingBox.add(it->second.pos());
    ++it;
  }
}

//------------------------------------------------------------------------------
void RealEditModel::updateNormals()
{
  map<unsigned int, RealEditPolygon>::iterator it;
  it = mpGuts->mPolygons.begin();
  while(it != mpGuts->mPolygons.end())
  {
    it->second.computeNormals();
    ++it;
  }
}
