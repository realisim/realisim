/*
 
 */

#include "DataModel/DataModel.h"
#include "math/Polygon.h"
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
static RealEditSegment mDummySegment = RealEditSegment();
  
#ifndef NDEBUG
#define DEBUG_GUTS 0
static int newGuts = 0;
static int deleteGuts = 0;
static int newSegmentGuts = 0;
static int deleteSegmentGuts = 0;
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
  setId(mIdCounter);
  
#if DEBUG_GUTS
cout<<"id: "<<mId<<endl;
#endif
}

//-------------------------RealEditPoint::Guts----------------------------------
RealEditPoint::Guts::Guts (const Point3d& iP) : mPoint (iP),
  mRefCount (1)
{
#if DEBUG_GUTS
  ++newGuts;
  cout<<"new Guts Point: "<<newGuts<<endl;
#endif
}

RealEditPoint::Guts::~Guts ()
{
#if DEBUG_GUTS
  ++deleteGuts;
  cout<<"delete Guts Point: "<<deleteGuts<<endl;
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
//--------------------------RealEditSegment::Guts-------------------------------
RealEditSegment::Guts::Guts() : mRefCount(1),
  mPoint1(),
  mPoint2()
{
#if DEBUG_GUTS
  ++newSegmentGuts;
  cout<<"new Segment Guts: "<<newSegmentGuts<<endl;
#endif
}

RealEditSegment::Guts::Guts(RealEditPoint p1, RealEditPoint p2) : mRefCount(1),
  mPoint1(),
  mPoint2()
{
	if(p1.getId() < p2.getId())
  { mPoint1 = p1; mPoint2 = p2; }
  else
  { mPoint1 = p2; mPoint2 = p1; }

#if DEBUG_GUTS
  ++newSegmentGuts;
  cout<<"new Segment Guts: "<<newSegmentGuts<<endl;
#endif
}

RealEditSegment::Guts::~Guts()
{
#if DEBUG_GUTS
  ++deleteSegmentGuts;
  cout<<"delete Segment Guts: "<<deleteSegmentGuts<<endl;
#endif
}

//------------------------------------------------------------------------------
/*Object dummy... rien n'est assigné et le ID est 0 */
RealEditSegment::RealEditSegment() :
  DataModelBase(),
  mpGuts(new Guts())
{}

RealEditSegment::RealEditSegment(RealEditPoint iP1, RealEditPoint iP2) :
  DataModelBase(),
  mpGuts(new Guts(iP1, iP2))
{ assign(); }

RealEditSegment::RealEditSegment(const RealEditSegment& iS) :
  DataModelBase(iS),
  mpGuts(iS.mpGuts)
{ ++mpGuts->mRefCount; }

RealEditSegment& RealEditSegment::operator=(const RealEditSegment& iS)
{
  if (mpGuts == iS.mpGuts)
    return * this;
    
  if (--mpGuts->mRefCount == 0)
    delete mpGuts;
    
  mId = iS.getId ();
  mpGuts = iS.mpGuts;
  ++mpGuts->mRefCount;
  return *this;
}

RealEditSegment::~RealEditSegment()
{
  if (--mpGuts->mRefCount == 0)
    delete mpGuts;
}

//------------------------------------------------------------------------------
/*Ce comparateur est utilisé afin de pouvoir insérer les segments dans
  les conteneurs std qui necessite un 'Strict weak ordering'. Voir
  RealEditModel::mSegmentPool.*/
bool RealEditSegment::Comparator::operator()(const RealEditSegment& iS1,
  const RealEditSegment& iS2) const
{
  if(iS1.mpGuts->mPoint1.getId() < iS2.mpGuts->mPoint1.getId())
    return true;
  else if(iS1.mpGuts->mPoint1.getId() == iS2.mpGuts->mPoint1.getId() && 
    iS1.mpGuts->mPoint2.getId() < iS2.mpGuts->mPoint2.getId())
    return true;
  else
    return false;
}

//------------------------------------------------------------------------------
const RealEditPoint& RealEditSegment::getPoint1() const
{return mpGuts->mPoint1;}

//------------------------------------------------------------------------------
const RealEditPoint& RealEditSegment::getPoint2() const
{return mpGuts->mPoint2;}

//--------------------------RealEditPolygon::Guts-------------------------------
RealEditPolygon::Guts::Guts() : mRefCount (1),
  mPoints (),
  mSegments(),
  mNormals ()
{
#if DEBUG_GUTS
  ++newGutsPoly;
  cout<<"new Guts Poly: "<<newGutsPoly<<endl;
#endif
}

RealEditPolygon::Guts::Guts (const vector<RealEditPoint>& iP) :
  mRefCount (1),
  mPoints (iP),
  mSegments(),
  mNormals()
{
	makeSegments();
  computeNormals();
  
#if DEBUG_GUTS
  ++newGutsPoly;
  cout<<"new Guts Poly: "<<newGutsPoly<<endl;
#endif
}

RealEditPolygon::Guts::~Guts()
{
#if DEBUG_GUTS
  ++deleteGutsPoly;
  cout<<"delete Guts Poly: "<<deleteGutsPoly<<endl;
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

void RealEditPolygon::Guts::makeSegments()
{
	const vector<RealEditPoint>& p = mPoints;
  for(unsigned int i = 1; i < p.size(); ++i)
  	mSegments.push_back(RealEditSegment(p[i-1], p[i]));
	mSegments.push_back(RealEditSegment(p[p.size()-1], p[0]));
}
//--------------------------RealEditPolygon-------------------------------------
/*Object dummy... rien n'est assigné et le ID est 0 */
RealEditPolygon::RealEditPolygon () : DataModelBase(),
  mpGuts (new Guts ())
{}

/*Les Polygones sont temporairement limité a 3 points parce que la classe 
mathématique de Polygon est limité a 3 points.... il faudrait arranger
ca! */
RealEditPolygon::RealEditPolygon (const vector<RealEditPoint>& iP) :
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
void RealEditPolygon::computeNormals()
{ mpGuts->computeNormals(); }

//------------------------------------------------------------------------------
const RealEditPoint& RealEditPolygon::getPoint(unsigned int iIndex) const
{
  if(iIndex < getPoints().size())
    return getPoints()[iIndex];
  else
    return mDummyPoint;
}

//------------------------------------------------------------------------------
const vector<RealEditPoint>& RealEditPolygon::getPoints () const
{ return mpGuts->mPoints; }

//------------------------------------------------------------------------------
unsigned int RealEditPolygon::getPointCount () const
{ return mpGuts->mPoints.size(); }

//------------------------------------------------------------------------------
const Vector3d& RealEditPolygon::getNormal (unsigned int iIndex) const
{
  assert(iIndex < getNormals().size());
  return getNormals()[iIndex];
}

//------------------------------------------------------------------------------
const vector<Vector3d>& RealEditPolygon::getNormals () const
{ return mpGuts->mNormals;}

//------------------------------------------------------------------------------
const RealEditSegment& RealEditPolygon::getSegment(unsigned int iIndex) const
{
 if(iIndex < getSegments().size())
    return getSegments()[iIndex];
  else
    return mDummySegment;
}

//------------------------------------------------------------------------------
const vector<RealEditSegment>& RealEditPolygon::getSegments() const
{ return mpGuts->mSegments; }

//------------------------------------------------------------------------------
void RealEditPolygon::setSegment(unsigned int i, RealEditSegment iS)
{
	mpGuts->mSegments[i] = iS;
}

//------------------------RealEditModel::Guts-----------------------------------
RealEditModel::Guts::Guts () : mBoundingBox (),
  mPoints (),
  mPolygons (),
  mSegments(),
  mSegmentPool(),
  mRefCount (1),
  mCentroid(0.0)
{
#if DEBUG_GUTS
  ++newGutsModel;
  cout<<"new Guts Model: "<<newGutsModel<<endl;
#endif
}

RealEditModel::Guts::~Guts ()
{
#if DEBUG_GUTS
  ++deleteGutsModel;
  cout<<"delete Guts Model: "<<deleteGutsModel<<endl;
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
void RealEditModel::addPoint (RealEditPoint iP)
{
  mpGuts->mPoints.insert(make_pair(iP.getId(), iP));
  mpGuts->mBoundingBox.add(iP.pos());
}

//------------------------------------------------------------------------------
void RealEditModel::addPolygon (RealEditPolygon iP)
{ 
  mpGuts->mPolygons.insert(make_pair(iP.getId(), iP));
  
  /*Pour chaque segment du polygon, on va regarder s'il est déjà présent
  dans le pool de segment, si oui, on remplace le segment du polygon par
  celui du pool, si non, on insère se segment dans le pool et dans la 
  map des segments du models.*/
  const vector<RealEditSegment>& s = iP.getSegments();
  for(unsigned int i = 0; i < s.size(); ++i)
  {
  	set<RealEditSegment, RealEditSegment::Comparator>::iterator it =
      mpGuts->mSegmentPool.find(s[i]);
  	if(it != mpGuts->mSegmentPool.end())
    {
    	/*on remplace le segment i du polygon parce que ce dernier est
      déjà dans le pool de segment opur ce model*/
      iP.setSegment(i, *it);
    }
    else
    {    	
      addSegment(s[i]);
    }
  }
}

//------------------------------------------------------------------------------
void RealEditModel::addSegment (RealEditSegment iS)
{
  mpGuts->mSegments.insert(make_pair(iS.getId(), iS)); 
  /*Les segments est nouveau. On l'ajoute au pool et au model.*/
  mpGuts->mSegmentPool.insert(iS);
}

//------------------------------------------------------------------------------
const BB3d& RealEditModel::getBoundingBox () const
{ return mpGuts->mBoundingBox; }

//------------------------------------------------------------------------------
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
/*cette méthode const est dangereuse parce qu'elle retourne une reference const
sur un object qui est partagé implicitement. Donc si l'utilisateur fait une
copie et modifie la copie, il modifiera aussi l'object référencé qui se
veut const!!!*/  
RealEditPoint& RealEditModel::getPoint(unsigned int iId) const
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
/*Attention! voir RealEditModel::getPoint(unsigned int)*/ 
RealEditPolygon& RealEditModel::getPolygon(unsigned int iId) const
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
vector<RealEditPolygon> RealEditModel::getPolygonsContainingPoint(unsigned int iId) const
{
	vector<RealEditPolygon> r;
  if(!hasPoint(iId))
  	return r;
    
	map<unsigned int, RealEditPolygon>::const_iterator it = getPolygons().begin();
	for(; it != getPolygons().end(); ++it)
  {
  	const RealEditPolygon& p = it->second;
    for(unsigned int j = 0; j < p.getPoints().size(); ++j)
    {
    	if(p.getPoint(j).getId() == iId)
      {
      	r.push_back(p);
        break;
      }
    }
  }
  return r;
}

//------------------------------------------------------------------------------
vector<RealEditPolygon> RealEditModel::getPolygonsContainingSegment(unsigned int iId) const
{
  vector<RealEditPolygon> r;
  if(!hasSegment(iId))
  	return r;
    
	map<unsigned int, RealEditPolygon>::const_iterator it = getPolygons().begin();
	for(; it != getPolygons().end(); ++it)
  {
  	const RealEditPolygon& p = it->second;
    for(unsigned int j = 0; j < p.getSegments().size(); ++j)
    {
    	if(p.getSegment(j).getId() == iId)
      {
      	r.push_back(p);
        break;
      }
    }
  }
  return r;
}

//------------------------------------------------------------------------------
const map<unsigned int, RealEditSegment>& RealEditModel::getSegments() const
{return mpGuts->mSegments;}

//------------------------------------------------------------------------------
/*Attention! voir RealEditModel::getPoint(unsigned int)*/ 
RealEditSegment& RealEditModel::getSegment(unsigned int iId) const
{
  map<unsigned int, RealEditSegment>::iterator it = mpGuts->mSegments.find(iId);
  if(it != mpGuts->mSegments.end())
    return it->second;
  return mDummySegment;
}

//------------------------------------------------------------------------------
vector<RealEditSegment> RealEditModel::getSegmentsContainingPoint(unsigned int iId) const
{
  vector<RealEditSegment> r;
  if(!hasPoint(iId))
  	return r;
  
	map<unsigned int, RealEditSegment>::const_iterator it = getSegments().begin();
	for(; it != getSegments().end(); ++it)
  {
  	const RealEditSegment& s = it->second;
    if(s.getPoint1().getId() == iId || s.getPoint2().getId() == iId)
    	r.push_back(s);
  }
  return r;
}

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
bool RealEditModel::hasSegment (unsigned int iId) const
{
  map<unsigned int, RealEditSegment>::const_iterator it;
  it = mpGuts->mSegments.find(iId);
  if(it != mpGuts->mSegments.end())
      return true;
  return false;
}

//------------------------------------------------------------------------------
void RealEditModel::removePoint(unsigned int iId)
{
	if(!hasPoint(iId))
  	return;
  mpGuts->mPoints.erase(iId);
}

//------------------------------------------------------------------------------
void RealEditModel::removePolygon(unsigned int iId)
{
	if(!hasPolygon(iId))
  	return;
  mpGuts->mPolygons.erase(iId);
}

//------------------------------------------------------------------------------
void RealEditModel::removeSegment(unsigned int iId)
{
  if(!hasSegment(iId))
    return;
  //on enleve le segment du pool avant d'enelever le segment du model,
  //sinon la méthode getSegment va échouer!
  mpGuts->mSegmentPool.erase(getSegment(iId));
  mpGuts->mSegments.erase(iId);
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
