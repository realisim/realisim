/**/

#pragma once

#include <QColor>
#include <vector>

namespace engine3d
{
namespace data
{
  struct Light
  {
    Light();
    ~Light();
    
    enum type{tOmnidirectional, tSpot};
    
    QColor mColor;
  };
  
}
}
