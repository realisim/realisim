#include "BezierCurve.h"
#include <cassert>
#include <cmath>
#include "MathUtils.h"

using namespace realisim;
  using namespace math;
using namespace std;

Point3d BezierCurve::mDummyPoint = Point3d(0.0);
static unsigned int kNumSegments = 15;

//---- ControlPoint
BezierCurve::ControlPoint::ControlPoint() : mIsHandleTo(-1),
  mPos(0.0)
{}

BezierCurve::ControlPoint::ControlPoint(int iIndex, const Point3d& iP) :
  mIsHandleTo(iIndex),
  mPos(iP)
{}

BezierCurve::ControlPoint::ControlPoint(const ControlPoint& iP) :
  mIsHandleTo(iP.isHandleTo()),
  mPos(iP.getPos())
{}

BezierCurve::ControlPoint& 
BezierCurve::ControlPoint::operator=(const BezierCurve::ControlPoint& iP)
{
	mIsHandleTo = iP.isHandleTo();
  mPos = iP.getPos();
  return *this;
}

BezierCurve::ControlPoint::~ControlPoint()
{}

//--- Bezier Curve
BezierCurve::BezierCurve() : mControlPoints(),
  mRaster(),
  mIsRasterValid(false),
  mIsClosed(false)
{}

BezierCurve::BezierCurve(const BezierCurve& iB) :
  mControlPoints(iB.getControlPoints()),
  mRaster(iB.isRasterValid() ? iB.getRaster() : std::vector<Point3d>()),
  mIsRasterValid(iB.isRasterValid()),
  mIsClosed(iB.isClosed())
{}

BezierCurve& BezierCurve::operator=(const BezierCurve& iB)
{
  mControlPoints = iB.getControlPoints();
  if(iB.isRasterValid())
  	mRaster = iB.getRaster();
  mIsRasterValid = iB.isRasterValid();
  mIsClosed = iB.isClosed();
  return *this;
}

BezierCurve::~BezierCurve()
{}

//-----------------------------------------------------------------------------
int BezierCurve::add(const Point3d& iP)
{
  mIsRasterValid = false;
	ControlPoint p(-1, iP);
	mControlPoints.push_back(p);
  return mControlPoints.size() - 1;
}

//-----------------------------------------------------------------------------
int BezierCurve::addHandlesTo(int iIndex)
{
	//Si l'index n'est pas dnans la courbe ou si l'index refere à
  //un handle, on sort.
  if(iIndex < 0 || (unsigned int)iIndex >= mControlPoints.size()
    || mControlPoints[iIndex].isHandle())
  {
    return -1;
  }

	int r = -1;
  mIsRasterValid = false;
  
  vector<ControlPoint>::iterator it;	
  //On ajoute un seul handle au premier cp de la courbe
  if(mControlPoints.size() == 1) 
  {
  	ControlPoint p1(iIndex, getPos(iIndex));
    mControlPoints.push_back(p1);
    r = mControlPoints.size() - 1;
  }
  //On ajoute un/deux handle(s) au premier cp (quand on ferme la courbe)
  else if (iIndex == 0)
  {
  	if(mControlPoints[1].isHandleTo() == 0)
    {
      /*Si le premier point de la courbe a déja un handle, on
        en ajoute seulement un de plus a l'index size() - 1*/
      ControlPoint p1(iIndex, getPos(iIndex));
      mControlPoints.push_back(p1);
      r = mControlPoints.size() - 1;
    }    
    else
    {
      /*Sinon on ajoute 2 handles. Un a l'index 1 et un a l'index
        size() - 1*/
      ControlPoint p1(iIndex, getPos(iIndex));
      ControlPoint p2(iIndex, getPos(iIndex));
      mControlPoints.push_back(p2);
    	it = mControlPoints.insert(mControlPoints.begin() + 1, p1);  
      
      /*On réajuste tout les index des cp qui suivent le point
        1 (handle du point 0) puisqu'on vient d'ajouter un handle
        à l'index 1.*/
      for(unsigned int i = 2; i < mControlPoints.size() - 1; ++i)
        if(mControlPoints[i].isHandle())
          mControlPoints[i].setAsHandleTo(mControlPoints[i].isHandleTo() + 1);
      r = 1;
    }  	
  }
  //On ajoute deux handle(s) a un point sur la courbe (le dernier inclu)
  else 
  {
    /*On ajoute 1 a l'index parce qu'on va insérer p1 juste avant
    le cp à l'index iIndex */
  	ControlPoint p1(iIndex+1, getPos(iIndex));
    ControlPoint p2(iIndex+1, getPos(iIndex));
    it = mControlPoints.insert(mControlPoints.begin() + iIndex, p1);
    
    if(it + 2 == mControlPoints.end())
    {
      mControlPoints.push_back(p2);
      r = mControlPoints.size() - 1;
    }    
    else
    {
      it = mControlPoints.insert(it + 2, p2);
      
      /*On doit réajuster tous les mHandleTo (index) des handles
        qui suivent les 2 handles qui viennent d'être ajoutés*/
      for(unsigned int i = iIndex+3; i < mControlPoints.size(); ++i)
        if(mControlPoints[i].isHandle())
          mControlPoints[i].setAsHandleTo(mControlPoints[i].isHandleTo() + 2);
      
      r = it - mControlPoints.begin();
    }
  }
  
  return r;
}

