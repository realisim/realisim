//------------------------------------------------------------------------------
//! AUTHOR:  Pierre-Olivier Beaudoin
//------------------------------------------------------------------------------

#ifndef realisim_math_intersection_hh
#define realisim_math_intersection_hh

#include <limits>
#include "math/Primitives.h"
#include "math/Point.h"
//#include "Line3d.h"
#include "Plane.h"
#include <vector>

//------------------------------------------------------------------------------
namespace realisim
{
namespace math
{
	class Intersection2d
  {
  public:
  	Intersection2d() : mPoints(), mNormals(),
    	mPenetration( std::numeric_limits<double>::quiet_NaN() ) {;}
    Intersection2d( const Intersection2d& i ) :
    	mPoints( i.mPoints ), mNormals( i.mNormals ),
      mPenetration( i.getPenetration() ) {;}
    Intersection2d& operator=(const Intersection2d& i)
    { mPoints = i.mPoints; mNormals = i.mNormals; 
      mPenetration = i.mPenetration; return *this; }
    virtual ~Intersection2d() {;}
    
    virtual void add( const Intersection2d& );
    virtual void add( const Point2d& );
    virtual void add( const Point2d&, const Vector2d& );
    virtual void clear();
    virtual bool hasContacts() const { return !mPoints.empty(); }
    virtual int getNumberOfContacts() const { return (int)mPoints.size(); }
    virtual Vector2d getPenetration() const { return mPenetration; }
    virtual Point2d getPoint(int i) const { return mPoints[i]; }
    virtual Vector2d getNormal(int i) const { return mNormals[i]; }
    virtual void setPenetration( const Vector2d& iV ) { mPenetration = iV; }
    
  protected:    
    std::vector< Point2d > mPoints;
    std::vector< Vector2d > mNormals;
    Vector2d mPenetration;
  };
	enum intersectionType{ itNone, itPoint, itContained };
  
//Point3d intersect( const Line3d&, const Plane& );
//Point3d intersect( const Plane&, const Line3d& );
	bool intersects( const Circle&, const Circle& );
  bool intersects( const Line2d&, const Line2d& );
  bool intersects( const LineSegment2d&, const LineSegment2d& );
  bool intersects( const LineSegment2d&, const Rectangle& );
  bool intersects( const Rectangle&, const Rectangle& );
  bool intersects( const Rectangle&, const LineSegment2d& );

	Intersection2d intersect( const Circle&, const LineSegment2d& );
  Intersection2d intersect( const Line2d&, const Line2d& );
  Intersection2d intersect( const LineSegment2d&, const Circle& );
  Intersection2d intersect( const LineSegment2d&, const LineSegment2d& );
  Intersection2d intersect( const LineSegment2d&, const Rectangle& );
  Intersection2d intersect( const Circle&, const Rectangle& );
  Intersection2d intersect( const Rectangle&, const Circle& );
  Intersection2d intersect( const Rectangle&, const LineSegment2d& );
  Intersection2d intersect( const Rectangle&, const Rectangle& );
  double axisOverLap( double, double, double, double, double* = 0, double* = 0 );
} //math
} // end of namespace realisim
#endif // LINE_H
