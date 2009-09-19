/*
 *  BoundingBox.h
 */

#ifndef realisim_math_BoundingBox_hh
#define realisim_math_BoundingBox_hh

#include <cassert>
#include <limits>
#include "Point.h"

namespace realisim
{
namespace math
{

template<class T>
class BoundingBox
{
  public:
    BoundingBox();
    BoundingBox(const BoundingBox<T>&){assert(0);}
    virtual ~BoundingBox();
  
  virtual void add (const Point<T>& iV);
  virtual void clear();
  const Point<T>& getMin() const {return mMin;}
  const Point<T>& getMax() const {return mMax;}
protected:
  Point<T> mMin;
  Point<T> mMax;
};

template<class T>
BoundingBox<T>::BoundingBox() :
  mMin(std::numeric_limits<T>::max()),
  mMax(-std::numeric_limits<T>::max())
{}

template<class T>
BoundingBox<T>::~BoundingBox()
{}

//------------------------------------------------------------------------------
template<class T>
void BoundingBox<T>::add (const Point<T>& iP)
{
  mMin.minCoord (iP);
  mMax.maxCoord (iP);
}

//------------------------------------------------------------------------------
template<class T>
void BoundingBox<T>::clear()
{
  mMin.set(std::numeric_limits<T>::max());
  mMax.set(-std::numeric_limits<T>::max());
}

typedef BoundingBox<double> BB3d;
typedef BoundingBox<int> BB3i;

} //math
} //

#endif