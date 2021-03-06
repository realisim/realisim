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
#include "ParticuleSystem.h"
#include "Viewer.h"
#include <QColor>
#include <QKeyEvent>
#include <qlayout.h>
#include <QFile.h>

using namespace realisim;
  using namespace math;
  using namespace openCL;
  using namespace treeD;
  
const unsigned int kPlusMinusId = 10000;

Viewer::Viewer(QWidget* ipParent /*=0*/) : Widget3d(ipParent),
 mPs(PlatonicSolid::tIsocahedron),
 mFbo(),
 mParticuleTexture(),
 mGravityHoleTexture(),
 mpParticuleSystem(0),
 mSelectedId(0),
 mMousePosX(0),
 mMousePosY(0),
 mPlusMinus(),
 mButtonState(bsMinus)
{
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
}

Viewer::~Viewer()
{}

//-----------------------------------------------------------------------------
void Viewer::drawEmitter(bool iPicking) const
{
  const ParticuleSystem& ps = *mpParticuleSystem;
  glPointSize(20);
  
  if(iPicking)
  {
    QColor c = idToColor(1);
    glColor4ub(c.red(), c.green(), c.blue(), c.alpha());
  }
  else
    glColor4ub(255, 255, 255, 255);
  glBegin(GL_POINTS);

  	glVertex3dv(ps.getEmitterPosition().getPtr());  

  glEnd();
  
}

//-----------------------------------------------------------------------------
void Viewer::drawGravityHoles(bool iPicking) const
{
  const ParticuleSystem& ps = *mpParticuleSystem;
  //gravityHole
  glBindTexture(GL_TEXTURE_2D, mGravityHoleTexture.getId());
  glPointSize(20.0);
  
  glColor4ub(255, 255, 255, 255);    
  glBegin(GL_POINTS);
  for(unsigned int i = 0; i < ps.getGravityHoles().size(); i++)
  {
    if(iPicking)
    {
      QColor c = idToColor(1 + i + 1);
      glColor4ub(c.red(), c.green(), c.blue(), c.alpha());
    }
  	glVertex3dv(ps.getGravityHoles()[i].mPos.getPtr());  
  }
  glEnd();
}

//-----------------------------------------------------------------------------
void Viewer::drawParticules(bool iPicking) const
{
  //toutes les particules
  const ParticuleSystem& ps = *mpParticuleSystem;
  glBindTexture(GL_TEXTURE_2D, mParticuleTexture.getId());
	glPointSize(4.0);
//  glBegin(GL_POINTS);
//    for(unsigned int i = 0; i < ps.getNumberOfParticules(); i+=4)
//    	glVertex3fv(&ps.getParticulesPosition()[i]);
//  glEnd();
  glEnableClientState(GL_VERTEX_ARRAY);
  const float* pp = ps.getParticulesPosition();
  glVertexPointer(4, GL_FLOAT, 0, pp);
  glDrawArrays(GL_POINTS, 0, ps.getNumberOfParticules());
  glDisableClientState(GL_VERTEX_ARRAY);
}

//-----------------------------------------------------------------------------
void Viewer::drawScene() const
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glActiveTexture(GL_TEXTURE0);
	glEnable(GL_POINT_SPRITE);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  const ParticuleSystem& ps = *mpParticuleSystem;
  glColor3ub(ps.getColor().red(), ps.getColor().green(), ps.getColor().blue());
  drawParticules();
  
  glEnable(GL_DEPTH_TEST);
  drawGravityHoles();
  
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  
  {
    treeD::ScreenSpaceProjection ssp( Vector2d( width(), height() ) );
    glPushMatrix();
    glTranslated( 10, 10, 0);
    mPlusMinus.draw();
    glPopMatrix();
  }
}

//------------------------------------------------------------------------------
void Viewer::drawSceneForPicking() const
{
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glDisable(GL_POINT_SPRITE);
  
  drawEmitter(true);
  drawGravityHoles(true);
  
  QColor c = idToColor(kPlusMinusId);
  glColor4ub(c.red(), c.green(), c.blue(), c.alpha());
  {
    treeD::ScreenSpaceProjection ssp( Vector2d( width(), height() ) );
    Vector2d t = mPlusMinus.getTranslation();
    glPushMatrix();
    glTranslated( 10, 10, 0);
    drawRectangle( Rectangle( Point2d(), mPlusMinus.getFrameSize() ) );
    glPopMatrix();
  }
  
  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
}

//------------------------------------------------------------------------------
void Viewer::handlePlusMinusState()
{
  Texture t = mPlusMinus.getTexture();
	switch (mButtonState)
  {
    case bsMinus:
    	mPlusMinus.set(t, QRect(QPoint(21, 0), QSize(21, 21)));
      break;
    case bsMinusHover:
      mPlusMinus.set(t, QRect(QPoint(21, 21), QSize(21, 21)));
      break;
    case bsPlus:
      mPlusMinus.set(t, QRect(QPoint(0, 0), QSize(21, 21)));
      break;
    case bsPlusHover:
      mPlusMinus.set(t, QRect(QPoint(0, 21), QSize(21, 21)));
      break;
    default:
      break;
  }
}

