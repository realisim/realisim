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

Intersection2d::Intersection2d(const Point2d& p, const Vector2d& n /*=0.0*/)
{ add( p, n ); }

//------------------------------------------------------------------------------
void Intersection2d::add( const Intersection2d& iI )
{
	for( int i = 0; i < iI.getNumberOfContacts(); ++i )
  {
  	mPoints.push_back( iI.getContact(i) );
    mNormals.push_back( iI.getNormal(i) );
  }
}

//------------------------------------------------------------------------------
void Intersection2d::add( const Point2d& p, const Vector2d& n /*=0.0*/ )
{
	mPoints.push_back(p);
  mNormals.push_back(n.getUnit());
}

//------------------------------------------------------------------------------
void Intersection2d::clear()
{
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

//------------------------------------------------------------------------------
bool intersects( const Circle& iA, const Circle& iB )
{
	return Vector2d( iA.getCenter(), iB.getCenter() ).norm() <= 
  	( iA.getRadius() + iB.getRadius() );
}

//------------------------------------------------------------------------------
bool intersects( const Line2d& iL1, const Line2d& iL2 )
{ return intersect( iL1, iL2 ).hasContacts(); }
  
//------------------------------------------------------------------------------
bool intersects( const Line2d& iL1, const LineSegment2d& iL2 )
{ return intersect( iL1, iL2 ).hasContacts(); }

//------------------------------------------------------------------------------
bool intersects( const Line2d& l, const Rectangle& r )
{ return intersect( l, r ).hasContacts(); }

//------------------------------------------------------------------------------
bool intersects( const LineSegment2d& iL1, const LineSegment2d& iL2 )
{ return intersect(iL1, iL2).hasContacts() ; }

//------------------------------------------------------------------------------
bool intersects( const LineSegment2d& iL1, const Line2d& iL2 )
{ return intersect(iL2, iL1).hasContacts() ; }

//------------------------------------------------------------------------------
bool intersects( const LineSegment2d& iL, const Rectangle& iR )
{ return intersect(iL, iR).hasContacts() ; }

//------------------------------------------------------------------------------
bool intersects( const Rectangle& iA, const Rectangle& iB )
{
  //intersection en utilisant minkowski
  BoundingBox2d mink;
  for( int j = 0; j < 4; j++ )
    for( int i = 0; i < 4; i++ )
    {
      mink.add( toPoint(iB.point(j) - iA.point(i)) );
    }
    
  return mink.contains(Point2d(0.0), true );
}

//------------------------------------------------------------------------------
bool intersects( const Rectangle& iR, const LineSegment2d& iL )
{ return intersect(iR, iL).hasContacts() ; }


//------------------------------------------------------------------------------
Intersection2d intersect( const Circle& c, const LineSegment2d& l)
{ return intersect( l, c ); }

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
      Vector2d ec( c.getCenter(), e );
      if( ab.normSquare() >= ae.normSquare() && ab * ae >= 0.0 )
      	r.add( e, ec );
    }
    else
    {
    	double edNorm = sqrt( std::abs( dc.normSquare() - c.getRadius() * c.getRadius() ) );
      Point2d e = l.a() + (adNorm - edNorm) * abUnit;
      Vector2d ae(l.a(), e);
      Vector2d ec( c.getCenter(), e );
      if( ab.normSquare() >= ae.normSquare() && ab * ae >= 0.0 )
      	r.add( e, ec );
      e = l.a() + (adNorm +edNorm) * abUnit;
      ae = Vector2d(l.a(), e);
      ec = Vector2d( c.getCenter(), e );
      if( ab.normSquare() >= ae.normSquare() && ab * ae >= 0.0 )
      	r.add( e, ec );
    }
  }
  return r;
}

//------------------------------------------------------------------------------
Intersection2d intersect( const Line2d& iL1, const Line2d& iL2)
{
	Intersection2d r;
  Point2d p1 = iL1.getPoint(), p2 = iL2.getPoint();
  Vector2d v1 = iL1.getDirection(), v2 = iL2.getDirection();
  
  //TODO quoi faire quand les lignes sont paralleles
  
  double s = 0.0, t = 0.0;
  if( isEqual(v1.x(), 0.0) )
  {
  	s = -( p2.x() - p1.x() ) / v2.x();
    t = ( p2.y() - p1.y() + s*v2.y() ) / v1.y();
  }
  else if( isEqual( v1.y(), 0.0 ) )
  {
    s = -( p2.y() - p1.y() ) / v2.y();
    t = ( p2.x() - p1.x() + s*v2.x() ) / v1.x();
  }
  else if( isEqual( v2.x(), 0.0 ) )
  {
    t = ( p2.x() - p1.x() ) / v1.x();
    s = -( p2.y() - p1.y() - t*v1.y() ) / v2.y();
  }
  else if( isEqual( v2.y(), 0.0 ) )
  {
    t = ( p2.y() - p1.y() ) / v1.y();
    s = -( p2.x() - p1.x() - t*v1.x() ) / v2.x();
  }
  else 
  {
    s = ( ( (p2.y() - p1.y())*v1.x() ) / v1.y() - p2.x() + p1.x() ) /
    	( v2.x() - ( v2.y() * v1.x() / v1.y() ) );
  	t = (p2.x() - p1.x() + s * v2.x()) / v1.x();
  }


  Point2d i = p1 + t*v1;
  /*//pour verification i2 == i
  Point2d i2 = p2 + s*v2;
  printf( "%f, %f = %f, %f\n", i.x(), i.y(), i2.x(), i2.y() ); */
  Vector3d n3d = Vector3d( v2.x(), v2.y(), 0.0 ) ^ Vector3d( 0.0, 0.0, -1.0 );
  Vector2d n( n3d.x(), n3d.y() );
  n.normalise();
  r.add( i, n );
  return r;
}

