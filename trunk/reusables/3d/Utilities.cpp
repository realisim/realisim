
#include "3d/Camera.h"
#include "3d/Utilities.h"
#include "math/Noise.h"
#include "math/PlatonicSolid.h"

namespace realisim 
{
  using namespace math;
namespace treeD 
{

//-----------------------------------------------------------------------------
ScreenSpaceProjection::ScreenSpaceProjection( const math::Vector2d& iS )
{
  Camera c;
  c.set( Point3d(0.0, 0.0, 5.0), 
    Point3d(0.0, 0.0, 0.0),
    Vector3d( 0.0, 1.0, 0.0 ) );
  c.setViewportSize( iS.x(), iS.y() );
  c.setProjection( 0, iS.x(), 
    0, iS.y(), 0.5, 100.0,
    Camera::Projection::tOrthogonal );
  c.pushAndApplyMatrices();
}

ScreenSpaceProjection::~ScreenSpaceProjection()
{
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();
}

//-----------------------------------------------------------------------------
//decode un QColor en id. Voir méthode idToColor
unsigned int colorToId(const QColor& iC)
{
  return iC.alpha() + iC.blue() *255 + iC.green() * 255*255 +
    iC.red() *255*255*255; 
}

//------------------------------------------------------------------------------
void draw( const PlatonicSolid& iPs )
{
  for( int i = 0; i < iPs.getNumberOfPolygons(); ++i )
  {
    const Polygon& p = iPs.getPolygon( i );
    glBegin(GL_POLYGON);
    glNormal3dv( p.getNormal().getPtr() );
    for( int j = 0; j < p.getNumberOfVertices(); ++j )
    { glVertex3dv( p.getVertex( j ).getPtr() ); }
    glEnd();
  }
}

//------------------------------------------------------------------------------
/*Dessine un cercle sur le plan défini par la normal iN centré sur iP de 
  rayon iR*/
void drawCircle( Vector3d iN, const Point3d& iP, double iR )
{
	//on trouve l'angle entre iN et Z (parce qu'on dessine en sur le plan z)
  double rot = acos( iN.normalise() * Vector3d( 0.0, 0.0, 1.0 ) );
  //on trouve le vecteur perpendiculaire a la rotation
  Vector3d r = iN ^ Vector3d( 0.0, 0.0, 1.0 );
  r.normalise();

  double a;
  glPushMatrix();
  glTranslated( iP.getX(), iP.getY(), iP.getZ() );
  glRotated( rot * 180 / PI, r.getX(), r.getY(), r.getZ() );
	glBegin( GL_LINE_LOOP );
  	for( int i = 0; i < 360; i += 2 )
    {
    	a = i * 3.1415629 / 180.0;
    	glVertex3d( cos( a ) * iR, sin( a ) * iR, 0.0 );	
    }
  glEnd();
  glPopMatrix();
}

//------------------------------------------------------------------------------
/*dessine un cercle de rayon iR centré en iP sur le plan xy*/
void drawCircle( const Point2d& iP, double iR )
{ drawCircle( Vector3d( 0.0, 0.0, 1.0 ), Point3d( iP.x(), iP.y(), 0.0 ), iR ); }

//------------------------------------------------------------------------------
/*dessine un rectangle dans le plan xy.*/
void drawLine( const Point2d& ip1, const Point2d& ip2 )
{
	glBegin( GL_LINES );
  glVertex2dv( ip1.getPtr() ); glVertex2dv( ip2.getPtr() );
  glEnd();
}

//------------------------------------------------------------------------------
void drawPoint(const Point2d& iP, double iPointSize /*= 1.0*/)
{
	glPointSize( iPointSize );
  glBegin(GL_POINTS);
  glVertex2dv( iP.getPtr() );
  glEnd();
}
  
//------------------------------------------------------------------------------
/*dessine un rectangle dans le plan xy.*/
void drawRectangle( const Rectangle& iR )
{
  glBegin(GL_QUADS);
  glVertex2dv( iR.bottomLeft().getPtr() );
  glVertex2dv( iR.topLeft().getPtr() );
  glVertex2dv( iR.topRight().getPtr() );
  glVertex2dv( iR.bottomRight().getPtr() );
  glEnd();
}


//------------------------------------------------------------------------------
/*dessine un rectangle dans le plan xy. origine iO et taille iS*/
void drawRectangle( const Point2d& iO, const Vector2d& iS)
{ drawRectangle( Rectangle( iO, iS ) ); }

//------------------------------------------------------------------------------
/*dessine un rectangle dans le plan xy. origine iO et taille iS*/
void drawRectangle( const Texture& iT, const Point2d& iO, const Vector2d& iS )
{
	glEnable( GL_TEXTURE_2D );
  glBindTexture( GL_TEXTURE_2D, iT.getId() );
  glBegin(GL_QUADS);
  glTexCoord2d( 0.0, 0.0 );
  glVertex2d( iO.x(), iO.y() );
  glTexCoord2d( 0.0, 1.0 );
  glVertex2d( iO.x(), iO.y() + iS.y() );
  glTexCoord2d( 1.0, 1.0 );
  glVertex2d( iO.x() + iS.x(), iO.y() + iS.y() );
  glTexCoord2d( 1.0, 0.0 );
  glVertex2d( iO.x() + iS.x(), iO.y() );
  glEnd();
	glDisable( GL_TEXTURE_2D );
}

//------------------------------------------------------------------------------
void drawRectangularPrism( const Point3d& iLowerLeft, const Point3d& iTopRight )
{
	Point3d minCorner = iLowerLeft;
	Point3d maxCorner = iTopRight;

  glBegin(GL_QUADS);
  
  //cote X
  glNormal3d( 1.0, 0.0, 0.0 );
  glVertex3d(maxCorner.getX(), maxCorner.getY(), maxCorner.getZ());
  glVertex3d(maxCorner.getX(), minCorner.getY(), maxCorner.getZ());
  glVertex3d(maxCorner.getX(), minCorner.getY(), minCorner.getZ());
  glVertex3d(maxCorner.getX(), maxCorner.getY(), minCorner.getZ());    
  
  //cote -X
  glNormal3d( -1.0, 0.0, 0.0 );
  glVertex3d(minCorner.getX(), minCorner.getY(), minCorner.getZ());
  glVertex3d(minCorner.getX(), minCorner.getY(), maxCorner.getZ());
  glVertex3d(minCorner.getX(), maxCorner.getY(), maxCorner.getZ());
  glVertex3d(minCorner.getX(), maxCorner.getY(), minCorner.getZ());
  
  //cote -Z
  glNormal3d( 0.0, 0.0, -1.0 );
  glVertex3d(minCorner.getX(), minCorner.getY(), minCorner.getZ());
  glVertex3d(minCorner.getX(), maxCorner.getY(), minCorner.getZ());
  glVertex3d(maxCorner.getX(), maxCorner.getY(), minCorner.getZ());
  glVertex3d(maxCorner.getX(), minCorner.getY(), minCorner.getZ());    
  
  //cote Z
  glNormal3d( 0.0, 0.0, 1.0 );
  glVertex3d(maxCorner.getX(), maxCorner.getY(), maxCorner.getZ());
  glVertex3d(minCorner.getX(), maxCorner.getY(), maxCorner.getZ());
  glVertex3d(minCorner.getX(), minCorner.getY(), maxCorner.getZ());
  glVertex3d(maxCorner.getX(), minCorner.getY(), maxCorner.getZ());
  
  //cote Y
  glNormal3d( 0.0, 1.0, 0.0 );
  glVertex3d(maxCorner.getX(), maxCorner.getY(), maxCorner.getZ());
  glVertex3d(maxCorner.getX(), maxCorner.getY(), minCorner.getZ());
  glVertex3d(minCorner.getX(), maxCorner.getY(), minCorner.getZ());
  glVertex3d(minCorner.getX(), maxCorner.getY(), maxCorner.getZ());
  
  //cote -Y
  glNormal3d( 0.0, -1.0, 0.0 );
  glVertex3d(minCorner.getX(), minCorner.getY(), minCorner.getZ());
  glVertex3d(maxCorner.getX(), minCorner.getY(), minCorner.getZ());
  glVertex3d(maxCorner.getX(), minCorner.getY(), maxCorner.getZ());
  glVertex3d(minCorner.getX(), minCorner.getY(), maxCorner.getZ());
  
  glEnd();
}


//------------------------------------------------------------------------------
Texture get2dNoiseTexture(int iWidth, int iHeight)
{
  return Texture();
}

//------------------------------------------------------------------------------
/*Algorithme pour générer un texture3d de noise pris dans le livre:
  OpenGL Shading Language third edition par Randi J. Rost et Bill Licea-Kane.
  Chapitre 15.2 Noise Textures.*/
Texture get3dNoiseTexture(const Vector3i& iSize)
{
  Texture r;
  int f, i, j, k, inc;
  int startFrequency = 4;
  int numOctaves = 4;
  double ni[3];
  double inci, incj, inck;
  int frequency = startFrequency;
  GLubyte* ptr;
  GLubyte* noise3dTexPtr;
  double amp = 0.5;
  
  noise3dTexPtr = (GLubyte*) malloc(iSize.getX() *
    iSize.getY() * iSize.getZ() * 4);
  if(noise3dTexPtr == NULL)
    return Texture();  
  
  for(f = 0, inc = 0; f < numOctaves; ++f, frequency *= 2, ++inc, amp *= 0.5)
  {
    noise::setNoiseFrequency(frequency);
    ptr = noise3dTexPtr;
    ni[0] = ni[1] = ni[2] = 0; 
    
    inci = 1.0 / (iSize.getX() / frequency);
    for(i = 0; i < iSize.getX(); ++i, ni[0] += inci)
    {
      incj = 1.0 / (iSize.getY() / frequency);
      for(j = 0; j < iSize.getY(); ++j, ni[1] += incj)
      {
        inck = 1.0 / (iSize.getZ() / frequency);
        for(k = 0; k < iSize.getZ(); ++k, ni[2] += inck, ptr += 4)
        {
          *(ptr+inc) = (GLubyte)(((noise::noise3(ni)+1.0) * amp)*128.0);
        }
      }
    }
  }

	vector<int> s; s.resize(3);
  s[0] = iSize.getX(); s[1] = iSize.getY(); s[2] = iSize.getZ();
  r.set( noise3dTexPtr, s );
  free(noise3dTexPtr);
  return r;
}

//-----------------------------------------------------------------------------
/*encode un id 32 bits en Qcolor pour faciliter le colorPicking (voir
  méthode widget3d::pick() )*/
QColor idToColor(unsigned int iId)
{
  int r,g,b,a;
  r = g = b = a = 0;
  int remaining = iId / 255;
  a = iId % 255;
  b = remaining % 255;
  remaining /= 255;
  g = remaining % 255;
  remaining /= 255;
  r = remaining % 255;
  
  return QColor(r, g, b, a);
}


}
}
