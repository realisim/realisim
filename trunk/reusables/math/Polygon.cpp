
#include <algorithm>
#include "math/MathUtils.h"
#include "Polygon.h"
#include <vector>

using namespace std;
using namespace realisim;
	using namespace math;

Polygon::Polygon() : mVertices(),
	mNormal(),
  mIsCoplanar( false ),
  mIsConvex( false ),
  mIsValid( false ),
  mAreVerticesOwned( true )
{}

Polygon::Polygon( const Polygon& iP ) :
	mVertices(),
  mNormal( iP.getNormal() ),
  mIsCoplanar( iP.isCoplanar() ),
  mIsConvex( iP.isConvex() ),
  mIsValid( iP.isValid() ),
  mAreVerticesOwned( true )
{
	//deep copy
  mVertices.resize( iP.getNumberOfVertices() );
  for( int i = 0; i < iP.getNumberOfVertices(); ++i )
  	mVertices[i] = new Point3d( iP.getVertex( i ) );
  validate();
}

Polygon::Polygon( const std::vector< Point3d >& iV ) :
	mVertices(),
  mNormal( 0.0 ),
  mIsCoplanar( false ),
  mIsConvex( false ),
  mIsValid( false ),
  mAreVerticesOwned( true )
{
	mVertices.reserve( iV.size() );
//  addVertices( iV );
	for( int i = 0; i < (int)iV.size(); ++i )
  { mVertices.push_back( new Point3d( iV[i] ) ); }
  validate();
}

Polygon::Polygon( const std::vector< Point3d* >& iV ) :
	mVertices(),
  mNormal( 0.0 ),
  mIsCoplanar( false ),
  mIsConvex( false ),
  mIsValid( false ),
  mAreVerticesOwned( false )
{
	mVertices.reserve( iV.size() );
  //addVertices( iV );
  for( int i = 0; i < (int)iV.size(); ++i )
  { mVertices.push_back( iV[i] ); }
  validate();
}

Polygon::~Polygon()
{
	for( int i = 0; mAreVerticesOwned && i < getNumberOfVertices(); ++i )
  	delete mVertices[i];
  mVertices.clear();
}
//-----------------------------------------------------------------------------
void Polygon::checkIfConvex() const
{
	mIsConvex = getNumberOfVertices() >= 3;
  if( getNumberOfVertices() > 3 )
  {
  	vector<double> angles;
    /*on fabrique un systeme de coordonnée orthonormal qui a sont axe Z 
      parallele a l'axe normal du polygon et son origine sur un vertex. Ainsi,
      on peut transformer les coordonnées des vertices 3d en position 2d ( x,y sur
      le plan du polygon) et ainsi faciliter le travail.*/
      
    Vector3d z = getNormal();
    Vector3d x = getVertex( 0 ) - getCentroid();
    x.normalise();
    Vector3d y = z ^ x;
    
    /*Pour chaque vertex du polygon, on déplace le systeme de coordonné m
      sur ce vertex. On met le vertex suivant dans ce systeme de coordonné et
      on trouve l'angle de ce vertex par rapport a l'origine du systeme de
      coordonné (donc par rapport au premier vertex). On place dans les vecteur
      angles les résultats.*/
    Point3d p;
    double r, cosA, sinA, theta;
    for( int i = 0; i < getNumberOfVertices(); ++i )
    {
    	int nextIndex = ( i + 1 ) % getNumberOfVertices();
      myMatrix4 m1(x,y,z);
			m1.setTranslation( toVector( getVertex( i ) ) );
      m1.invert();
      p = m1 * getVertex( nextIndex );

      r = toVector( p ).norm();
      cosA = acos( p.x() / r );
      sinA = asin( p.y() / r );
      theta = sinA < 0.0 ? DEUX_PI - cosA : cosA;
      angles.push_back( theta );
      
//Point3d _d = m1 * getVertex( i );
//printf("\n-----------p%d; %f, %f, %f\n", i, _d.x(), _d.y(), _d.z() );      
//printf("p%d; %f, %f, %f\n", nextIndex, p.x(), p.y(), p.z() );
//printf("cosA %d-%d; %f\n", i, nextIndex, cosA );
//printf("sinA %d-%d; %f\n", i, nextIndex, sinA );
//printf("theta %d-%d; %f\n", i, nextIndex, theta * 180 / PI);
    }
    
    /*On trouve l'index du plus petit angle*/
    int smallestAngleIndex = 0;
    double smallestAngle = numeric_limits<double>::max();
    for( int i = 0; i < (int) angles.size(); ++i )
    { 
      if( angles[i] < smallestAngle )
      { smallestAngle = angles[i]; smallestAngleIndex = i; }
    }
    
    /*On parcourt tous les angles en commencant par le plus petit. Pour que le
    polygone soit convex, les angles doivent toujours croitre, sinon le polygone
    n'est pas convexe.*/
    for( int i = 0; i < (int) angles.size() - 1; ++i )
    {
    	int index = (smallestAngleIndex + i) % angles.size();
      int nextIndex = (index + 1) % angles.size();
      if( angles[ nextIndex ] < angles [ index ] )
      { mIsConvex = false; break; }
    }
  }
}
//-----------------------------------------------------------------------------
void Polygon::checkIfCoplanar() const
{
	mIsCoplanar = getNumberOfVertices() == 3;
  if( getNumberOfVertices() > 3 )
	{
  	mIsCoplanar = math::isCoplanar( mVertices, 0.000001 );
//    //on dirait que le code qui suit est redondant... a tester et enlever.
//    if( !mIsCoplanar )
//    	math::isCoplanar( mVertices, 0.0001 );
  }
}
//-----------------------------------------------------------------------------
double Polygon::getArea() const
{
	double r = 0.0;
  if( isConvex() )
  {
  	/* la formule est pour un polygone 2d. Je crois quen trouve la normale
    	au polygone et le cenre de masse, je pourrais faire un systeme de
      coordonnée local ou le polygone serait en x,y et appliquer la formule. */
//  	int i0 = 0; i1 = 0;
//    double x, y;
//  	for( int i = 0; i < getNumberOfVertices(); ++i )
//    {
//    	i0 = i; i1 = ( i + 1 ) % getNumberOfVertices();    	
//    }
  }
  return r;
}
//-----------------------------------------------------------------------------
Point3d Polygon::getCentroid() const
{
	double cx = 0.0, cy = 0.0, cz = 0.0;
  for( int i = 0; i < getNumberOfVertices(); ++i )
  {
  	cx += getVertex( i ).x();
    cy += getVertex( i ).y();
    cz += getVertex( i ).z();
  }
  cx /= (double)getNumberOfVertices();
  cy /= (double)getNumberOfVertices();
  cz /= (double)getNumberOfVertices();
  return Point3d( cx, cy, cz );
}
//-----------------------------------------------------------------------------
const Vector3d& Polygon::getNormal() const
{	
	if( !isValid() ) validate();
	return mNormal;
}
//-----------------------------------------------------------------------------
int Polygon::getNumberOfVertices() const
{ return mVertices.size(); }
//-----------------------------------------------------------------------------
/*Retourne le vertex à l'index i, si l'index est négatif, on retourne a partir
  de la fin.*/