//-----------------------------------------------------------------------------
bool BezierCurve::canCloseOn(int iIndex)
{ 
  if(iIndex == 0)
    return true;
  return false;
}

//-----------------------------------------------------------------------------
void BezierCurve::close()
{
  mIsRasterValid = false; 
  mIsClosed = true;
}

//-----------------------------------------------------------------------------
/*Retourne vrai si la liste des points de controle contient au moins 2 points
  qui ne sont pas des handles.*/
bool BezierCurve::containsAtLeast2Points() const
{
  int count = 0;
  bool r = false;
  for(unsigned int i = 0; i < mControlPoints.size(); ++i)
  {
    if(!mControlPoints[i].isHandle())
    {
      ++count;
      if(count >= 2)
      {
        r = true;
        break;
      }
    }
  }
  return r;
}

//-----------------------------------------------------------------------------
void BezierCurve::cubicRasterization(const vector<Point3d>& iV) const
{
	double dt = 1.0 / kNumSegments;
  double t = 0.0;
  double a, b, c, d;
	for(unsigned int i = 0; i <= kNumSegments; ++i)
  {
  	a = pow(1.0-t, 3);
    b = 3 * (1-t) * (1-t) * t;
    c = 3 * (1-t) * t * t;
    d = pow(t, 3);
    
    mRaster.push_back(a*iV[0] + b*iV[1] + c*iV[2] + d*iV[3] );
    t += dt;
  }
}

//-----------------------------------------------------------------------------
const vector<BezierCurve::ControlPoint>& BezierCurve::getControlPoints() const
{ return mControlPoints; }

//-----------------------------------------------------------------------------
const Point3d& BezierCurve::getPos(int iIndex) const
{
	if(iIndex < 0 || (unsigned int)iIndex >= mControlPoints.size())
  { return mDummyPoint; }
  
  return mControlPoints[iIndex].getPos();
}

//-----------------------------------------------------------------------------
const vector<Point3d>& BezierCurve::getRaster() const
{
	if(isRasterValid())
  	return mRaster;

  mRaster.clear();
  
  /*Il doit y avoir au moins 2 cp qui ne sont pas des handles afin
  de pouvoir rasterizer*/
  if(!containsAtLeast2Points())
  	return mRaster;
  
  vector<Point3d> v;
  v.reserve(4);
  int nonHandle = 0;
  for(unsigned int i = 0; i < mControlPoints.size(); )
  {
  	const ControlPoint& cp = mControlPoints[i];
    v.push_back(cp.getPos());
    if(!cp.isHandle())
    	++nonHandle;
    
    if(nonHandle == 2)
    {
      rasterizeSegment(v);
      nonHandle = 0;
      v.clear();
      --i;
    }
    ++i;
  }
  
  v.clear();
  int size = mControlPoints.size();
  if(isClosed())
  {
  	if(mControlPoints[size - 1].isHandle() && 
       mControlPoints[size - 2].isHandle())
    {
      v.push_back(mControlPoints[size-3].getPos());
      v.push_back(mControlPoints[size-2].getPos());
      v.push_back(mControlPoints[size-1].getPos());
    }
    else if(mControlPoints[size - 1].isHandle())
    {
      v.push_back(mControlPoints[size-2].getPos());
      v.push_back(mControlPoints[size-1].getPos());
    }
    else
      v.push_back(mControlPoints[size-1].getPos());
    v.push_back(mControlPoints[0].getPos());
  	rasterizeSegment(v);
  }

  
  mIsRasterValid = true;
	return mRaster;
}

//-----------------------------------------------------------------------------
bool BezierCurve::isRasterValid() const
{ return mIsRasterValid; }