//------------------------------------------------------------------------------
void Viewer::initializeGL()
{
  Widget3d::initializeGL();
  glClearColor(0.0, 0.0, 0.0, 0.0);
  
//  //for point attenuation
//  glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 1.0f );
//	glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, 24.0f );
//  float quadratic[] =  { 0.0f, 1.0f, 0.0f };
//  glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, quadratic);
  
  mFbo.addColorAttachment(true);
  mFbo.addDepthAttachment(true);
  
  mParticuleTexture.set(QImage(":/images/particule2.png"));
  mGravityHoleTexture.set(QImage(":/images/gravityHole.png"));
  
  Texture t;
  t.set( QImage(":/images/plusMinus.png") );
  //initialisé a minus
  mPlusMinus.set(t, QRect(QPoint(21,0), QSize(21, 21)));
  mPlusMinus.setAnchorPoint(Sprite::aBottomLeft);
}

//-----------------------------------------------------------------------------
void Viewer::mouseDoubleClickEvent(QMouseEvent* ipE)
{
//  makeCurrent();
//  vector<unsigned int> hits = pick(ipE->x(), ipE->y());
//  if(!hits.empty())
//  {
//  	unsigned int hit = hits[0];
//    
//    ParticuleSystem& ps = *mpParticuleSystem;
//    Camera c = getCamera();
//    Vector3d d;
//    switch (hit)
//    {
//      case 0: break;
//      case 1: //emitter selected 
//      	d = ps.getEmitterPosition() - c.getLook();
//        break;
//      default: // gravity holes selected
//      {
//        ParticuleSystem::GravityHole g = 
//          ps.getGravityHoles()[hit - 2];
//      	d = g.mPos - c.getLook();
//        break;
//      }
//      c.translate( d );
//      setCamera(c, true);
//    }
//  }
}

//-----------------------------------------------------------------------------
void Viewer::mouseMoveEvent(QMouseEvent* ipE)
{
  makeCurrent();
  int deltaX = ipE->x() - mMousePosX;
  int deltaY = ipE->y() - mMousePosY;
  
  if(ipE->buttons() == Qt::LeftButton && !mSelectedId) 
  {
    Camera c = getCamera();
    Vector3d v = c.pixelDeltaToGLDelta(deltaX, deltaY);
    c.translate(-v);
    setCamera(c, false);
  } 
  
  ParticuleSystem& ps = *mpParticuleSystem;
  switch (mSelectedId)
  {
    case 0: break;
    case 1: //emitter selected
    {
			Camera c = getCamera();
      Vector3d v = 
        c.pixelDeltaToGLDelta(deltaX, deltaY, ps.getEmitterPosition());
      ps.setEmitterPosition( ps.getEmitterPosition() + v );
      break;
    }
    default: // gravity holes selected
    {
      ParticuleSystem::GravityHole g = 
        ps.getGravityHoles()[mSelectedId-2];
      Camera c = getCamera();
      Vector3d v = 
        c.pixelDeltaToGLDelta(deltaX, deltaY, g.mPos);
      ps.setGravityHolePosition(mSelectedId-2, g.mPos + v);
    }
      break;
  }
  
  vector<unsigned int> p = pick(ipE->x(), ipE->y());
  if(!p.empty() && p[0] == kPlusMinusId)
  {
  	switch (mButtonState)
    {
      case bsMinus: mButtonState = bsMinusHover; break;
      case bsPlus: mButtonState = bsPlusHover; break;
      default: break;
    }
  }
  else
	{
  	switch (mButtonState) 
    {
      case bsMinusHover : mButtonState = bsMinus; break;
      case bsPlusHover : mButtonState = bsPlus; break;
      default: break;
    }
  }
  handlePlusMinusState();
  
  mMousePosX = ipE->x();
  mMousePosY = ipE->y();
}

//-----------------------------------------------------------------------------
void Viewer::mousePressEvent(QMouseEvent* ipE)
{
  makeCurrent();
  mMousePosX = ipE->x();
  mMousePosY = ipE->y();
  
  vector<unsigned int> hits = pick(ipE->x(), ipE->y());
  if(!hits.empty())
  {
  	mSelectedId = hits[0];
    if(mSelectedId == kPlusMinusId)
    {
      bool hide = false;
    	switch (mButtonState)
      {
        case bsMinusHover: hide = true; mButtonState = bsPlus; break;
        case bsPlusHover: mButtonState = bsMinus; break;
        default: break;
      }
      handlePlusMinusState();
      emit hidePanel(hide);
    }    	
    else if(mSelectedId >= 2) //les gravity hole commence a l'index 2.
      emit gravityHoleSelected(mSelectedId-2);
  }
}

//-----------------------------------------------------------------------------
void Viewer::mouseReleaseEvent(QMouseEvent* ipE)
{
  makeCurrent();
  mSelectedId = 0;
}

//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
  Widget3d::paintGL();
  if(mpParticuleSystem)
    drawScene();
  
  showFps();
}

//-----------------------------------------------------------------------------
void Viewer::resizeGL(int iWidth, int iHeight)
{
  Widget3d::resizeGL(iWidth, iHeight);
  mFbo.resize(iWidth, iHeight);
  Camera c = getCamera();
}
