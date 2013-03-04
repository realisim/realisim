/*
 *  MainWindow.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 08/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "3d/Camera.h"
#include "3d/Texture.h"
#include "3d/Utilities.h"
#include "math/Matrix4x4.h"
#include "math/Point.h"
#include "math/MathUtils.h"
#include "MainDialog.h"
#include <QKeyEvent>
#include <qlayout.h>
#include <QFile.h>

using namespace realisim;
  using namespace math;
  using namespace treeD;

Viewer::Viewer(QWidget* ipParent /*=0*/) : Widget3d(ipParent),
  mMouseState(msIdle),
	mMouseX(0),
  mMouseY(0),
  mCurves(1, BezierCurve()),
  mCurrentCurve(0),
  mCurrentPoint(-1),
  mHover(-1),
  mState(sCreation)
{
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
}

Viewer::~Viewer()
{}

//-----------------------------------------------------------------------------
void Viewer::drawBezier(bool iPicking) const
{
  QColor c;
  for(unsigned int i = 0; i < mCurves.size(); ++i)
  {
  	const BezierCurve& bc = mCurves[i];
    
    const vector<BezierCurve::ControlPoint>& cp = bc.getControlPoints();
    const vector<Point3d>& rasterPoints = bc.getRaster();
    
    glDisable(GL_LIGHTING);
    
    if((int)i == mCurrentCurve)
    {
      //tous les points      
      glPointSize(5.0);
      glBegin(GL_POINTS);
      for(unsigned int i = 0; i < cp.size(); ++i)
      {        
        glColor3ub(50, 0, 255);
        if(cp[i].isHandle())
        {
          glColor3ub(50, 125, 255);
        }
        if(!iPicking && (mCurrentPoint == i || mHover == i))
        	glColor3ub(200, 200, 0);
        else if(iPicking && (getState() == sCreation || getState() == sEdition))
        {
          glPointSize(8.0);
        	c = utilities::idToColor(i);
          glColor4ub(c.red(), c.green(), c.blue(), c.alpha());
        }
        glVertex3dv(cp[i].getPos().getPtr());
      }
      glEnd();
      
      //ligne des handles
      glLineWidth(1.0);
      glColor3ub(50, 125, 255);
      for(unsigned int i = 0; i < cp.size(); ++i)
      {
        if(cp[i].isHandle())
        {
          glBegin(GL_LINES);
            glVertex3dv(cp[i].getPos().getPtr());
            glVertex3dv(cp[cp[i].handleTo()].getPos().getPtr());  
          glEnd();
        }
      }
    }
    
    glLineWidth(1.5);
    //la rasterization
    glColor3ub(0, 250, 0);
    if(iPicking && getState() == sSelection)
    {
      glLineWidth(5.0);
      c = utilities::idToColor(i);
      glColor4ub(c.red(), c.green(), c.blue(), c.alpha());
    }
    glBegin(GL_LINE_STRIP);
    for(unsigned int i = 0; i < rasterPoints.size(); ++i)
    {
      glVertex3dv(rasterPoints[i].getPtr());
    }
    glEnd();
  }
}

//-----------------------------------------------------------------------------
void Viewer::drawSceneForPicking() const
{
	glDisable(GL_MULTISAMPLE);
  drawBezier(true);
};

//-----------------------------------------------------------------------------
void Viewer::initializeGL()
{
  Widget3d::initializeGL();
  
	/*Multi sample à été activé globalement pour l'application.
    voir main.cpp*/
  glEnable(GL_MULTISAMPLE);

}

//-----------------------------------------------------------------------------
void Viewer::keyPressEvent(QKeyEvent* ipE)
{
	switch (ipE->key()) 
  {
  	case Qt::Key_Backspace:
    	if(mCurrentCurve != -1 && mCurrentPoint != -1)
      	mCurves[mCurrentCurve].remove(mCurrentPoint);
        break;
    case Qt::Key_C: 
      setState(sCreation);
      mCurrentPoint = -1;
      break;
    case Qt::Key_E: 
      setState(sEdition);
      mCurrentPoint = -1;
      break;
    case Qt::Key_N:    	
      mCurves.push_back(BezierCurve());
      mCurrentCurve = mCurves.size() - 1;
      mCurrentPoint = - 1;
      setState(sCreation);
      break;
    case Qt::Key_S:
      setState(sSelection);
      mCurrentPoint = -1;
      break;
    case Qt::Key_O:
			mCurves[mCurrentCurve].open();
      break;
    default: break;
  }
  update();
}