//------------------------------------------------------------------------------
Intersection2d intersect( const Line2d& l, const LineSegment2d& ls)
{
	Intersection2d r;
  Line2d l1( ls.a(), ls.b() - ls.a() );
  
  r = intersect( l, l1 );
  if( r.hasContacts() )
  {
    /*on s'assure que le point d'intersection est bien sur le segement ls.
      La projection v2 sur v1 doit etre supérieur= a 0 et la norme de v1 plus
      grande que la norme de v2.*/
    Point2d i = r.getContact(0);

		Vector2d v1( ls.a(), ls.b() ), v2( ls.a(), i );
    double pv1 = v2 * v1;
    bool n1 = v1.normSquare() >= v2.normSquare();
    if( !( pv1 >= 0 && n1 ) ) { r.clear() ; }
  }
  
  return r;
}


//------------------------------------------------------------------------------
Intersection2d intersect( const Line2d& iL, const Rectangle& iR )
{
	Intersection2d r;
  LineSegment2d ls0, ls1, ls2, ls3;
  ls0.set( iR.bottomLeft(), iR.topLeft() );
  ls1.set( iR.topLeft(), iR.topRight() );
  ls2.set( iR.topRight(), iR.bottomRight() );
  ls3.set( iR.bottomRight(), iR.bottomLeft() );
  
  r.add( intersect( iL, ls0 ) );
  r.add( intersect( iL, ls1 ) );
  r.add( intersect( iL, ls2 ) );
  r.add( intersect( iL, ls3 ) );
  
  return r;
}

//------------------------------------------------------------------------------
Intersection2d intersect( const LineSegment2d& iL1, const LineSegment2d& iL2)
{
	Intersection2d r;
  Line2d l1( iL1.a(), iL1.b() - iL1.a() ), l2( iL2.a(), iL2.b() - iL2.a() );
  
  r = intersect( l1, l2 );
  if( r.hasContacts() )
  {
  	/*on s'assure que le point d'intersection est bien sur les 2 segements.
      La projection v2 sur v1 doit etre supérieur= a 0 et la norme de v1 plus
      grande que la norme de v2.*/
    Point2d i = r.getContact(0);

		Vector2d v1( iL1.a(), iL1.b() ), v2( iL1.a(), i );
    double pv1 = v2 * v1;
    bool n1 = v1.normSquare() >= v2.normSquare();
    v1.set( iL2.a(), iL2.b() );
    v2.set( iL2.a(), i );
    double pv2 = v2 * v1;
    bool n2 = v1.normSquare() >= v2.normSquare(); 
    if( !( pv1 >= 0 && pv2 >= 0 && n1 && n2 ) ) { r.clear() ; }
  }
  return r;
}

//------------------------------------------------------------------------------
Intersection2d intersect( const LineSegment2d& iL, const Rectangle& iR)
{
	Intersection2d r;
  LineSegment2d ls0, ls1, ls2, ls3;
  ls0.set( iR.bottomLeft(), iR.topLeft() );
  ls1.set( iR.topLeft(), iR.topRight() );
  ls2.set( iR.topRight(), iR.bottomRight() );
  ls3.set( iR.bottomRight(), iR.bottomLeft() );
  
  r.add( intersect(iL, ls0) );
  r.add( intersect(iL, ls1) );
  r.add( intersect(iL, ls2) );
  r.add( intersect(iL, ls3) );
  
  return r;
}
  
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
Intersection2d intersect( const Rectangle& r, const LineSegment2d& l )
{ return intersect( l, r ); }

//------------------------------------------------------------------------------
Intersection2d intersect( const Rectangle& r1, const Rectangle& r2 )
{
	Intersection2d r;
  
	if( intersects( r1, r2 ) )
  {
  	r.add( intersect( LineSegment2d( r1.bottomLeft(), r1.topLeft() ), r2 ) );
    r.add( intersect( LineSegment2d( r1.topLeft(), r1.topRight() ), r2 ) );
    r.add( intersect( LineSegment2d( r1.topRight(), r1.bottomRight() ), r2 ) );
    r.add( intersect( LineSegment2d( r1.bottomRight(), r1.bottomLeft() ), r2 ) );
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