//-----------------------------------------------------------------------------
void BezierCurve::move(int iIndex, const Vector3d& iV)
{
  if(iIndex < 0 || (unsigned int)iIndex >= mControlPoints.size())
  { return; }
  
  mIsRasterValid = false;
  mControlPoints[iIndex].setPos(getPos(iIndex) + iV);
  
  /*Si on bouge un handle, on doit aussi déplacer l'autre handle
    (si présente) de ce point.*/
  if(mControlPoints[iIndex].isHandle())
  {
  	int handleTo = mControlPoints[iIndex].isHandleTo();
    int otherHandle = -1;
    for(unsigned int i = 0; i < mControlPoints.size(); ++i)
    {
    	/*si on trouve un autre handle qui est appliquer au même point*/
    	if((int)i != iIndex && mControlPoints[i].isHandleTo() == handleTo)
      {
      	otherHandle = i;
        break;
      }
    }
    
    Vector3d delta(mControlPoints[iIndex].getPos(),
      mControlPoints[handleTo].getPos());
    
    mControlPoints[otherHandle].setPos(mControlPoints[handleTo].getPos() + delta);
  }
  /*Si on ne deplace pas un handle on doit déplacer tous les handles
    de ce point de controle du même delta*/
  else
  {
  	int count = 0;
    for(unsigned int i = 0; i < mControlPoints.size(); ++i)
    {
    	if(mControlPoints[i].isHandleTo() == iIndex)
      {
        mControlPoints[i].setPos(getPos(i) + iV);
        if(++count == 2)
        	break;
      }
    }
  }

}

//-----------------------------------------------------------------------------
void BezierCurve::quadraticRasterization(const vector<Point3d>& iV) const
{
	double dt = 1.0 / kNumSegments;
  double t = 0.0;
  double a, b, c;
	for(unsigned int i = 0; i <= kNumSegments; ++i)
  {
  	a = (1-t) * (1-t);
    b = 2 * (1-t) * t;
    c = t * t;
    
    mRaster.push_back(a*iV[0] + b*iV[1] + c*iV[2]);
    t += dt;
  }
}

//-----------------------------------------------------------------------------
void BezierCurve::rasterizeSegment(const vector<Point3d>& iV) const
{
  if(!mRaster.empty())
    mRaster.pop_back();
  if(iV.size() == 2) //linear
  {        
    mRaster.push_back(iV[0]);
    mRaster.push_back(iV[1]);
  }
  else if(iV.size() == 3) //quadratic
  {
    quadraticRasterization(iV);
  }
  else if(iV.size() == 4)	//cubic
  {
    cubicRasterization(iV);
  }
  else
    assert(0); //cette taille n'est pas supportée
}

//-----------------------------------------------------------------------------
void BezierCurve::remove(int iIndex)
{
  if(iIndex < 0 || (unsigned int)iIndex >= mControlPoints.size())
  { return; }
  
  mIsRasterValid = false;
  const ControlPoint& cp = mControlPoints[iIndex];  
  
  /*Si le point de contrôle n'est pas un handle, il faut enlever tous les
    handles qui pointe vers ce point et remettre tous les index des autres
    handle à jour*/
  int startIndex = iIndex;
  int endIndex = iIndex;
  int count = 1;
  vector<ControlPoint>::iterator it;
	if(!cp.isHandle())
  {
  	if(iIndex > 0 && mControlPoints[iIndex - 1].isHandleTo() == iIndex)
    	startIndex = iIndex - 1;
    if((unsigned int)iIndex < mControlPoints.size() -1 && 
       mControlPoints[iIndex + 1].isHandleTo() == iIndex)
    	endIndex = iIndex + 1;
    
    /*Cas spécial pour iIndex == 0*/
    if(iIndex == 0)
    {
    	if((unsigned int)endIndex + 1 < mControlPoints.size() - 1 && 
         mControlPoints[endIndex + 1].isHandle())
      	endIndex += 1;
    }
    
    count = endIndex - startIndex + 1;
    it = mControlPoints.begin() + startIndex;
    for(int i = 0; i < count; ++i)
      it = mControlPoints.erase(it);
  }
  else
		it = mControlPoints.erase(mControlPoints.begin() + iIndex);  
    
  //on remet tous les index à jour
  for(; it != mControlPoints.end(); ++it)
  {
  	if(it->isHandle())
	  	it->setAsHandleTo(it->isHandleTo() - count);
  }
}

//-----------------------------------------------------------------------------
void BezierCurve::setPos(int iIndex, const Point3d& iP)
{
  if(iIndex < 0 || (unsigned int)iIndex >= mControlPoints.size())
  { return; }
  
  mIsRasterValid = false;
  mControlPoints[iIndex].setPos(iP);
}