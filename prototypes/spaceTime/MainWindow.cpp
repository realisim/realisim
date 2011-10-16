
#include "3d/Camera.h"
#include "Engine.h"
#include "math/Matrix4x4.h"
#include "math/Point.h"
#include "math/MathUtils.h"
#include "MainWindow.h"
#include <qapplication.h>
#include <QKeyEvent>
#include <qlayout.h>
#include <QFile.h>
#include <time.h>

using namespace realisim;
  using namespace math;
  using namespace treeD;
using namespace SpaceTime;

namespace
{
  double kFov = 60.0;
  double kNear(0.005);
  double kFar(25000.0);
}
//-----------------------------------------------------------------------------
//--- MainWindow
//-----------------------------------------------------------------------------
MainWindow::MainWindow() : QMainWindow(),
  mpViewer(0),
  mEngine(),
  mIsDebugging(false)
{
  resize(800, 600);
  
  //initialise l'engin
  mEngine.generateAstronomicalBodies(5000);

  //initialise le layout
  mpViewer = new Viewer(this, mEngine);
  mpViewer->initialize();
  setCentralWidget(mpViewer);
  
  //on enregistre le viewer comme client de l'engin
  mEngine.registerClient(mpViewer);
    
  setAsDebugging(true);
     
  mEngine.goToState(Engine::sPlaying);
}

//-----------------------------------------------------------------------------
void MainWindow::keyPressEvent(QKeyEvent* ipE)
{
	switch (ipE->key()) 
  {
  	case Qt::Key_Escape:
    	if(mEngine.getState() == Engine::sPlaying)
      	mEngine.goToState(Engine::sPaused);
      else if(mEngine.getState() == Engine::sPaused)
        mEngine.goToState(Engine::sPlaying);
    break;
    case Qt::Key_M:
    {
    	Camera c = mpViewer->getCamera();
      if(c.getMode() == Camera::ORTHOGONAL)
      	c.setProjection(kFov, 1.0, kNear, kFar, true);
      else
      	c.setProjection(50, 0.5, 2000);
      mpViewer->setCamera(c, false);
      update();
    }
    break;
    case Qt::Key_G:
    {
    	mEngine.goToState(Engine::sGenerating);
      update();
    }
    break;
  	default: ipE->setAccepted(false); break;
  }
}
//-----------------------------------------------------------------------------
void MainWindow::setAsDebugging(bool iD)
{
	mIsDebugging = iD;
	mEngine.setAsDebugging(iD);
  mpViewer->setAsDebugging(iD);
}