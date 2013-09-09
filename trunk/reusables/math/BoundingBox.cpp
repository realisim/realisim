
#include "math/BoundingBox.h"

using namespace realisim;
  using namespace math;

BoundingBox2d::BoundingBox2d() :
  mMin(std::numeric_limits<double>::max()),
  mMax(-std::numeric_limits<double>::max())
{}

BoundingBox2d::BoundingBox2d( const Point2d& p, const Vector2d& s ) :
  mMin(std::numeric_limits<double>::max()),
  mMax(-std::numeric_limits<double>::max())
{ add(p); add(p+s);}

BoundingBox2d::BoundingBox2d(const BoundingBox2d& bb) :
  mMin( bb.mMin ),
  mMax( bb.mMax )
{}

BoundingBox2d::~BoundingBox2d()
{}

//------------------------------------------------------------------------------
void BoundingBox2d::add (const Point2d& iP)
{
  mMin.minCoord (iP);
  mMax.maxCoord (iP);
}

//------------------------------------------------------------------------------
const Point2d& BoundingBox2d::bottomLeft() const
{return mMin;}

//------------------------------------------------------------------------------
bool BoundingBox2d::contains( const Point2d& p, bool iInclusive /*true*/) const
{
	if( iInclusive )
  	return p.x() >= bottomLeft().x() && p.x() <= topRight().x() && 
    	p.y() >= bottomLeft().y() && p.y() <= topRight().y();
  else
  	return p.x() > bottomLeft().x() && p.x() < topRight().x() && 
    	p.y() > bottomLeft().y() && p.y() < topRight().y();

}
  
//------------------------------------------------------------------------------
void BoundingBox2d::clear()
{
  mMin.set(std::numeric_limits<double>::max());
  mMax.set(-std::numeric_limits<double>::max());
}

//------------------------------------------------------------------------------
Point2d BoundingBox2d::point(int i) const
{
	Point2d r(0.0);
  switch (i) 
  {
    case 0: r = bottomLeft(); break;
    case 1: r = Point2d( bottomLeft().x(), topRight().y() ); break; //topleft
    case 2: r = topRight(); break;
    case 3: r = Point2d( topRight().x(), bottomLeft().y() ); break; //bottomRight
    default: break;
  }
	return r;
}

//------------------------------------------------------------------------------
vector<Point2d> BoundingBox2d::points() const
{
	vector<Point2d> r;
  r.push_back( point(0) );
  r.push_back( point(1) ); //topleft
  r.push_back( point(2) );
  r.push_back( point(3) ); //bottom right
	return r;
}

//------------------------------------------------------------------------------
Vector2d BoundingBox2d::size() const 
{ return mMax - mMin; }
//------------------------------------------------------------------------------
const Point2d& BoundingBox2d::topRight() const 
{return mMax;}
  

