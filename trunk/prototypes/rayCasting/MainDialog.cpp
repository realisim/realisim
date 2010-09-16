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

namespace  			
{
  void drawUnitCube()
  {
    float c0[3] = {1.0, 1.0, 0.0};
    float c1[3] = {0, 1.0, 0};
    float c2[3] = {0, 1.0, 1.0};
    float c3[3] = {1.0, 1.0, 1.0};
    
    float c4[3] = {1.0, 0, 0};
    float c5[3] = {0, 0, 0};
    float c6[3] = {0, 0, 1.0};
    float c7[3] = {1.0, 0, 1.0};
    
    float v0[3] = { 1.0f, 1.0f, 0.0f};
    float v1[3] = { 0.0f, 1.0f, 0.0f};
    float v2[3] = { 0.0f, 1.0f, 1.0f};
    float v3[3] = { 1.0f, 1.0f, 1.0f};
    
    float v4[3] = { 1.0f, 0.0f, 0.0f};
    float v5[3] = { 0.0f, 0.0f, 0.0f};
    float v6[3] = { 0.0f, 0.0f, 1.0f};
    float v7[3] = { 1.0f, 0.0f, 1.0f};
    
    glBegin(GL_QUADS);
    //top
    glColor3fv(c0);
    glVertex3fv(v0);
    glColor3fv(c1);
    glVertex3fv(v1);
    glColor3fv(c2);
    glVertex3fv(v2);
    glColor3fv(c3);
    glVertex3fv(v3);
    
    //bottom
    glColor3fv(c4);
    glVertex3fv(v4);
    glColor3fv(c5);
    glVertex3fv(v5);
    glColor3fv(c6);
    glVertex3fv(v6);
    glColor3fv(c7);
    glVertex3fv(v7);
    
    //right
    glColor3fv(c4);
    glVertex3fv(v4);
    glColor3fv(c7);
    glVertex3fv(v7);
    glColor3fv(c3);
    glVertex3fv(v3);
    glColor3fv(c0);
    glVertex3fv(v0);
    
    //left
    glColor3fv(c5);
    glVertex3fv(v5);
    glColor3fv(c1);
    glVertex3fv(v1);
    glColor3fv(c2);
    glVertex3fv(v2);
    glColor3fv(c6);
    glVertex3fv(v6);
    
    //front
    glColor3fv(c4);
    glVertex3fv(v4);
    glColor3fv(c0);
    glVertex3fv(v0);
    glColor3fv(c1);
    glVertex3fv(v1);
    glColor3fv(c5);
    glVertex3fv(v5);
    
    //back
    glColor3fv(c7);
    glVertex3fv(v7);
    glColor3fv(c6);
    glVertex3fv(v6);
    glColor3fv(c2);
    glVertex3fv(v2);
    glColor3fv(c3);
    glVertex3fv(v3);
        
    glEnd();
  }
}

Viewer::Viewer(QWidget* ipParent /*=0*/) : Widget3d(ipParent),
 mRayCastShader(),
 mCtTexture(),
 mDepth(0.01),
 mColorThreshold(0.5)
{
  setFocusPolicy(Qt::StrongFocus);
}

Viewer::~Viewer()
{}

void Viewer::initializeGL()
{
  Widget3d::initializeGL();
  
  QString fragmentSource, vertexSource;
  
  //--- raycast shader
  QFile vert(":/raycast.vert");
  vert.open(QIODevice::ReadOnly);
  vertexSource = vert.readAll();

  QFile frag(":/raycast.frag");
  frag.open(QIODevice::ReadOnly);
  fragmentSource = frag.readAll();
    
  //  qDebug(vertexSource.toStdString().c_str());
  //  qDebug(fragmentSource.toStdString().c_str());
  mRayCastShader.addVertexShaderSource(vertexSource);
  mRayCastShader.addFragmentShaderSource(fragmentSource);
  mRayCastShader.link();
  
  //QFile f("/Users/po/Documents/travail/resonant/data/139_512_512");
  QFile f("../Resources/139_512_512");
  if(f.open(QIODevice::ReadOnly))
  {
    QByteArray a = f.readAll();
    mCtTexture.set(a.data(), Vector3i(139, 512, 512), Texture::fLuminance,
      Texture::dtUnsignedShort);
    f.close();
  }
  
}

//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
  Widget3d::paintGL();
  
  draw3dTextureCube();
}

//-----------------------------------------------------------------------------
void Viewer::draw3dTextureCube()
{
	glPushMatrix();
  //glTranslated(1.0, 0.5, 0.5);
  Matrix4f wcMat;
  wcMat.setTranslation(Point3f(0.0, 0.0, 0.0));
  	
  glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT);
  glEnable(GL_TEXTURE_3D);
  glBindTexture(GL_TEXTURE_3D, mCtTexture.getTextureId());
  pushShader(mRayCastShader);
  mRayCastShader.setUniform("WCmatrix", wcMat);
  mRayCastShader.setUniform("texture", 0);
  mRayCastShader.setUniform("stepSize", mDepth);
  Point3d camPos = getCamera().getPos() * getCamera().getTransformationToGlobal();
  mRayCastShader.setUniform("CameraPos", toVector(camPos));
  mRayCastShader.setUniform("colorThreshold", mColorThreshold);
  
  drawUnitCube();
  popShader();
 
  glDisable(GL_TEXTURE_3D);
  glDisable(GL_BLEND);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1, 0xBBBB);
  drawUnitCube();

  glPopAttrib();
  glPopMatrix();
}

//-----------------------------------------------------------------------------
void Viewer::keyPressEvent(QKeyEvent* ipEvent)
{
  switch (ipEvent->key()) 
  {
    case Qt::Key_Q : mDepth += 0.01; break;
    case Qt::Key_W : mDepth -= 0.01; break;
    
    case Qt::Key_C : mColorThreshold += 0.005; break;
    case Qt::Key_V : mColorThreshold -= 0.005; break;
    default: break;
  }
  update();
}

//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QDialog(),
  mpViewer(0)
{
  resize(200, 200);
  
  QHBoxLayout* pLyt = new QHBoxLayout(this);
  pLyt->setMargin(5);
  mpViewer = new Viewer(this);
  mpViewer->setCameraOrientation(Camera::FREE);
  pLyt->addWidget(mpViewer);
  
  Camera c = mpViewer->getCamera();
  Matrix4d m;
  m.setTranslation(Point3d(0.5, 0.5, 0.5));
  m.setRotation(getRotationMatrix(PI_SUR_2, Vector3d(1.0, 0.0, 0.0)));
  c.setTransformationToGlobal(m);
  mpViewer->setCamera(c, false);
}