

#include <Broker.h>
#include <data/Scene.h>
#include "Renderer.h"

using namespace realisim;
  using namespace treeD;
using namespace engine3d;
  using namespace core;
  using namespace render;


//-----------------------------------------------------------------------------
Renderer::Renderer(QWidget* ipParent /*=0*/) : Widget3d(ipParent),
  core::Core::Client(),
  mGlIsInitialized(false)
{
  setFocusPolicy(Qt::StrongFocus);
}

//-----------------------------------------------------------------------------
Renderer::~Renderer()
{}

//-----------------------------------------------------------------------------
void Renderer::draw()
{
  showFps();
  const data::Scene& s = getHub().getBroker().getScene();
}

//-----------------------------------------------------------------------------
void Renderer::initializeGL()
{
  Widget3d::initializeGL();

  mGlIsInitialized = true;
}

//-----------------------------------------------------------------------------
bool Renderer::requestStateChange(core::Core::state from, core::Core::state to)
{
  bool r = true;
  if( from == Core::sInitializing && to == Core::sRunning)
  { r = isGlInitialized(); }
  return r;
}

//-----------------------------------------------------------------------------
void Renderer::update(double iMsecElapsed)
{
  QWidget::update();
}