//------------------------------------------------------------------------------
//! AUTHOR:  Pierre-Olivier Beaudoin
//------------------------------------------------------------------------------

#ifndef realisim_math_plane_hh
#define realisim_math_plane_hh

#include "Point.h"
#include "Vect.h"

//------------------------------------------------------------------------------
namespace realisim
{
namespace math
{

class Plane
{
public:
	Plane();
  Plane( const Point3d&, const Vector3d& );
  Plane( const Plane& );
  const Plane& operator= ( const Plane& );
  virtual ~Plane();
  
  const Vector3d& getNormal() const;
  const Point3d& getPoint() const;
  void setNormal( const Vector3d& );
  void setPoint( const Point3d& );
  //Point3d toLocal( const Point3d& ) const;
  //Point3d toGlobal( const Point3d& ) const;
  
protected:
  Point3d mPoint;
  Vector3d mNormal;
};

} //math
} // end of namespace realisim
#endif // LINE_H
