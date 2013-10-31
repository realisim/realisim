
#include "Primitives.h"
#include "math/MathUtils.h"

namespace realisim
{
namespace math
{

//------------------------------------------------------------------------------
//--- Circle
//------------------------------------------------------------------------------  
Circle::Circle() : mCenter(0.0), mRadius(0.0) {}

Circle::Circle( const Point2d& iC, double iR ) : mCenter( iC ), mRadius( iR )
{}

Circle::Circle( const Circle& iC ) : mCenter( iC.center() ),
	mRadius( iC.radius() )
{}

Circle::~Circle() {}

Circle& Circle::operator=( const Circle& iC )
{
	mRadius = iC.radius();
  mCenter = iC.center();
  return *this;
}

//------------------------------------------------------------------------------  
double Circle::area() const
{ return 0.0; }

//------------------------------------------------------------------------------  
Point2d Circle::center() const
{ return mCenter; }

//------------------------------------------------------------------------------  
bool Circle::contains( const Point2d& iP ) const
{ return ( iP - center() ).norm() <= radius(); }

//------------------------------------------------------------------------------  
double Circle::radius() const
{ return mRadius; }

//------------------------------------------------------------------------------  
void Circle::setCenter( const Point2d& iC )
{ mCenter = iC; }

//------------------------------------------------------------------------------  
void Circle::setRadius( double iR )
{ mRadius = iR; }

//------------------------------------------------------------------------------
//--- Rectangle
//------------------------------------------------------------------------------  
Rectangle::Rectangle() : mBottomLeft(0.0), mTopRight(0.0) {}

Rectangle::Rectangle( const Point2d& iBl, const Point2d& iTr) :
  mBottomLeft( iBl ),
  mTopRight( iTr )
{}

Rectangle::Rectangle( const Point2d& iBl, const Vector2d& iS ) :
  mBottomLeft( iBl ),
  mTopRight( iBl + iS )
{}

Rectangle::Rectangle( const Rectangle& iR ) : 
	mBottomLeft( iR.bottomLeft() ),
  mTopRight( iR.topRight() )
{}
  
Rectangle::~Rectangle() {}

Rectangle& Rectangle::operator=( const Rectangle& iR)
{
	mBottomLeft = iR.bottomLeft();
  mTopRight = iR.topRight();
  return *this;
}

//------------------------------------------------------------------------------  
double Rectangle::area() const
{ return size().x() * size().y(); }
//------------------------------------------------------------------------------  
Point2d Rectangle::center() const
{ return toPoint( (mTopRight - mBottomLeft) / 2.0 ); }
//------------------------------------------------------------------------------  
bool Rectangle::contains( const Point2d& p ) const
{
	return p.x() >= left() && p.x() <= right() && p.y() >= bottom()
    && p.y() <= top();
}
//------------------------------------------------------------------------------  
double Rectangle::bottom() const
{ return mBottomLeft.y(); }
//------------------------------------------------------------------------------  
Point2d Rectangle::bottomLeft() const
{ return mBottomLeft; }
//------------------------------------------------------------------------------  
Point2d Rectangle::bottomRight() const
{ return Point2d( right(), bottom() ); }

//------------------------------------------------------------------------------  
double Rectangle::width() const
{ return size().x(); }

//------------------------------------------------------------------------------  
double Rectangle::left() const
{ return mBottomLeft.x(); }

//------------------------------------------------------------------------------
Point2d Rectangle::point(int i) const
{
	Point2d r(0.0);
  switch (i) 
  {
    case 0: r = bottomLeft(); break;
    case 1: r = topLeft(); break;
    case 2: r = topRight(); break;
    case 3: r = bottomRight(); break;
    default: break;
  }
	return r;
}

//------------------------------------------------------------------------------  
vector<Point2d> Rectangle::points() const
{
	vector<Point2d> r;
  r.push_back( bottomLeft() );
  r.push_back( topLeft() );
  r.push_back( topRight() );
  r.push_back( bottomRight() );
	return r;
}
//------------------------------------------------------------------------------  
double Rectangle::right() const
{ return mTopRight.x(); }
//------------------------------------------------------------------------------  
double Rectangle::top() const
{ return mTopRight.y(); }
//------------------------------------------------------------------------------  
Point2d Rectangle::topLeft() const
{ return Point2d( left(), top() ); }
//------------------------------------------------------------------------------  
Point2d Rectangle::topRight() const
{ return mTopRight; }
//------------------------------------------------------------------------------  
void Rectangle::setBottom(double iV)
{ mBottomLeft.setY( iV ); }
//------------------------------------------------------------------------------  
void Rectangle::setBottomLeft(const Point2d& iV)
{ mBottomLeft = iV; }
//------------------------------------------------------------------------------  
void Rectangle::setBottomRight(const Point2d& iV)
{ setBottom( iV.y() ); setRight( iV.x() ); }
//------------------------------------------------------------------------------  
void Rectangle::setLeft(double iV)
{ mBottomLeft.setX( iV ); }
//------------------------------------------------------------------------------  
void Rectangle::setRight(double iV)
{ mTopRight.setX( iV ); }
//------------------------------------------------------------------------------  
void Rectangle::setTop(double iV)
{ mTopRight.setY( iV ); }
//------------------------------------------------------------------------------  
void Rectangle::setTopLeft(const Point2d& iV)
{ setTop( iV.y() ); setLeft( iV.x() ); }
//------------------------------------------------------------------------------  
void Rectangle::setTopRight(const Point2d& iV)
{ mTopRight = iV; }
//------------------------------------------------------------------------------  
Vector2d Rectangle::size() const
{ return mTopRight - mBottomLeft; }
//------------------------------------------------------------------------------  
double Rectangle::height() const
{ return size().y(); }

}
}