Point3d& Polygon::getVertex( int i )
{
	return const_cast<Point3d&>(
    const_cast<const Polygon*>(this)->getVertex( i ) );
}
//-----------------------------------------------------------------------------
const Point3d& Polygon::getVertex( int i ) const
{
  if( i < 0 )
  { i = mVertices.size() + i; }
  i = i % mVertices.size();  
	return *(mVertices[ i ]);
}
//-----------------------------------------------------------------------------
void Polygon::invalidate()
{ mIsValid = false; }
//-----------------------------------------------------------------------------
bool Polygon::isConvex() const
{
	if( !isValid() ) validate();
  return mIsConvex;
}
//-----------------------------------------------------------------------------
bool Polygon::isCoplanar() const
{
	if( !isValid() ) validate();
  return mIsCoplanar;
}
//-----------------------------------------------------------------------------
bool Polygon::operator==( const Polygon& iP ) const
{ return true; }
//-----------------------------------------------------------------------------
Polygon& Polygon::operator= ( const Polygon& iP )
{
	//deep copy
  mVertices.resize( iP.getNumberOfVertices() );
  for( int i = 0; i < iP.getNumberOfVertices(); ++i )
  	mVertices[i] = new Point3d( iP.getVertex( i ) );
  mNormal = iP.getNormal();
  mIsCoplanar = iP.isCoplanar();
  mIsConvex = iP.isConvex();
	return *this;
}
//-----------------------------------------------------------------------------
void Polygon::validate() const
{
	if( !isValid() )
  {
  	/*Attention, l'ordre ici est important...
    1- La normale est dépendante de la coplanarité.
    2- Pour determiner si le poly est convex on a besoin de la normale. */
  	mIsValid = true;
    checkIfCoplanar();
    if( isCoplanar() )
    {
      mNormal = Vector3d( getVertex( 0 ), getVertex( 1 ) ) ^
        Vector3d( getVertex( 1 ), getVertex( 2 ) );
      mNormal.normalise();
    }
    else mNormal = Vector3d( 0.0 );
    checkIfConvex();    
  }
}
