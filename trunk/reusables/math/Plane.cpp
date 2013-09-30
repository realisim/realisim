//------------------------------------------------------------------------------
//! AUTHOR:  Pierre-Olivier Beaudoin
//------------------------------------------------------------------------------

#include "Plane.h"

using namespace realisim;
	using namespace math;

Plane::Plane() :
 mPoint( 0.0 ),
 mNormal( 0.0, 1.0, 0.0)
{}

Plane::Plane( const Point3d& iP, const Vector3d& iN ) :
 mPoint( iP ),
 mNormal( iN )
{}

Plane::Plane( const Plane& iP ) :
	mPoint( iP.getPoint() ),
  mNormal( iP.getNormal() )
{}

const Plane& Plane::operator=( const Plane& iP )
{
	mPoint = iP.getPoint();
  mNormal = iP.getNormal();
  return *this;
}

Plane::~Plane()
{}

//------------------------------------------------------------------------------
const Point3d& Plane::getPoint() const
{ return mPoint; }
//------------------------------------------------------------------------------
const Vector3d& Plane::getNormal() const
{ return mNormal; }
//------------------------------------------------------------------------------
void Plane::setNormal( const Vector3d& iN )
{ mNormal = iN; }
//------------------------------------------------------------------------------
void Plane::setPoint( const Point3d& iP )
{ mPoint = iP; }
