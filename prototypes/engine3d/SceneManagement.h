/**/

#pragma once
#include "Core.h"
#include <utils/log.h>

namespace engine3d{ namespace data {class Scene;} }

namespace engine3d
{
  namespace core
  {
    class SceneManagement : public core::Core::Client
    {
    public:
      friend class Hub;
      
      SceneManagement();
      ~SceneManagement();
      
      void clear();
      void loadDemoScene();
      
    protected:
      void setScene(data::Scene*);
      
      data::Scene *mScene; //not owned. coming from broker      
    };
    
  }
}
