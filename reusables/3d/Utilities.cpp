
#include "3d/Utilities.h"
#include "math/Noise.h"
#include "math/PlatonicSolid.h"

namespace realisim 
{
  using namespace math;
namespace treeD 
{
namespace utilities 
{
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
  double rot = acos( iN.normalise() & Vector3d( 0.0, 0.0, 1.0 ) );
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
}
