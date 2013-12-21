

#ifndef Realisim_TreeD_Utilities_hh
#define Realisim_TreeD_Utilities_hh

#include "3d/Texture.h"
#include <cassert>
#include <QByteArray>
#include "math/Point.h"
#include "math/Primitives.h"
#include "math/Vect.h"

namespace realisim {namespace math {class PlatonicSolid;} }

/* Divers utilitaires openGL.
  
  ScreenSpaceProjection est un garde qui sert a faire un projection 2d
  orthogonale afin de dessiner directement a l'écran en coordonné pixel.
  Typiquement:
  
  {
  	ScreenSpaceProjection( 100, 100 );
    glBegin(...);
    ...
    glEnd();
  }
  
  La projection originale est appliquée lorsque l'object se détruit.
*/

namespace realisim
{
namespace treeD
{

	using namespace math;

	class ScreenSpaceProjection
  {
  	public:
  	explicit ScreenSpaceProjection( const math::Vector2d& );
    ~ScreenSpaceProjection();
  };

  unsigned int colorToId(const QColor&);
  void draw(const PlatonicSolid&);
  void drawCircle( Vector3d, const Point3d&, double );
  void drawCircle( const Point2d&, double );
  void drawLine( const Point2d&, const Point2d& );
  void drawRectangle( const Rectangle& );
  void drawRectangle( const Point2d&, const Vector2d& );
  void drawRectangle( const Texture&, const Point2d&, const Vector2d& );
//Texture get1dNoiseTexture();  
  Texture get2dNoiseTexture(int, int);//const Vector2i&);
  Texture get3dNoiseTexture(const Vector3i&);
  QColor idToColor(unsigned int);

}//treeD
}//realisim

#endif