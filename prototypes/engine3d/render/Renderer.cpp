

#include <3d/Utilities.h>
#include <Broker.h>
#include <data/Scene.h>
#include <data/Tile.h>
#include "Renderer.h"
#include <QMouseEvent>

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

  Camera c = getCamera();
  c.set(Point3d(0.0, -100, 50.0),
      Point3d(0.0, 0.0, 0.0),
      Vector3d(0.0, 0.0, 1.0) );
  c.setPerspectiveProjection( 65.0, 1.0, 1.0, 150000 );
  setCamera(c, false);
  setControlType(Widget3d::ctFree);
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
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glColor3ub(255, 255, 255);

    //treeD::drawRectangle( math::Point2d(0.0, 0.0), math::Vector2d(100, 100) );

	const data::Scene& s = getHub().getBroker().getScene();
	for (size_t i = 0; i < s.mTiles.size(); ++i)
	{
		const data::Tile& t = s.mTiles[i];
		treeD::drawRectangle( math::Point2d(t.getLatitude(), t.getLongitude()),
            t.getSize() );
	}

    glEnable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

//-----------------------------------------------------------------------------
void Renderer::initializeGL()
{
  Widget3d::initializeGL();

  glCullFace(GL_BACK);

  mGlIsInitialized = true;
}

//-----------------------------------------------------------------------------
void Renderer::mouseMoveEvent(QMouseEvent *e)
{
    makeCurrent();

    if (mMousePressed && !isAnimatingCamera()) //deplacement de la camera
    {
        int deltaX = e->x() - mMousePosX;
        int deltaY = e->y() - mMousePosY;

        Vector3d delta = getCamera().screenToWorld(Vector2d(deltaX, deltaY),
            mCam.getLook());
        switch (getControlType())
        {
        case ctPan: mCam.translate(-delta); break;
        case ctRotateAround:
        {
            //arbitrairement, la taille du viewport correspond a une rotation de 360        
            double radX = deltaX * 2 * PI / (double)mCam.getViewport().getWidth();
            double radY = deltaY * 2 * PI / (double)mCam.getViewport().getHeight();
            //rotation relative a x;
            mCam.rotate(-radX, Vector3d(0.0, 0.0, 1.0), mCam.getLook());
            //rotation relative a y
            mCam.rotate(-radY, mCam.cameraToWorld(Vector3d(1, 0, 0)), mCam.getLook());
        } break;
        case ctFree:
        {
            //arbitrairement, la taille du viewport correspond a une rotation de 360        
            double radX = deltaX * 2 * PI / (double)mCam.getViewport().getWidth();
            double radY = deltaY * 2 * PI / (double)mCam.getViewport().getHeight();

            mCam.rotate(-radX, Vector3d(0.0, 0.0, 1.0), mCam.getPos());
            mCam.rotate(-radY, mCam.getLat(),
                mCam.getPos());
        }break;
        default: break;
        }
    }

    mMousePosX = e->x();
    mMousePosY = e->y();

    QWidget::update();
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