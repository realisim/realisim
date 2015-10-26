#define _USE_MATH_DEFINES //pour utilisation M_PI
#include <cmath>
#include <algorithm>
#include "coreLibrary, primitives.h"
#include <limits>
#include <map>
#include <QStringList>
#include "utilities.h"

using namespace std;
using namespace resonant;
   using namespace coreLibrary;

namespace
{
   double kDMax = numeric_limits<double>::max();
}

//-----------------------------------------------------------------------------
// line
//-----------------------------------------------------------------------------
line::line() : mIsValid(false), mPoint(0.0, 0.0, 0.0), mDirection(0.0, 0.0, 0.0)
{}
//-----------------------------------------------------------------------------
line::line(point3 p1, point3 p2)
{ set(p1, p2); }
//-----------------------------------------------------------------------------
line::line(point3 p, vector3 v) : mPoint(p), mDirection(v)
{ validate(); }
//-----------------------------------------------------------------------------
bool line::contains(point3 p) const
{
   bool isOnLine = false;
   const double kEpsilon = 1e-5;
   /*de léquation parametrique suivante:
     x = x0 + at 
     y = y0 + bt
     z = z0 + ct

     ou (x0, y0, z0) représente un point sur la ligne et (a, b, c) le
     vecteur parallele à la ligne.
     On peut déduire:
     (x-x0)/a = (y-y0)/b = (z-z0)/c

     si l'équalité est satisfaite pour (x,y,z) alors le point est sur la ligne
   */
   const double rx = (p.x() - getPoint().x())/getDirection().dx();
   const double ry = (p.y() - getPoint().y())/getDirection().dy();
   const double rz = (p.z() - getPoint().z())/getDirection().dz();
   isOnLine = isEqualTo( rx, ry, kEpsilon ) &&
      isEqualTo( ry, rz, kEpsilon );
   return isOnLine;
}
//-----------------------------------------------------------------------------
point3 line::getPoint() const
{ return mPoint; }
//-----------------------------------------------------------------------------
vector3 line::getDirection() const
{ return mDirection; }
//-----------------------------------------------------------------------------
bool line::isValid() const
{ return mIsValid; }
//-----------------------------------------------------------------------------
void line::set(point3 p1, point3 p2)
{
   vector3 v = p2 - p1;
   set(p1, v);
}
//-----------------------------------------------------------------------------
void line::set(point3 p, vector3 v)
{ 
   mPoint = p;
   mDirection = v;
   validate();
}
//-----------------------------------------------------------------------------
void line::validate()
{ 
   mIsValid = !mDirection.isEqualTo( vector3(0.0, 0.0, 0.0), 1e-13 );
   if( mIsValid ) mDirection.normalize();
}
//-----------------------------------------------------------------------------
// lineSegment
//-----------------------------------------------------------------------------
lineSegment::lineSegment() : mIsValid(false), mPoint1(), mPoint2()
{}
//-----------------------------------------------------------------------------
lineSegment::lineSegment(point3 p1, point3 p2)
{ set(p1, p2); }
//-----------------------------------------------------------------------------
point3 lineSegment::getFirstPoint() const
{ return mPoint1; }
//-----------------------------------------------------------------------------
point3 lineSegment::getSecondPoint() const
{ return mPoint2; }
//-----------------------------------------------------------------------------
bool lineSegment::isValid() const
{ return mIsValid; }
//-----------------------------------------------------------------------------
void lineSegment::set(point3 p1, point3 p2)
{ mPoint1 = p1; mPoint2 = p2; validate(); }
//-----------------------------------------------------------------------------
void lineSegment::validate()
{
   const double kEpsilon = 1e-10;
   mIsValid = mPoint1.isEqualTo(mPoint2, kEpsilon);
}
//-----------------------------------------------------------------------------
// plane
//-----------------------------------------------------------------------------
plane::plane() : mIsValid(false), mPoint(0.0, 0.0, 0.0), mNormal(0.0, 0.0, 0.0)
{}
//-----------------------------------------------------------------------------
/*la normale au plan suit la convention main droite.*/
plane::plane(point3 p1, point3 p2, point3 p3)
{ set( p1, p2, p3 ); }
//-----------------------------------------------------------------------------
plane::plane(point3 p, vector3 n) : 
   mPoint(p), mNormal(n)
{ validate(); }
//-----------------------------------------------------------------------------
point3 plane::getPoint() const
{ return mPoint; }
//-----------------------------------------------------------------------------
vector3 plane::getNormal() const
{ return mNormal; }
//-----------------------------------------------------------------------------
bool plane::isValid() const
{ return mIsValid; }
//-----------------------------------------------------------------------------
/*retourne la forme parametrique du plan ax + by + cz + d = 0*/
void plane::parametricForm(double* a, double* b, double* c, double* d) const
{
   *a = mNormal.dx();
   *b = mNormal.dy();
   *c = mNormal.dz();
   /*Pour trouver d, il faut résoudre: ax + by + cz + d = 0
   Nous avons déja un point sur le plan mPoint et nous connaissons la 
   normale... donc
   -d = ax + by + cz */
   *d = -(mNormal.dx()*mPoint.x() + 
          mNormal.dy()*mPoint.y() + 
          mNormal.dz()*mPoint.z() );
}
//-----------------------------------------------------------------------------
void plane::set(point3 p1, point3 p2, point3 p3)
{
   vector3 v1 = p2 - p1;
   vector3 v2 = p3 - p1;
   set( p1, v1 ^ v2 );
}
//-----------------------------------------------------------------------------
void plane::set(point3 p, vector3 v)
{
   mPoint = p;
   mNormal = v;
   validate();
}
//-----------------------------------------------------------------------------
void plane::validate()
{   
   mIsValid = !mNormal.isEqualTo( vector3(0.0, 0.0, 0.0), 1e-13 );
   if(mIsValid)
   { mNormal.normalize(); }
}
//-----------------------------------------------------------------------------
// polygon
//-----------------------------------------------------------------------------
polygon::polygon() : mIsValid(false)
{}
//-----------------------------------------------------------------------------
polygon::polygon(point3 a, point3 b, point3 c) : mIsValid(false)
{
   mVertices.push_back(a);
   mVertices.push_back(b);
   mVertices.push_back(c);
   validate();
}
//-----------------------------------------------------------------------------
polygon::polygon(const std::vector<point3>& iPoints) : mIsValid(false)
{ mVertices = iPoints; validate(); }
//-----------------------------------------------------------------------------
point3 polygon::getCenterOfMass() const
{
   double x = 0, y = 0, z = 0;
   const int count = getNumberOfVertices();
   for( int i = 0; i < count; ++i )
   {
      const point3 p = getVertex(i);
      x += p.x();
      y += p.y();
      z += p.z();
   }   
   return point3( x/(double)count, y/(double)count, z/(double)count);
}
//-----------------------------------------------------------------------------
vector3 polygon::getNormal() const
{ return mNormal; }
//-----------------------------------------------------------------------------
int polygon::getNumberOfVertices() const
{return mVertices.size() ;}
//-----------------------------------------------------------------------------
point3 polygon::getVertex(int i) const
{return mVertices[i];}
//-----------------------------------------------------------------------------
/*On suppose qu'il y a au moins 3 vertex.*/
bool polygon::isCoplanar() const
{
   bool r = true;
   const point3 cm = getCenterOfMass();
   const vector3 x(getVertex(0) - cm);
   vector3 n0( x ^ vector3(getVertex(1) - cm) );
   const double kEpsilon = 1e-5;

   for(int i = 0; i < getNumberOfVertices(); ++i)
   {
      //la projection du vecteur (getVertex(i) - cm) sur la normale n0
      //doit etre 0.
      if( !isEqualTo( (getVertex(i) - cm) * n0, 0.0, kEpsilon ) )
      {
         r = false;
         break;
      }
   }
   return r;
}
//-----------------------------------------------------------------------------
bool polygon::isValid() const
{ return mIsValid; }
//-----------------------------------------------------------------------------
//On suppose que les points sont coplanaires
void polygon::orderizeContour()
{
   point3 centerOfMass = getCenterOfMass();
   const vector3 v0 = getVertex(0) - centerOfMass;
   const double v0Norm = v0.norm();
   /*On calcul une normal qu'on utilisera comme guide afin de determiner
     les angles qui dépasse PI.*/
   const vector3 normal = v0 ^ (getVertex(1) - centerOfMass);
   
   //faire un functor serait plus performant ici... nlogn au lieu de 2n et
   //plain de memoire.
   vector3 v1;
   double cosAngle, angle;
   const double kEpsilon = 1e-5;
   std::map<double, point3> ordered;
   ordered[0] = getVertex(0);
   for( int i = 1; i < getNumberOfVertices(); ++i )
   {
      v1 = (getVertex(i) - centerOfMass);
      cosAngle = v0 * v1 / (v0Norm * v1.norm());

      /*On borne le cosAngle a -1.0 -> 1.0*/
      cosAngle = max(-1.0, cosAngle);
      cosAngle = min(1.0, cosAngle);
      angle = acos(cosAngle);

      /*si la projection de (v0 ^ v1) sur la normal est plus petite que 0,
        alors on a passé PI et il faut soustraire l'angle a 2*PI.*/
      if( (v0 ^ v1) * normal < kEpsilon )
      { angle = 2*M_PI - angle; }
      ordered[angle] = getVertex(i);
   }

   mVertices.clear();
   auto it = ordered.begin();
   while(it != ordered.end())
   {
      mVertices.push_back(it->second);
      ++it;
   }
}
//-----------------------------------------------------------------------------
void polygon::validate()
{
   bool clearAll = false;
   if( getNumberOfVertices() < 3)
   { clearAll = true; }
   else if (getNumberOfVertices() == 3)
   {
      mNormal = vector3(getVertex(1) - getVertex(0)) ^
         vector3(getVertex(2) - getVertex(0));
      mNormal.normalize();
      mIsValid = true;
   }
   else
   {
      mNormal = vector3(getVertex(1) - getVertex(0)) ^
         vector3(getVertex(2) - getVertex(0));
      mNormal.normalize();
      mIsValid = true;
   }

   if(clearAll)
   {
      mIsValid = false;
      mNormal = vector3();
      mVertices.clear();
   }
}
//-----------------------------------------------------------------------------
//--- mesh
//-----------------------------------------------------------------------------
mesh2::mesh2()
{}
mesh2::~mesh2()
{}
//-----------------------------------------------------------------------------
int mesh2::getNumberOfFaces() const
{ return mFaces.size(); }
//-----------------------------------------------------------------------------
int mesh2::getNumberOfNormals() const
{ return mNormals.size(); }
//-----------------------------------------------------------------------------
int mesh2::getNumberOfPolygons() const
{ return getNumberOfFaces(); }
//-----------------------------------------------------------------------------
int mesh2::getNumberOfVertices() const
{ return mVertices.size(); }
//-----------------------------------------------------------------------------
int mesh2::getNumberOfNormalsOnFace(int i) const
{ return mFaces[i].mNormalIndices.size(); }
//-----------------------------------------------------------------------------
int mesh2::getNumberOfVerticesOnFace(int i) const
{ return mFaces[i].mVertexIndices.size(); }
//-----------------------------------------------------------------------------
int mesh2::getVertexIndexOnFace(int i, int face) const
{ return mFaces[face].mVertexIndices[i]; }
//-----------------------------------------------------------------------------
int mesh2::getNormalIndexOnFace(int i, int face) const
{ return mFaces[face].mNormalIndices[i]; }
//-----------------------------------------------------------------------------
vector3 mesh2::getNormal(int i) const
{ return mNormals[i]; }
//-----------------------------------------------------------------------------
const double* mesh2::getPointerToNormals() const
{
   double* r = 0;
   if(mNormals.size()) { r = (double*)&mNormals[0]; } 
   return r;
}
//-----------------------------------------------------------------------------
const double* mesh2::getPointerToVertices() const
{
   double* r = 0;
   if(mVertices.size()) { r = (double*)&mVertices[0]; } 
   return r;
}
//-----------------------------------------------------------------------------
polygon mesh2::getPolygon(int iIndex) const
{
   std::vector<point3> vp;
   for( int i = 0; i < getNumberOfVerticesOnFace(iIndex); ++i )
   { vp.push_back( getVertex( getVertexIndexOnFace(i, iIndex) ) ); }

   return polygon(vp);
}
//-----------------------------------------------------------------------------
point3 mesh2::getVertex(int i) const
{ return mVertices[i]; }
//-----------------------------------------------------------------------------
/*Support obj minimal... juste 1 group, pas de materiaux, pas de texture.*/
mesh2* mesh2::makeFromObj( QString iFileName )
{   
   mesh2 *m = new mesh2();

   QString content( resonant::utilities::read( iFileName ) );
   if( !content.isEmpty() )
   {
      QStringList lines = content.split("\n", QString::SkipEmptyParts);
      for( int i = 0; i < lines.size(); ++i )
      {
//printf("%s\n", lines[i].toStdString().c_str());
         QStringList tokens = lines[i].split(" ", QString::SkipEmptyParts);         
         if( tokens[0] == "v" ) //vertex
         {
            m->mVertices.push_back( 
               point3( tokens[1].toDouble(),
                  tokens[2].toDouble(), tokens[3].toDouble() ) );
         }
         else if( tokens[0] == "vn" )
         {
            m->mNormals.push_back( 
               vector3( tokens[1].toDouble(),
               tokens[2].toDouble(), tokens[3].toDouble() ) );
         }
         /*else if( tokens[0] == "vt" )
         {
            mTextureCoordinates.push_back( 
               point3( tokens[1].toDouble(),
               tokens[2].toDouble(), tokens[3].toDouble() ) );
         }*/
         else if( tokens[0] == "f")
         {
            face f;
            for( int j = 1; j < tokens.size(); ++j )
            {   
//printf("%s\n", tokens[j].toStdString().c_str());
               /*A noter que le format waveFront note les indices de 1 à n
                 alors qu'en informatique, les indices vont de 0 a n-1. C'est
                 pourquoi, on fait -1 sur tous les indices.*/
               QStringList indices = tokens[j].split("/");
               if( indices.size() == 1 )
               { f.mVertexIndices.push_back( indices[0].toInt() - 1 ); }
               else if (indices.size() == 2) //vertex + texture coordinate
               {}
               else //3 //vertex + texture + normal
               {
                  f.mVertexIndices.push_back( indices[0].toInt() - 1 );
                  f.mNormalIndices.push_back( indices[2].toInt() - 1 );
               }
            }
            m->mFaces.push_back(f);         
         }
         else{}
      }
   }
   return m;
}
//-----------------------------------------------------------------------------
//--- box
//-----------------------------------------------------------------------------
box::box() : mMin(kDMax, kDMax, kDMax),
   mMax(-kDMax, -kDMax, -kDMax)
{}
//-----------------------------------------------------------------------------
box::box(point3 iMin, point3 iMax) : mMin(kDMax, kDMax, kDMax),
   mMax(-kDMax, -kDMax, -kDMax)
{ add(iMin); add(iMax); }
//-----------------------------------------------------------------------------
box::box(point3 iMin, vector3 iSize) : mMin(kDMax, kDMax, kDMax),
   mMax(-kDMax, -kDMax, -kDMax)
{ add(iMin); add(iMin + iSize); }
//-----------------------------------------------------------------------------
void box::add(point3 iP)
{
   mMin.setX( min( mMin.x(), iP.x() ) );
   mMin.setY( min( mMin.y(), iP.y() ) );
   mMin.setZ( min( mMin.z(), iP.z() ) );

   mMax.setX( max( mMax.x(), iP.x() ) );
   mMax.setY( max( mMax.y(), iP.y() ) );
   mMax.setZ( max( mMax.z(), iP.z() ) );
}
//-----------------------------------------------------------------------------
point3 box::getCenterOfMass() const
{ 
   return point3( (mMax.x() + mMin.x()) / 2.0,
      (mMax.y() + mMin.y()) / 2.0,
      (mMax.z() + mMin.z()) / 2.0);
}
//-----------------------------------------------------------------------------
point3 box::getMax() const
{ return mMax; }
//-----------------------------------------------------------------------------
point3 box::getMin() const
{ return mMin; }
//-----------------------------------------------------------------------------
vector3 box::getSize() const
{ return mMax - mMin; }
//-----------------------------------------------------------------------------
bool box::isValid() const
{ 
   return !mMax.isEqualTo(point3(-kDMax,-kDMax,-kDMax), 0.1) && 
      !mMin.isEqualTo(point3(kDMax,kDMax,kDMax), 0.1);
}
//-----------------------------------------------------------------------------
void box::setMax(point3 iP)
{ mMax = iP; }
//-----------------------------------------------------------------------------
void box::setMin(point3 iP)
{ mMin = iP; }
//-----------------------------------------------------------------------------
bool box::contains(point3 iP, bool iInclusive) const
{
   bool r = false;
   if(iInclusive)
   { 
      r = iP.x() >= mMin.x() && iP.y() >= mMin.y() && iP.z() >= mMin.z() &&
         iP.x() <= mMax.x() && iP.y() <= mMax.y() && iP.z() <= mMax.z();
   }
   else
   {
      r = iP.x() > mMin.x() && iP.y() > mMin.y() && iP.z() > mMin.z() &&
         iP.x() < mMax.x() && iP.y() < mMax.y() && iP.z() < mMax.z();
   }
   return r;
}
//-----------------------------------------------------------------------------
bool box::contains(box iB, bool iInclusive) const
{
   bool r = false;
   if(iInclusive)
   { r = contains(iB.getMin(), true) && contains(iB.getMax(), true ); }
   else
   { r = contains( iB.getMin() ) || contains(iB.getMax()); }
   return r;
}