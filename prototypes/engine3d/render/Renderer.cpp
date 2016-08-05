

#include <3d/Utilities.h>
#include <Broker.h>
#include <data/Scene.h>
#include <data/Tile.h>
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

  drawTiles();
}

//-----------------------------------------------------------------------------
void Renderer::drawTiles()
{
	const data::Scene& s = getHub().getBroker().getScene();
	for (size_t i = 0; i < s.mTiles.size(); ++i)
	{
		const data::Tile& t = s.mTiles[i];
		treeD::drawRectangle( math::Point2d(t.getLatitude(), t.getLongitude()),
            t.getSize() );
	}
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