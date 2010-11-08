

#ifndef Realisim_TreeD_Utilities_hh
#define Realisim_TreeD_Utilities_hh

#include "3d/Texture.h"
#include <cassert>
#include <QByteArray>
#include "math/Vect.h"

/*
*/

namespace realisim
{
namespace treeD
{
namespace utilities
{
//Texture get1dNoiseTexture();  
  Texture get2dNoiseTexture(int, int);//const Vector2i&);
  Texture get3dNoiseTexture(const math::Vector3i&);
  
}//utilities
}//treeD
}//realisim

#endif