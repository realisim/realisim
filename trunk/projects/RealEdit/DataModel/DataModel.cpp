/*
 
 */

#include "DataModel.h"
#include "Polygon.h"
#include <iostream>

using namespace realisim;
using namespace realisim::math;
using namespace realisim::treeD;
using namespace realEdit;

//initialisation du membre static
unsigned int DataModelBase::mIdCounter = 0;

static int newGuts = 0;
static int deleteGuts = 0;
static int newGutsPoly = 0;
static int deleteGutsPoly = 0;
static int newGutsModel = 0;
static int deleteGutsModel = 0;

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
  std::cout<<"id: "<<mId<<std::endl;
}

//-------------------------RealEditPoint::Guts----------------------------------
RealEditPoint::Guts::Guts (const Point3d& iP) : mPoint (iP),
  mRefCount (1)
{
  ++newGuts;
  std::cout<<"new Guts Point: "<<newGuts<<std::endl;
}

RealEditPoint::Guts::~Guts ()
{
  ++deleteGuts;
  std::cout<<"delete Guts Point: "<<deleteGuts<<std::endl;
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
  ++newGutsPoly;
  std::cout<<"new Guts Poly: "<<newGutsPoly<<std::endl;
}

RealEditPolygon::Guts::Guts (const std::vector<RealEditPoint>& iP) :
  mRefCount (1),
  mPoints (iP),
  mNormals ()
{
  assert (iP. size () == 3);
  math::Polygon poly (mPoints[0].pos(), mPoints[1].pos(), mPoints[2].pos());
  
  mNormals. push_back (poly.getNormal ());
  mNormals. push_back (poly.getNormal ());
  mNormals. push_back (poly.getNormal ());
  
  ++newGutsPoly;
  std::cout<<"new Guts Poly: "<<newGutsPoly<<std::endl;
}

RealEditPolygon::Guts::~Guts()
{
  ++deleteGutsPoly;
  std::cout<<"delete Guts Poly: "<<deleteGutsPoly<<std::endl;
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
const std::vector<RealEditPoint>& RealEditPolygon::getPoints () const
{ return mpGuts->mPoints; }

//------------------------------------------------------------------------------
const std::vector<Vector3d>& RealEditPolygon::getNormals () const
{ return mpGuts->mNormals;}

//------------------------RealEditModel::Guts-----------------------------------
RealEditModel::Guts::Guts () : mBoundingBox (),
  mPoints (),
  //mLineSegements(),
  mPolygons (),
  mRefCount (1)
{
  ++newGutsModel;
  std::cout<<"new Guts Model: "<<newGutsModel<<std::endl;
}

RealEditModel::Guts::~Guts ()
{
  ++deleteGutsModel;
  std::cout<<"delete Guts Model: "<<deleteGutsModel<<std::endl;
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
  mpGuts->mPoints. push_back (iP);
  mpGuts->mBoundingBox. add (iP.pos ());
}

//------------------------------------------------------------------------------
void RealEditModel::addPolygon (const RealEditPolygon iP)
{ mpGuts->mPolygons. push_back (iP); }

//------------------------------------------------------------------------------
const BB3d& RealEditModel::getBoundingBox () const
{ return mpGuts->mBoundingBox; }

//------------------------------------------------------------------------------
unsigned int RealEditModel::getPointCount () const
{ return mpGuts->mPoints.size(); }

//------------------------------------------------------------------------------
const RealEditPoint& RealEditModel::getPoint (unsigned int iIndex) const
{ return mpGuts->mPoints[iIndex]; }

//------------------------------------------------------------------------------
unsigned int RealEditModel::getPolygonCount () const
{ return mpGuts->mPolygons.size(); }

//------------------------------------------------------------------------------
const RealEditPolygon& RealEditModel::getPolygon (unsigned int iIndex) const
{ return mpGuts->mPolygons[iIndex]; }
