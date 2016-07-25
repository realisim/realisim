/**/

#pragma once

namespace engine3d { namespace core {class Broker;} }
namespace engine3d { namespace core {class SceneManagement;} }
namespace engine3d { namespace render {class Renderer;} }

namespace engine3d
{
namespace core
{
  class Hub
  {
    friend class Core;
  public:
    Hub();
    ~Hub();
    
    Broker& getBroker() {return *mpBroker;}
    render::Renderer& getRenderer() {return *mpRenderer;}
    SceneManagement& getSceneManagement() {return *mpSceneManagement;}
    
  protected:
    void initialize(Broker*, SceneManagement*, render::Renderer*);
    
    Broker *mpBroker; //never null, not owned
    SceneManagement *mpSceneManagement; //never null, not owned
    render::Renderer *mpRenderer; //never null, not owned
  };
  
}
}