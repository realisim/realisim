#include <cassert>
#include "coreLibrary, intersection.h"
#include <limits>


using namespace std;


namespace resonant
{
namespace coreLibrary
{

//------------------------------------------------------------------------------
//--- intersection
//------------------------------------------------------------------------------
intersection::intersection() : mType(tNone)
{}
//------------------------------------------------------------------------------
void intersection::addLine(const line& iL)
{ mLines.push_back(iL); }
//------------------------------------------------------------------------------
void intersection::addLineSegment(const lineSegment& iL)
{ mLineSegments.push_back(iL); }
//------------------------------------------------------------------------------
void intersection::addPlan(const plane& iP)
{ mPlanes.push_back(iP); }
//------------------------------------------------------------------------------
void intersection::addPoint(const point3& iP)
{ mPoints.push_back(iP); }
//------------------------------------------------------------------------------
void intersection::addPolygon(const polygon& iP)
{ mPolygons.push_back(iP); }
//------------------------------------------------------------------------------
void intersection::addType(type t)
{ mType |= t; }
//------------------------------------------------------------------------------
line intersection::getLine(int i) const
{ return mLines[i]; }
//------------------------------------------------------------------------------
lineSegment intersection::getLineSegment(int i) const
{ return mLineSegments[i]; }
//------------------------------------------------------------------------------
int intersection::getNumberOfPlanes() const
{ return mPlanes.size(); }
//------------------------------------------------------------------------------
int intersection::getNumberOfPoints() const
{ return mPoints.size(); }
//------------------------------------------------------------------------------
int intersection::getNumberOfPolygons() const
{ return mPolygons.size(); }
//------------------------------------------------------------------------------
int intersection::getNumberOfLines() const
{ return mLines.size(); }
//------------------------------------------------------------------------------
int intersection::getNumberOfLineSegments() const
{ return mLineSegments.size(); }
//------------------------------------------------------------------------------
plane intersection::getPlane(int i) const
{ return mPlanes[i]; }
//------------------------------------------------------------------------------
point3 intersection::getPoint(int i) const
{ return mPoints[i]; }
//------------------------------------------------------------------------------
polygon intersection::getPolygon(int i) const
{ return mPolygons[i]; }
//------------------------------------------------------------------------------
int intersection::getType() const
{ return mType; }
//------------------------------------------------------------------------------
bool intersection::hasIntersections() const
{ return getType()!= tNone || mPoints.size() || mLines.size() || 
   mLineSegments.size() || mPlanes.size() || mPolygons.size() ; }
//------------------------------------------------------------------------------
void intersection::setType(type iT)
{ mType = iT; }

//------------------------------------------------------------------------------
//--- fonctions
//------------------------------------------------------------------------------
bool areAllVerticesOnSameSide(const std::vector<point3>& vertices, const plane& p)
{
   bool r = false;
   /*On verifies si tous les points du poly sont du meme coté du plan
     Si oui, pas d'intersection, si non, intersection*/
   const vector3 planeNormal = p.getNormal();
   const point3 pointOnPlane = p.getPoint();
   bool firstPass = true;
   double projCache = 0;
   for( size_t i = 0; i < vertices.size(); ++i )
   {
      vector3 v = vertices[i] - pointOnPlane;
      double proj = v * planeNormal;
      if(!firstPass)
      {
         if( proj < 0 != projCache < 0 )
         { r = true; break; }
      }
      if(firstPass){ projCache = proj; firstPass = false;}
   }
   return r;
}
//------------------------------------------------------------------------------
bool intersects(plane p, line l)
{ return false; }
//------------------------------------------------------------------------------
bool intersects(plane p1, plane p2)
{ return false; }
//------------------------------------------------------------------------------
bool intersects(const plane& p, const box& b)
{
   vector<point3> v(8);
   const point3 pMin = b.getMin(); const point3 pMax = b.getMax();
   v[0] = pMin;
   v[1] = point3(pMax.x(), pMin.y(), pMin.z());
   v[2] = point3(pMax.x(), pMax.y(), pMin.z());
   v[3] = point3(pMin.x(), pMax.y(), pMin.z());
   v[4] = point3(pMin.x(), pMin.y(), pMax.z());
   v[5] = point3(pMax.x(), pMin.y(), pMax.z());
   v[6] = pMax;
   v[7] = point3(pMin.x(), pMax.y(), pMax.z());
   return areAllVerticesOnSameSide( v, p );
}
//------------------------------------------------------------------------------
bool intersects(const plane& p, const polygon& poly)
{
   const int c = poly.getNumberOfVertices();
   vector<point3> v(c);
   for( int i = 0; i < c; ++i )
   { v[i] = poly.getVertex(i); }
   return areAllVerticesOnSameSide(v, p );
}
//------------------------------------------------------------------------------
bool intersects(const plane& plan, const mesh2& m)
{
   for(int i = 0; i < m.getNumberOfPolygons(); ++i)
   {
      if( intersects( plan, m.getPolygon(i) ) )
         return true;
   }
   return false;
}
//------------------------------------------------------------------------------
bool intersects(line l, plane p)
{ return false; }
//------------------------------------------------------------------------------
bool intersects(const box& b, const plane& p)
{ return intersects( p, b ); }
//------------------------------------------------------------------------------
bool intersects(const polygon& poly, const plane& plan)
{ return intersects(plan, poly); }
//------------------------------------------------------------------------------
bool intersects(const mesh2& m, const plane& p)
{ return intersects(p, m); }
//------------------------------------------------------------------------------
intersection intersect(plane p, line l)
{ return intersect(l, p); }
//------------------------------------------------------------------------------
intersection intersect(plane p1, plane p2)
{
   /* il existe 3 cas
   1- les plan sont confondu... les normales sont colineaires et les coefficients
     de la forme parametrique sont des facteurs.
   2- les plan sont parallele... les normales sont colineaires et les 
     coefficients de la forme parametrique ne sont pas des facteurs.
   3- les plans sont en intersection. Il faut resoudre le systeme dequation.
   */
   intersection r;
   const double epsilon = 1e-5;
   double a1,b1,c1,d1,a2,b2,c2,d2;
   p1.parametricForm(&a1, &b1, &c1, &d1);
   p2.parametricForm(&a2, &b2, &c2, &d2);
   if( isEqualTo( p1.getNormal() * p2.getNormal(), 1.0, epsilon ) || 
       isEqualTo( p1.getNormal() * p2.getNormal(), -1.0, epsilon ) )
   {      
      double maxa,mina, maxb,minb, maxc,minc, maxd,mind;
      maxa = std::max(a1, a2); mina = std::min(a1, a2);
      maxb = std::max(b1, b2); minb = std::min(b1, b2);
      maxc = std::max(c1, c2); minc = std::min(c1, c2);
      maxd = std::max(d1, d2); mind = std::min(d1, d2);
      const bool aEquals = isEqualTo( maxa, mina, epsilon );
      const bool bEquals = isEqualTo( maxb, minb, epsilon );
      const bool cEquals = isEqualTo( maxc, minc, epsilon );
      const bool dEquals = isEqualTo( maxd, mind, epsilon );
      if( (aEquals || isEqualTo( fmod(maxa, mina), 0, epsilon )) && 
        (bEquals || isEqualTo( fmod(maxb, minb), 0, epsilon )) && 
        (cEquals || isEqualTo( fmod(maxc, minc), 0, epsilon )) && 
        (dEquals || isEqualTo( fmod(maxd, mind), 0, epsilon )) )
      { 
         r.setType(intersection::tPlane);
         r.addPlan(p1);
      }
      else
      { r.setType(intersection::tNone); }
   }
   else
   {
      vector3 lineDirection = p1.getNormal() ^ p2.getNormal();
      /*Le code qui suit est inspiré de RmPlane.cpp et 
        http://mathworld.wolfram.com/Plane-PlaneIntersection.html */

      /*En gros on resout le systeme suivant:
        n1 => normale au plan1
        n2 => normale au plan2
        ld => direction de la ligne d'intersction (n1^n2)
        d1 => distance au plan p1 (de l'origine au point le plus proche sur le plan)
        d2 => distance au plan p2 (de l'origine au point le plus proche sur le plan)

        on cherche un point v qui reside sur la ligne a l'intersection des
        deux plans.

        donc:

                 M              v        d
        | n1.x n1.y n1.z 0 | | vx |   | -d1 |
        | n2.x n2.y n2.z 0 | | vy | = | -d2 |
        | ld.x ld.y ld.z 0 | | vz |   | 0   |
        | 0    0    0    1 | | 1  |   | 1   |

        Cette équation montre que la projection du point v (sur la ligne)
        sur n1 donne d1 et que la projection de v sur n2 donne d2.

        Ainsi
          v  
        | vx |
        | vy |
        | vz | = M^-1 * d
        | 1  |
      */
      double mTemp[4][4] = { 
       {p1.getNormal().dx(), p1.getNormal().dy(), p1.getNormal().dz(), 0},
       {p2.getNormal().dx(), p2.getNormal().dy(), p2.getNormal().dz(), 0},
       {lineDirection.dx(), lineDirection.dy(), lineDirection.dz(), 0},
       {0, 0, 0, 1} };
      matrix4 m(mTemp, true);
      point3 pointOnLine = m.inverse().transform(point3(-d1, -d2, 0.0));      
      r.setType( intersection::tLine );
      r.addLine( line(pointOnLine, lineDirection) );
   }
   return r;
}
//------------------------------------------------------------------------------
intersection intersect(plane p, lineSegment ls)
{
   intersection r;
   line l(ls.getFirstPoint(), ls.getSecondPoint() - ls.getFirstPoint() );
   intersection ipl = intersect(p, l);
   if( ipl.getType() == intersection::tPoint )
   {
      /*on verifie que le point d'intersection est sur le segment de ligne.
        x est le point d'intersection
        p1 le premier point du segment
        p2 le deuxieme point du segment

        On fait la projection du point x sur le segment de ligne et.
        (x1-p1)*(p2-p1) doit etre entre 0 et 1.
      */
      const vector3 a(ipl.getPoint(0) - ls.getFirstPoint());
      const vector3 b(ls.getSecondPoint() - ls.getFirstPoint());
      double proj = (a*b)/(b*b);
      if( proj >= 0.0 && proj <= 1.0 )
      { r = ipl; }
   }
   return r;
}
//------------------------------------------------------------------------------
intersection intersect(const plane& p, const box& b)
{ return intersect(b, p); }
//------------------------------------------------------------------------------
intersection intersect(const plane& plan, const polygon& poly)
{
   intersection r;
   if( intersects(plan, poly) )
   {      
      const int n = poly.getNumberOfVertices();
      vector<lineSegment> vls(n);
      for( int i = 1; i < n; ++i )
      {
         vls[i] = lineSegment( poly.getVertex(i), poly.getVertex(i - 1 ) );
      }
      vls[0] = lineSegment( poly.getVertex(0), poly.getVertex(n-1) );

      for(size_t i = 0; i < vls.size(); ++i)
      {
         intersection x = intersect( plan, vls[i] );
         if( x.hasIntersections() )
         {
            r.addType(intersection::tPoint);
            r.addPoint( x.getPoint(0) );
         }
      }

      if( r.getNumberOfPoints() == 2 )
      {
         r.addType( intersection::tLineSegment );
         r.addLineSegment( lineSegment(r.getPoint(0), r.getPoint(1)) );
      }
   }
   return r;
}
//------------------------------------------------------------------------------
intersection intersect(const plane& p, const mesh2& m)
{
   intersection r;
   for( int i = 0; i < m.getNumberOfPolygons(); ++i )
   {
      const polygon poly = m.getPolygon(i);
      intersection x = intersect( p, poly );
      for( int j = 0; j < x.getNumberOfPoints(); ++j )
      { 
         r.addType(intersection::tPoint);
         r.addPoint( x.getPoint(j) );
      }
      if( x.getType() & intersection::tLineSegment )
      { 
         r.addType(intersection::tLineSegment);
         r.addLineSegment( x.getLineSegment(0) );
      }
   }
   
   /*On ne transforme pas l'intersection en polygon parc qu'il est
     fort possible que le polygone resultat soit concave et on ne supporte
     pas les poly concaves.*/
   return r;
}
//------------------------------------------------------------------------------
//dmnop = (xm - xn)(xo - xp) + (ym - yn)(yo - yp) + (zm - zn)(zo - zp)
/*Cette methode est utilisée par intersect(line, line)*/
double _d(const point3& m, const point3& n, const point3& o, const point3& p)
{
   return (m.x()-n.x())*(o.x()-p.x()) + 
     (m.y()-n.y())*(o.y()-p.y()) +
     (m.z()-n.z())*(o.z()-p.z());
}
//------------------------------------------------------------------------------
intersection intersect(line l1, line l2)
{   
   /*Il existe 4 cas:
     1- les lignes ne sont pas coplanaire -> pas d'intersection
     2- les lignes sont coplanaires et paralleles et ne se touchent pas. -> pas d'intersection
     3- les lignes sont coplanaires paralleles et confondues -> intersection
     4- les lignes sont coplanaires et se croisent en 1 point.*/

   intersection r;
   const vector3 a = l1.getDirection();
   const vector3 b = l2.getDirection();
   vector3 c = (l2.getPoint() ) - (l1.getPoint()  );
   
   const double kEpsilon = 1e-5;

   if(l1.getPoint().isEqualTo( l2.getPoint(), kEpsilon ))
   {
      r.setType( intersection::tPoint );
      r.addPoint( l1.getPoint() );
   }
   //test de coplanarité
   //La projection de c su a^b doit etre 0
   else if( isEqualTo( c*(a^b), 0.0, kEpsilon ) )
   {
      //parallele?
      if( isEqualTo( fabs(a*b), 1.0, kEpsilon ) )
      {
         //si un point sur l1 est sur l2 alors, elles sont confondues
         if( l1.contains(l2.getPoint()) )
         {
            r.setType( intersection::tLine );
            r.addLine(l1);
         }
      }
      else
      {
         //http://paulbourke.net/geometry/pointlineplane/
         /*
         dmnop = (xm - xn)(xo - xp) + (ym - yn)(yo - yp) + (zm - zn)(zo - zp)
         mua = ( d1343 d4321 - d1321 d4343 ) / ( d2121 d4343 - d4321 d4321 )
         mub = ( d1343 + mua d4321 ) / d4343 */
         const point3 p1(l1.getPoint()), p2(l1.getPoint()+l1.getDirection()),
            p3(l2.getPoint()), p4(l2.getPoint()+l2.getDirection());
         const double dp4321 = _d(p4,p3,p2,p1);
         const double dp4343 = _d(p4,p3,p4,p3);
         double mua = (_d(p1,p3,p4,p3)*dp4321 - _d(p1,p3,p2,p1)*dp4343 ) /
           ( _d(p2,p1,p2,p1)*dp4343 - dp4321*dp4321 );

         point3 x = l1.getPoint() + mua*l1.getDirection();
         r.setType( intersection::tPoint );
         r.addPoint( x );
      }
   }   

   return r;
}
//------------------------------------------------------------------------------
intersection intersect(line l, plane p)
{ 
   intersection r;

   /*soit un plan P = ax + by + cz + d = 0
     et une droite D sous forme parametrée:
     x = e + ht
     y = f + it
     z = g + jt

     on remplace x,y,z de la ligne dans lequation du plan
     et on resoud. 

     equation 2: a(e+ht) + b(f+it) + c(g+jt) + d = 0

     On cheche t = -ae - bf - cg - d / (ah +bi +cj)

     Il existe 3 cas:
     1- l'equation 2 n'a pas de solution -> il n'y a pas d'intersection entre
        le plan et la droite
     2- Il existe une infinité de solution -> la droite est contenu dans
        le plan.
     3- Il existe un solution unique -> la droite intersecte le plan. */

   double a,b,c,d,lx,ly,lz,lu,lv,lw, t = numeric_limits<double>::quiet_NaN() ;
   p.parametricForm(&a,&b,&c,&d);
   lx = l.getPoint().x();
   ly = l.getPoint().y();
   lz = l.getPoint().z();
   lu = l.getDirection().dx();
   lv = l.getDirection().dy();
   lw = l.getDirection().dz();

   double numerator = -a*lx - b*ly - c*lz - d;
   double denominator = a*lu + b*lv + c*lw;

   const double epsilon = 0.00001;
   if( isEqualTo(numerator, 0.0, epsilon) && isEqualTo(denominator, 0.0, epsilon)  )
   { 
      r.setType(intersection:: tLine );
      r.addLine(l);
   }
   else if( isEqualTo(denominator, 0.0, epsilon) )
   { r.setType( intersection::tNone ); }
   else
   {
      r.setType( intersection::tPoint );
      t = numerator/denominator;
      r.addPoint( l.getPoint() + l.getDirection()*t );
   }
   return r;
}
//------------------------------------------------------------------------------
intersection intersect(lineSegment ls, plane p)
{ return intersect( p, ls ); }
//------------------------------------------------------------------------------
/* //http://mathworld.wolfram.com/Line-LineIntersection.html */
intersection intersect(lineSegment ls1, lineSegment ls2)
{
   intersection r;
   const vector3 a = ls1.getSecondPoint() - ls1.getFirstPoint();
   const vector3 b = ls2.getSecondPoint() - ls2.getFirstPoint();
   //c represente le vecteur entre 2 points arbitraire des deux lignes
   vector3 c = (ls2.getFirstPoint() ) - (ls1.getFirstPoint() );
   
   const double kEpsilon = 1e-5;
   //test de coplanarité
   //La projection de c su a^b doit etre 0
   if( isEqualTo( c*(a^b), 0.0, kEpsilon ) )
   {
      //parallele?
      if( isEqualTo( fabs(a*b), 1.0, kEpsilon ) )
      {
         //si un point sur l1 est sur l2 alors, elles sont confondues en
         //parties ou en totalité...
         /*if( ls1.contains(ls2.getFirstPoint()) )
         {
         }*/
      }
      else
      {
         double s = ((c^b)*(a^b))/( pow((a^b).norm(), 2) );
         if( s >= 0 && s <= 1.0 )
         {
            point3 x = ls1.getFirstPoint() + a*s;
            {
               r.setType( intersection::tPoint );
               r.addPoint( x );
            }
         }
      }
   }
   return r;
}
//------------------------------------------------------------------------------
intersection intersect(const box& b, const plane& p)
{
   intersection r;

   //pour chaque vertices on peut avoir un point. Lorsque les points
   //sont ordonnés, il donne un polygone fermé
   /*
           p8------p7
      p4---|--p3   |
      |    |  |    |
      |    p5-|----p6
      p1------p2
   */
   const point3 pMin = b.getMin(); const point3 pMax = b.getMax();
   const point3 p1 = pMin;
   const point3 p2(pMax.x(), pMin.y(), pMin.z());
   const point3 p3(pMax.x(), pMax.y(), pMin.z());
   const point3 p4(pMin.x(), pMax.y(), pMin.z());
   const point3 p5(pMin.x(), pMin.y(), pMax.z());
   const point3 p6(pMax.x(), pMin.y(), pMax.z());
   const point3 p7 = pMax;
   const point3 p8(pMin.x(), pMax.y(), pMax.z());

   const int kNumLineSegment = 12;
   const lineSegment lsArray[kNumLineSegment] = {
      lineSegment(p1 ,p2),
      lineSegment(p2, p3),
      lineSegment(p3, p4),
      lineSegment(p4, p1),
      lineSegment(p1, p5),
      lineSegment(p2, p6),
      lineSegment(p3, p7),
      lineSegment(p4, p8),
      lineSegment(p5, p6),
      lineSegment(p6, p7),
      lineSegment(p7, p8),
      lineSegment(p8, p5) };

   std::vector<point3> vertices;
   for(int i = 0; i < kNumLineSegment; ++i)
   {
      intersection x = intersect(lsArray[i], p);
      if( x.getType() == intersection::tPoint )
      {
         r.setType( intersection::tPoint );
         r.addPoint( x.getPoint(0) );
         vertices.push_back( x.getPoint(0) );
      }
   }

   //transformer les points en polygones
   if(vertices.size())
   {
      polygon poly(vertices);
      poly.orderizeContour();
      r.addPolygon(poly);
      r.addType( intersection::tPolygon );
   }

   return r;
}

//------------------------------------------------------------------------------
intersection intersect( const mesh2& m, const plane& p)
{ return intersect(p, m); }

} //fin des namespaces...
}