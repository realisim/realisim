#ifndef REALISIM_MATH_POLYGON_H
#define REALISIM_MATH_POLYGON_H

#include "Point.h"
#include "Vect.h"
#include <vector>
namespace realisim { namespace math { class Polygon; } }
/*
	voir http://en.wikipedia.org/wiki/Polygon
  
  Note: Le constructeur Polygon( const std::vector< Point3d* >& )
  ne prend pas possesion des pointeurs.*/

class realisim::math::Polygon
{
public:
  Polygon();
  Polygon(const Polygon& );
  Polygon( const std::vector< Point3d >& );
  Polygon( const std::vector< Point3d* >& );
  virtual ~Polygon();

  virtual double getArea() const;
  virtual Point3d getCentroid() const;
	virtual const Vector3d& getNormal() const;
	virtual int getNumberOfVertices() const;
	virtual Point3d& getVertex( int );
  virtual const Point3d& getVertex( int ) const;
  virtual bool isConvex() const;
  virtual bool isCoplanar() const;
  virtual void invalidate();
  virtual bool operator== ( const Polygon& ) const;
  virtual Polygon& operator= ( const Polygon& );

protected:
  virtual void checkIfConvex() const;
  virtual void checkIfCoplanar() const;
  virtual bool isValid() const { return mIsValid; }
	virtual void validate() const;
  
  std::vector< Point3d* > mVertices;
  mutable Vector3d mNormal;
  mutable bool mIsCoplanar;
  mutable bool mIsConvex;
  mutable bool mIsValid;
  bool mAreVerticesOwned;
};
#endif
