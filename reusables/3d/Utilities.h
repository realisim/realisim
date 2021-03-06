

#ifndef Realisim_TreeD_Utilities_hh
#define Realisim_TreeD_Utilities_hh

#include "3d/Texture.h"
#include "3d/VertexbufferObject.h"
#include <cassert>
#include <QByteArray>
#include "math/Matrix4.h"
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
        explicit ScreenSpaceProjection(const math::Vector2d&);
        ~ScreenSpaceProjection() {}

        Matrix4 mViewMatrix;
        Matrix4 mProjectionMatrix;
    };

  unsigned int colorToId(const QColor&);
  void draw(const PlatonicSolid&);
  void drawCircle( Vector3d, const Point3d&, double );
  void drawCircle( const Point2d&, double );  
  void drawLine( const Point2d&, const Point2d& );
  void drawPoint(const Point2d&, double = 1.0 );
  void drawRectangle( const Rectangle& );
  void drawRectangle( const Point2d&, const Vector2d& );
  void drawRectangularPrism( const Point3d& , const Point3d& );
    
//Texture get1dNoiseTexture();  
  Texture get2dNoiseTexture(int, int);//const Vector2i&);
  Texture get3dNoiseTexture(const Vector3i&);
  VertexBufferObject getRectangularPrism(const Point3d& , const Point3d&);
  QColor idToColor(unsigned int);

  //gl error detection
  bool hasGlError(std::string*);

}//treeD
}//realisim

#endif