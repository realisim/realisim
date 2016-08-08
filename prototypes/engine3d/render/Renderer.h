/**/

#pragma once

#include "Core.h"
#include "3d/Widget3d.h"

namespace engine3d{ namespace data {struct Scene;} }

namespace engine3d
{
namespace render
{
  class Renderer : public realisim::treeD::Widget3d,
    public core::Core::Client
  {
  public:
    Renderer(QWidget*);
    ~Renderer();
    
    bool isGlInitialized() const {return mGlIsInitialized;}
    
  private:
    virtual void draw() override;
    virtual void drawTiles(); 
    virtual void initializeGL() override;
    virtual void mouseMoveEvent(QMouseEvent*) override;
    virtual bool requestStateChange(core::Core::state, core::Core::state) override;
    virtual void update(double) override;
    
    bool mGlIsInitialized;
  };
  
}
}



