//------------------------------------------------------------------------------
//! AUTHOR:  Pierre-Olivier Beaudoin
//------------------------------------------------------------------------------

#include "intersection.h"
#include "BoundingBox.h"
#include "MathUtils.h"

namespace realisim
{
namespace math
{

//------------------------------------------------------------------------------
void Intersection2d::add( const Intersection2d& iI )
{
	mHasIntersections = true;
	for( int i = 0; i < iI.getNumberOfPoints(); ++i )
  {
  	mPoints.push_back( iI.getPoint(i) );
    mNormals.push_back( iI.getNormal(i) );
  }
}

//------------------------------------------------------------------------------
void Intersection2d::add( const Point2d& p )
{
	mHasIntersections = true;
	mPoints.push_back(p);
  mNormals.push_back( Vector2d(0.0) );
}

//------------------------------------------------------------------------------
void Intersection2d::add( const Point2d& p, const Vector2d& n )
{
	mHasIntersections = true;
	mPoints.push_back(p);
  mNormals.push_back(n.getUnit());
}

//------------------------------------------------------------------------------
void Intersection2d::clear()
{
	mHasIntersections = false;
  mPoints.clear();
  mNormals.clear();
}
  
//------------------------------------------------------------------------------
/* voir http://en.wikipedia.org/wiki/Line-plane_intersection */
//Point3d intersect( const Line3d& iL, const Plane& iP )
//{
//	Point3d r( std::numeric_limits<double>::quiet_NaN() );
//	intersectionType t = itNone;
//	double numerator = Vector3d( iL.getOrigin(), iP.getPoint() ) * iP.getNormal();
//  double denominator = iL.getDirection() * iP.getNormal();
//  
//  t = itPoint;
//  if( isEqual( denominator, 0.0 ) )
//  	t = itNone;
//  if( isEqual( numerator, 0.0 ) && isEqual( denominator, 0.0 ) )
//  	t = itContained;
//	double d = std::numeric_limits<double>::quiet_NaN();
//  if( t == itPoint )
//  {
//  	d = numerator / denominator;
//    r = iL.getOrigin() + d * iL.getDirection();
//  }
//  return r;
//}
////------------------------------------------------------------------------------
//Point3d intersect( const Plane& iP, const Line3d& iL )
//{ return intersect( iL, iP ); }

bool intersects( const Circle& iA, const Circle& iB )
{
	return Vector2d( iA.getCenter(), iB.getCenter() ).norm() <= 
  	( iA.getRadius() + iB.getRadius() );
}

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
Intersection2d intersect( const LineSegment2d& l, const Circle& c)
{
	Intersection2d r;
	Vector2d ab(l.a(), l.b());
  Vector2d abUnit = ab.getUnit();
  Vector2d ac(l.a(), c.getCenter());
  double adNorm = ac * abUnit;
  Vector2d ad = adNorm * abUnit;
  Vector2d dc( l.a() + ad, c.getCenter() );
  double dcNorm = dc.norm();
  if( dcNorm <= c.getRadius() )
  {
  	if( isEqual( dcNorm, c.getRadius(), 1.0e-5 ) ) //l'intersection est tangente
    { 
    	Point2d e = l.a() + ad;
    	Vector2d ae(l.a(), e);
      if( ab.normSquare() >= ae.normSquare() && ab * ae >= 0.0 )
      	r.add( e, dc );
    }
    else
    {
    	double edNorm = sqrt( std::abs( dc.normSquare() - c.getRadius() * c.getRadius() ) );
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
Intersection2d intersect( const Circle& c, const LineSegment2d& l)
{ return intersect( l, c ); }

  
//------------------------------------------------------------------------------
Intersection2d intersect( const Circle& c, const Rectangle& r)
{
	Intersection2d result;  
//printf("bottomLeft %f, %f\n", r.bottomLeft().x(), r.bottomLeft().y() );
//printf("topLeft %f, %f\n", r.topLeft().x(), r.topLeft().y() );
//printf("topRight %f, %f\n", r.topRight().x(), r.topRight().y() );
//printf("bottomRight %f, %f\n", r.bottomRight().x(), r.bottomRight().y() );
  result.add( intersect( c, LineSegment2d( r.bottomLeft(), r.topLeft() ) ) );
  result.add( intersect( c, LineSegment2d( r.topLeft(), r.topRight() ) ) );
  result.add( intersect( c, LineSegment2d( r.topRight(), r.bottomRight() ) ) );
  result.add( intersect( c, LineSegment2d( r.bottomRight(), r.bottomLeft() ) ) );
  return result;
}

//------------------------------------------------------------------------------
Intersection2d intersect( const Rectangle& r, const Circle& c )
{ return intersect( c, r ); }

//------------------------------------------------------------------------------
Intersection2d intersect( const Rectangle& r1, const Rectangle& r2 )
{
	Intersection2d r;
  
	//intersection en utilisant minkowski
  BoundingBox2d mink;
  for( int j = 0; j < 4; j++ )
    for( int i = 0; i < 4; i++ )
    {
      mink.add( toPoint(r2.point(j) - r1.point(i)) );
    }
    
  if( mink.contains(Point2d(0.0), true ) )
  {
  	r.add( r1.getCenter() ); //un point bidon...
    
    double olx = axisOverLap( r1.bottomLeft().x(), r1.bottomRight().x(),
     r2.bottomLeft().x(), r2.bottomRight().x() );
    double oly = axisOverLap( r1.bottomLeft().y(), r1.topLeft().y(),
     r2.bottomLeft().y(), r2.topLeft().y() );
    Vector2d p( olx, oly );
    r.setPenetration( p );
  }  
  return r;
}

//------------------------------------------------------------------------------
double axisOverLap( double l1a, double l1b, double l2a, double l2b,
	double* ra /*=0*/, double* rb /*=0*/ )
{
	double r = 0.0, p1 = 0.0, p2 = 0.0;
  vector<double> v(4, 0);
  v[0] = l1a; v[1] = l1b;
  v[2] = l2a; v[3] = l2b;
  sort( v.begin(), v.end() );

  //l1 est contenu par dans l2
  if( l1a >= l2a && l1a <= l2b &&
    l1b >= l2a && l1b <= l2b )
  { r = ( v[2] - v[0] ); p1 = v[2]; p2 = v[0]; }
  //l1.b est dans l2
  else if( l1b >= l2a &&  l1b <= l2b )
  { r = ( v[2] - v[1] ); p1 = v[2]; p2 = v[1]; }
  //l1.a est dans r2
  else if( l1a >= l2a && l1a <= l2b )
  { r = ( v[1] - v[2] ); p1 = v[1]; p2 = v[2]; }
  //l1 contient l2
  else
  { r = ( v[3] - v[1] ); p1 = v[3]; p2 = v[1]; }
  
  if( ra && rb ){ *ra = p1; *rb = p2; } 
  return r;
}

}
}