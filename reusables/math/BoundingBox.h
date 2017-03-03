/*
 *  BoundingBox.h
 */

#ifndef realisim_math_BoundingBox_hh
#define realisim_math_BoundingBox_hh

#include <cassert>
#include <limits>
#include "Point.h"
#include "math/MathUtils.h"

namespace realisim
{
namespace math
{

//------------------------------------------------------------------------------
//--  BoundingBox2
//------------------------------------------------------------------------------
class BoundingBox2d
{
  public:
    BoundingBox2d();
    BoundingBox2d( const Point2d& p, const Vector2d& s );
    BoundingBox2d(const BoundingBox2d&);
    virtual ~BoundingBox2d();
  
  virtual void add (const Point2d&);
  virtual bool contains( const Point2d&, bool ) const;
  virtual void clear();
  virtual const Point2d& bottomLeft() const;
  virtual Point2d point(int) const;
virtual std::vector<Point2d> points() const;
  virtual const Point2d& topRight() const;
  virtual Vector2d size() const;
protected:
  Point2d mMin;
  Point2d mMax;
};

//------------------------------------------------------------------------------
//--  BoundingBox3
//------------------------------------------------------------------------------
// axis aligned bounding box...
template<class T>
class BoundingBox3
{
  public:
    BoundingBox3();
    BoundingBox3(const BoundingBox3<T>&) = default;
    BoundingBox3& operator=(const BoundingBox3<T>&) = default;
    virtual ~BoundingBox3();
  
  virtual void add (const Point3<T>& iV);
  virtual void clear();
  Point3<T> getCenter() const { return (mMin + (mMax - mMin)/2.0); }
  const Point3<T>& getMin() const {return mMin;}
  const Point3<T>& getMax() const {return mMax;}
  bool isValid() const {return mIsValid;}
  
protected:
    bool mIsValid;
  Point3<T> mMin;
  Point3<T> mMax;
};

template<class T>
BoundingBox3<T>::BoundingBox3() :
    mIsValid(false),
  mMin(std::numeric_limits<T>::max()),
  mMax(-std::numeric_limits<T>::max())
{}

template<class T>
BoundingBox3<T>::~BoundingBox3()
{}

//------------------------------------------------------------------------------
template<class T>
void BoundingBox3<T>::add (const Point3<T>& iP)
{
    mIsValid = true;
  mMin.minCoord (iP);
  mMax.maxCoord (iP);
}

//------------------------------------------------------------------------------
template<class T>
void BoundingBox3<T>::clear()
{
  mMin.set(std::numeric_limits<T>::max());
  mMax.set(-std::numeric_limits<T>::max());
}

typedef BoundingBox3<double> BB3d;
typedef BoundingBox3<int> BB3i;

} //math
} //

#endif