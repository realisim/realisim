

#ifndef Realisim_TreeD_Utilities_hh
#define Realisim_TreeD_Utilities_hh

#include "3d/Texture.h"
#include <cassert>
#include <QByteArray>
#include "math/Point.h"
#include "math/Vect.h"

namespace realisim {namespace math {class PlatonicSolid;} }

/*
*/

namespace realisim
{
namespace treeD
{
namespace utilities
{
	using namespace math;

  unsigned int colorToId(const QColor&);
  void draw(const PlatonicSolid&);
  void drawCircle( Vector3d, const Point3d&, double );
  void drawCircle2d( const Point2d&, double );
  void drawRectangle2d( const Point2d&, const Vector2d& );
  void drawRectangle2d( const Texture&, const Point2d&, const Vector2d& );
//Texture get1dNoiseTexture();  
  Texture get2dNoiseTexture(int, int);//const Vector2i&);
  Texture get3dNoiseTexture(const Vector3i&);
  QColor idToColor(unsigned int);
}//utilities
}//treeD
}//realisim

#endif