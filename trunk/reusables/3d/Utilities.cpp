
#include "3d/Utilities.h"
#include "math/Noise.h"

namespace realisim 
{
  using namespace math;
namespace treeD 
{
namespace utilities 
{

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
  
  Texture r(noise3dTexPtr, iSize);
  free(noise3dTexPtr);
  return r;
}

}
}
}
