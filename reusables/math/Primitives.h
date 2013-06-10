
#ifndef realisim_math_primitives_hh
#define realisim_math_primitives_hh

#include "math/Point.h"
#include "math/Vect.h"
#include <vector>

namespace realisim
{
namespace math
{

//------------------------------------------------------------------------------
class LineSegment2d
{
public:
  LineSegment2d();
  LineSegment2d( const Point2d& iA, const Point2d& iB ) : mA(iA), mB(iB) {}
  LineSegment2d( const LineSegment2d& l) : mA( l.mA ), mB(l.mB) {}
  LineSegment2d& operator=( const LineSegment2d& iL ) 
  { mA = iL.mA; mB = iL.mB; return *this; }
  virtual ~LineSegment2d() {}
  
  virtual const Point2d& a() const {return mA;}
  virtual const Point2d& b() const {return mB;}
  virtual void set( const Point2d& iA, const Point2d& iB ) {mA = iA; mB = iB;}
  virtual void setA( const Point2d& iA ) { mA = iA; }
  virtual void setB( const Point2d& iB ) { mB = iB; }
  
protected:
	Point2d mA;
  Point2d mB;
};

//------------------------------------------------------------------------------
class Circle
{
public:
  Circle();
  Circle( const Point2d&, double );
  Circle( const Circle& );
  virtual ~Circle();
  Circle& operator=( const Circle& );
  
  virtual double area() const;
  virtual Point2d center() const;
  virtual bool contains( const Point2d& ) const;
  virtual double radius() const;
  virtual void setCenter( const Point2d& );
  virtual void setRadius( double );
  
protected:
	Point2d mCenter;
	double mRadius;
};

//------------------------------------------------------------------------------
class Rectangle
{
public:
  Rectangle();
  Rectangle( const Point2d&, const Point2d& );
  Rectangle( const Point2d&, const Vector2d& );
  Rectangle( const Rectangle& );
  virtual ~Rectangle();
  Rectangle& operator=( const Rectangle& );
  
  virtual double area() const;
  virtual Point2d center() const;
  virtual bool contains( const Point2d& ) const;
	virtual double bottom() const;
  virtual Point2d bottomLeft() const;
  virtual Point2d bottomRight() const;
  virtual double left() const;
  virtual double right() const;
  virtual std::vector<Point2d> points() const;
  virtual void setBottom(double);
  virtual void setBottomLeft(const Point2d&);
  virtual void setBottomRight(const Point2d&);
  virtual void setLeft(double);
  virtual void setRight(double);
  virtual void setTop(double);
  virtual void setTopLeft(const Point2d&);
  virtual void setTopRight(const Point2d&);
  virtual Vector2d size() const;
  virtual double top() const;
  virtual Point2d topLeft() const;
  virtual Point2d topRight() const;
  
protected:
	Point2d mBottomLeft;
	Point2d mTopRight;
};

} //math
} // end of namespace realisim
#endif // LINE_H
