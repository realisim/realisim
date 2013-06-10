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
    BoundingBox2d(const BoundingBox2d&){assert(0);}
    virtual ~BoundingBox2d();
  
  virtual void add (const Point2d& iV);
  virtual void clear();
  virtual const Point2d& bottomLeft() const {return mMin;}
  virtual const Point2d& topRight() const {return mMax;}
  virtual const Vector2d size() const { return mMax - mMin; }
protected:
  Point2d mMin;
  Point2d mMax;
};

BoundingBox2d::BoundingBox2d() :
  mMin(std::numeric_limits<double>::max()),
  mMax(-std::numeric_limits<double>::max())
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
void BoundingBox2d::clear()
{
  mMin.set(std::numeric_limits<double>::max());
  mMax.set(-std::numeric_limits<double>::max());
}

//------------------------------------------------------------------------------
//--  BoundingBox3
//------------------------------------------------------------------------------
template<class T>
class BoundingBox3
{
  public:
    BoundingBox3();
    BoundingBox3(const BoundingBox3<T>&){assert(0);}
    virtual ~BoundingBox3();
  
  virtual void add (const Point3<T>& iV);
  virtual void clear();
  const Point3<T>& getMin() const {return mMin;}
  const Point3<T>& getMax() const {return mMax;}
protected:
  Point3<T> mMin;
  Point3<T> mMax;
};

template<class T>
BoundingBox3<T>::BoundingBox3() :
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