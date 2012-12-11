

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
  unsigned int colorToId(const QColor&);
  void draw(const math::PlatonicSolid&, bool = false);
  void drawCircle( math::Vector3d, const math::Point3d&, double );
//Texture get1dNoiseTexture();  
  Texture get2dNoiseTexture(int, int);//const Vector2i&);
  Texture get3dNoiseTexture(const math::Vector3i&);
  QColor idToColor(unsigned int);
}//utilities
}//treeD
}//realisim

#endif