//-----------------------------------------------------------------------------
void Viewer::mouseMoveEvent(QMouseEvent* ipE)
{
  if(ipE->buttons() == Qt::RightButton)
	  Widget3d::mouseMoveEvent(ipE);
  else 
  {
    int dx = ipE->x() - mMouseX;
    int dy = ipE->y() - mMouseY;
    mMouseX = ipE->x();
    mMouseY = ipE->y();
    
    switch (getMouseState()) 
    {
      case msIdle:
      {
      	vector<unsigned int> p = pick(ipE->x(), ipE->y());
        if(getState() == sCreation || getState() == sEdition)
        {
 	        if(!p.empty())
          	mHover = p[0];
          else
            mHover = -1;
					update();
        }
      }
      break;
      case msDown:
      {
        setMouseState(msDrag);
        if(getState() == sCreation)
	        mCurrentPoint = mCurves[mCurrentCurve].addHandlesTo(mCurrentPoint);
      }break;
      case msDrag: 
      {
      	if(getState() == sEdition || getState() == sCreation)
        {
          const Camera& c = getCamera();
          Vector3d v = c.pixelDeltaToGLDelta(dx, -dy,
            mCurves[mCurrentCurve].getPos(mCurrentPoint));
          mCurves[mCurrentCurve].move(mCurrentPoint, v);
          update();
        }
      }break;
      default: break;
    }
  }
}

//-----------------------------------------------------------------------------
void Viewer::mousePressEvent(QMouseEvent* ipE)
{
	if(ipE->buttons() == Qt::RightButton)
	  Widget3d::mousePressEvent(ipE);
  else 
  {
    const Camera& c = getCamera();
    switch (getMouseState()) 
    {
      case msIdle:
      {
        setMouseState(msDown);
        
        if(getState() == sCreation)
        {
          Point3d p = c.pixelToGL(ipE->x(), ipE->y(), c.getLook());
          if(mHover != -1)
          	if(mCurves[mCurrentCurve].canCloseOn(mHover))
            {
            	mCurrentPoint = mHover;
              mCurves[mCurrentCurve].close();
            }
            else
              mCurrentPoint = mHover;
          else
            mCurrentPoint = mCurves[mCurrentCurve].add(p);
        }
        else if(getState() == sSelection)
        {
        	vector<unsigned int> p = pick(ipE->x(), ipE->y());
          if(!p.empty() && p[0] >= 0 && p[0] < mCurves.size())
            mCurrentCurve = p[0];
        }
        else  //edition
        {
          vector<unsigned int> p = pick(ipE->x(), ipE->y());
          if(!p.empty())
            mCurrentPoint = p[0]; 
          else 
          {
            mCurrentPoint = -1;
          }
        }

      }break;

      case msDown: break;
      case msDrag: break;
      default: break;
    }
  }
  update();
}

//-----------------------------------------------------------------------------
void Viewer::mouseReleaseEvent(QMouseEvent* ipE)
{
	Widget3d::mouseReleaseEvent(ipE);
  switch (getMouseState()) 
  {
    case msIdle: setMouseState(msIdle); break;
    case msDown: setMouseState(msIdle);break;
    case msDrag: setMouseState(msIdle); break;
    default: break;
  }
  update();
}


//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
  Widget3d::paintGL();
  glEnable(GL_MULTISAMPLE);
  
  glColor3ub(156, 156, 157);
  QString s("'S' pour sélectionner une courbe.");
  QFontMetrics fm(font());
  int h = fm.height();
  renderText(5, 15, s);
  s = "'C' pour ajouter à une courbe.";
   renderText(5, 15 + h, s);
   s = "'E' pour éditer une courbe.";
   renderText(5, 15 + 2*h, s);
   s = "'Click' pour ajouter un point a la courbe.";
   renderText(5, 15 + 3*h, s);
   s = "'Backspace' pour effacer un point.";
   renderText(5, 15 + 4*h, s);
   s = "'Double click' pour ajouter un point sur la courbe";
  renderText(5, 15 + 5*h, s);
  s = "'N' pour ajouter une nouvelle courbe";
  renderText(5, 15 + 6*h, s);
  s = "'O' pour ouvrir la courbe (si elle est fermée)";
  renderText(5, 15 + 7*h, s);
 
  drawBezier(false);
}

//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QMainWindow(),
  mpViewer(0)
{
  resize(800, 600);
  
  QHBoxLayout* pLyt = new QHBoxLayout(this);
  pLyt->setMargin(5);
  mpViewer = new Viewer(this);
  //mpViewer->setCameraMode(Camera::ORTHOGONAL);
  //mpViewer->setCameraOrientation(Camera::XY);
  mpViewer->setCameraOrientation(Camera::FREE);
  pLyt->addWidget(mpViewer, 1);
  setCentralWidget(mpViewer);
  
//  Camera c = mpViewer->getCamera();
//  Matrix4d m;
//  m.setTranslation(Point3d(8.0, 0.0, 0.0));
//  c.setTransformationToGlobal(m);
//  mpViewer->setCamera(c, false);
}