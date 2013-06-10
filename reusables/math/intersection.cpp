//------------------------------------------------------------------------------
//! AUTHOR:  Pierre-Olivier Beaudoin
//------------------------------------------------------------------------------

#include "intersection.h"
#include "MathUtils.h"

namespace realisim
{
namespace math
{

//------------------------------------------------------------------------------
void Intersection2d::add( const Intersection2d& iI )
{ 
	for( int i = 0; i < iI.numberOfPoints(); ++i )
  {
  	mPoints.push_back( iI.point(i) );
    mNormals.push_back( iI.normal(i) );
  }
}

//------------------------------------------------------------------------------
void Intersection2d::add( const Point2d& p, const Vector2d& n )
{
	mPoints.push_back(p);
  mNormals.push_back(n.getUnit());
}
  
//------------------------------------------------------------------------------
/* voir http://en.wikipedia.org/wiki/Line-plane_intersection */
Point3d intersect( const Line3d& iL, const Plane& iP )
{
	Point3d r( std::numeric_limits<double>::quiet_NaN() );
	intersectionType t = itNone;
	double numerator = Vector3d( iL.getOrigin(), iP.getPoint() ) * iP.getNormal();
  double denominator = iL.getDirection() * iP.getNormal();
  
  t = itPoint;
  if( isEqual( denominator, 0.0 ) )
  	t = itNone;
  if( isEqual( numerator, 0.0 ) && isEqual( denominator, 0.0 ) )
  	t = itContained;
	double d = std::numeric_limits<double>::quiet_NaN();
  if( t == itPoint )
  {
  	d = numerator / denominator;
    r = iL.getOrigin() + d * iL.getDirection();
  }
  return r;
}
//------------------------------------------------------------------------------
Point3d intersect( const Plane& iP, const Line3d& iL )
{ return intersect( iL, iP ); }

//------------------------------------------------------------------------------
/* Considérons le segment de ligne l (AB) et le cercre centré en C 

	Le point D est la projection de AC sur AB.
  DC est le vecteur entre D et C.
  Lorsque que la norme de DC est plus petite que le rayon, nous savons qu'il
  y a une intersection sur la ligne AB. Il reste a determiner si le point 
  d'intersection est sur le segment AB.
  Ensuite on détermine s'il y a 1 ou 2 points d'intersections. Lorsque la
  norme de DC est strictement égale au rayon, il n'y a qu'une intersection parce
  que AB est tangente au cercle.
  Si la norme de DC est plus petite que le rayon, il y aura 2 intersections.
  Ensuite, pour chaque intersection, il faut d'éterminer le point précis de 
  l'intersection (E) et déterminer si ce point est sur le segment AB.
  
  Dans le cas de la tangente, le point d'intersection est déterminer par la 
  projection AC sur AB.
  
  Dans l'autre cas, on trouve le point d'intersection par trigonométrie. On
  connais le rayon et le point D. Par c^2 = a^2 + b^2 (triangle rectangle), on
  peut trouver la norme de ED. Ensuite les deux points d'intersections sont
  défini comme AD - ED et AD + ED.
  
  Pour terminer, afin de determiner si E est sur le segment AB on s'assure
  que le produit scalaire AB * AE est positif et que la norme de AB est plus
  grande qye la norme de AE
*/
Intersection2d intersects( const LineSegment2d& l, const Circle& c)
{
	Intersection2d r;
	Vector2d ab(l.a(), l.b());
  Vector2d abUnit = ab.getUnit();
  Vector2d ac(l.a(), c.center());
  double adNorm = ac * abUnit;
  Vector2d ad = adNorm * abUnit;
  Vector2d dc( l.a() + ad, c.center() );
  double dcNorm = dc.norm();
  if( dcNorm <= c.radius() )
  {
  	if( isEqual( dcNorm, c.radius(), 1.0e-5 ) ) //l'intersection est tangente
    { 
    	Point2d e = l.a() + ad;
    	Vector2d ae(l.a(), e);
      if( ab.normSquare() >= ae.normSquare() && ab * ae >= 0.0 )
      	r.add( e, dc );
    }
    else
    {
    	double edNorm = sqrt( std::abs( dc.normSquare() - c.radius() * c.radius() ) );
      Point2d e = l.a() + (adNorm - edNorm) * abUnit;
      Vector2d ae(l.a(), e);
      if( ab.normSquare() >= ae.normSquare() && ab * ae >= 0.0 )
      	r.add( e, dc );
      e = l.a() + (adNorm +edNorm) * abUnit;
      ae = Vector2d(l.a(), e);
      if( ab.normSquare() >= ae.normSquare() && ab * ae >= 0.0 )
      	r.add( e, dc );
    }
  }
  return r;
}

//------------------------------------------------------------------------------
Intersection2d intersects( const Circle& c, const LineSegment2d& l)
{ return intersects( l, c ); }

  
//------------------------------------------------------------------------------
Intersection2d intersects( const Circle& c, const Rectangle& r)
{
	Intersection2d result;  
//printf("bottomLeft %f, %f\n", r.bottomLeft().x(), r.bottomLeft().y() );
//printf("topLeft %f, %f\n", r.topLeft().x(), r.topLeft().y() );
//printf("topRight %f, %f\n", r.topRight().x(), r.topRight().y() );
//printf("bottomRight %f, %f\n", r.bottomRight().x(), r.bottomRight().y() );
  result.add( intersects( c, LineSegment2d( r.bottomLeft(), r.topLeft() ) ) );
  result.add( intersects( c, LineSegment2d( r.topLeft(), r.topRight() ) ) );
  result.add( intersects( c, LineSegment2d( r.topRight(), r.bottomRight() ) ) );
  result.add( intersects( c, LineSegment2d( r.bottomRight(), r.bottomLeft() ) ) );
  return result;
}

//------------------------------------------------------------------------------
Intersection2d intersects( const Rectangle& r, const Circle& c )
{ return intersects( c, r ); }

}
}

