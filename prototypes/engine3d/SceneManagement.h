/**/

#pragma once
#include "Core.h"
#include <utils/log.h>

namespace engine3d{ namespace data {struct Scene;} }

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
      
    protected:
      virtual bool requestStateChange(Core::state from, Core::state to) override;
      virtual void stateChanged(Core::state) override;
      
    private:
      void setScene(data::Scene*);
      
      data::Scene *mScene; //not owned. coming from broker      
    };
    
  }
}
