/**/

#pragma once

#include <QColor>
#include <math/Matrix4.h>
#include <vector>


namespace engine3d
{
namespace data
{
  struct Light
  {
    Light();
    ~Light();
    
    enum type{tDirectional, tOmnidirectional, tSpot};
    
    type mType;
    QColor mColor;
    realisim::math::Matrix4 mTransform;
  };
  
}
}
