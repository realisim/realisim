/**/

#pragma once

#include <data/Light.h>
#include <data/Model.h>
#include <vector>

namespace engine3d
{
namespace data
{
  class Scene
  {
  public:
    Scene();
    ~Scene();
    
    void clear();
    
  protected:
    std::vector<Model> mModels;
    //std::vector<Model> mTransparentModels;
    std::vector<Light> mLights;
    //std::vector<Camera> mCameras;
  };
  
}
}
