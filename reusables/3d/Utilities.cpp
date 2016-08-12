
#include "3d/Camera.h"
#include "3d/Utilities.h"
#include "3d/VertexBufferObject.h"
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
  glTranslated( iP.x(), iP.y(), iP.z() );
  glRotated( rot * 180 / PI, r.x(), r.y(), r.z() );
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
/*dessine un rectangle dans le plan xy en CCW.*/
void drawRectangle( const Rectangle& iR )
{
    VertexBufferObject vbo;

    float v[12] = {
        iR.bottomLeft().x(), iR.bottomLeft().y(), 0,
        iR.bottomRight().x(), iR.bottomRight().y(), 0,
        iR.topRight().x(), iR.topRight().y(), 0,
        iR.topLeft().x(), iR.topLeft().y(), 0 };

    int i[6] = {
        0, 1, 3,
        1, 2, 3 };

    float t[8] = {
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0
    };

    vbo.setVertices(12, v);
    vbo.setIndices(6, i);
    vbo.set2dTextureCoordinates(8, t);
    vbo.bake();
    vbo.draw();
}


//------------------------------------------------------------------------------
/*dessine un rectangle dans le plan xy. origine iO et taille iS*/
void drawRectangle( const Point2d& iO, const Vector2d& iS)
{ drawRectangle( Rectangle( iO, iS ) ); }

//------------------------------------------------------------------------------
void drawRectangularPrism( const Point3d& iLowerLeft, const Point3d& iTopRight )
{
	Point3d minCorner = iLowerLeft;
	Point3d maxCorner = iTopRight;

  glBegin(GL_QUADS);
  
  //cote X
  glNormal3d( 1.0, 0.0, 0.0 );
  glVertex3d(maxCorner.x(), maxCorner.y(), maxCorner.z());
  glVertex3d(maxCorner.x(), minCorner.y(), maxCorner.z());
  glVertex3d(maxCorner.x(), minCorner.y(), minCorner.z());
  glVertex3d(maxCorner.x(), maxCorner.y(), minCorner.z());    
  
  //cote -X
  glNormal3d( -1.0, 0.0, 0.0 );
  glVertex3d(minCorner.x(), minCorner.y(), minCorner.z());
  glVertex3d(minCorner.x(), minCorner.y(), maxCorner.z());
  glVertex3d(minCorner.x(), maxCorner.y(), maxCorner.z());
  glVertex3d(minCorner.x(), maxCorner.y(), minCorner.z());
  
  //cote -Z
  glNormal3d( 0.0, 0.0, -1.0 );
  glVertex3d(minCorner.x(), minCorner.y(), minCorner.z());
  glVertex3d(minCorner.x(), maxCorner.y(), minCorner.z());
  glVertex3d(maxCorner.x(), maxCorner.y(), minCorner.z());
  glVertex3d(maxCorner.x(), minCorner.y(), minCorner.z());    
  
  //cote Z
  glNormal3d( 0.0, 0.0, 1.0 );
  glVertex3d(maxCorner.x(), maxCorner.y(), maxCorner.z());
  glVertex3d(minCorner.x(), maxCorner.y(), maxCorner.z());
  glVertex3d(minCorner.x(), minCorner.y(), maxCorner.z());
  glVertex3d(maxCorner.x(), minCorner.y(), maxCorner.z());
  
  //cote Y
  glNormal3d( 0.0, 1.0, 0.0 );
  glVertex3d(maxCorner.x(), maxCorner.y(), maxCorner.z());
  glVertex3d(maxCorner.x(), maxCorner.y(), minCorner.z());
  glVertex3d(minCorner.x(), maxCorner.y(), minCorner.z());
  glVertex3d(minCorner.x(), maxCorner.y(), maxCorner.z());
  
  //cote -Y
  glNormal3d( 0.0, -1.0, 0.0 );
  glVertex3d(minCorner.x(), minCorner.y(), minCorner.z());
  glVertex3d(maxCorner.x(), minCorner.y(), minCorner.z());
  glVertex3d(maxCorner.x(), minCorner.y(), maxCorner.z());
  glVertex3d(minCorner.x(), minCorner.y(), maxCorner.z());
  
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
  
  noise3dTexPtr = (GLubyte*) malloc(iSize.x() *
    iSize.y() * iSize.z() * 4);
  if(noise3dTexPtr == NULL)
    return Texture();  
  
  for(f = 0, inc = 0; f < numOctaves; ++f, frequency *= 2, ++inc, amp *= 0.5)
  {
    noise::setNoiseFrequency(frequency);
    ptr = noise3dTexPtr;
    ni[0] = ni[1] = ni[2] = 0; 
    
    inci = 1.0 / (iSize.x() / frequency);
    for(i = 0; i < iSize.x(); ++i, ni[0] += inci)
    {
      incj = 1.0 / (iSize.y() / frequency);
      for(j = 0; j < iSize.y(); ++j, ni[1] += incj)
      {
        inck = 1.0 / (iSize.z() / frequency);
        for(k = 0; k < iSize.z(); ++k, ni[2] += inck, ptr += 4)
        {
          *(ptr+inc) = (GLubyte)(((noise::noise3(ni)+1.0) * amp)*128.0);
        }
      }
    }
  }

	vector<int> s; s.resize(3);
  s[0] = iSize.x(); s[1] = iSize.y(); s[2] = iSize.z();
  r.set( noise3dTexPtr, s, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
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
