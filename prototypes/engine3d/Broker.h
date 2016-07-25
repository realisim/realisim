/**/

#pragma once

#include "data/Scene.h"

namespace engine3d
{
namespace core
{
  class Broker
  {
  public:
    friend class Hub;
    
    Broker();
    ~Broker();

    const data::Scene& getScene() const {return mScene;}
    
  protected:
    
    data::Scene mScene;
  };
  
}
}
