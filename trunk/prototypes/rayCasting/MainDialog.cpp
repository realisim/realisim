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
#include <qslider.h>

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
 mHounsfieldLUT(),
 mDepth(0.01),
 mIsoSurfaceValue(0.3)
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
  
  QFile f("../Resources/73_512_512");
  //QFile f("../Resources/139_512_512");
  //QFile f("../Resources/CThead_256_256_113");
  //QFile f("../Resources/pelvic_91_512_512");
  if(f.open(QIODevice::ReadOnly))
  {
    QByteArray a = f.readAll();
    mCtTexture.set(a.data(), Vector3i(73, 512, 512), Texture::fLuminance,
      Texture::dtUnsignedShort);
    f.close();
  }
  
  mHounsfieldLUT.set(QImage(":/hounsfieldLUT.png"));
}

//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
  Widget3d::paintGL();
  
  draw3dTextureCube();
  
  showFps();
}

//-----------------------------------------------------------------------------
void Viewer::draw3dTextureCube()
{
	glPushMatrix();
  //glTranslated(1.0, 0.5, 0.5);
  Matrix4f wcMat;
  wcMat.setTranslation(Point3f(0.0, 0.0, 0.0));
  	
  glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT);
  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_3D);
  glBindTexture(GL_TEXTURE_3D, mCtTexture.getTextureId());
  pushShader(mRayCastShader);
  mRayCastShader.setUniform("WCmatrix", wcMat);
  mRayCastShader.setUniform("texture", 0);
  mRayCastShader.setUniform("stepSize", mDepth);
  Point3d camPos = getCamera().getPos() * getCamera().getTransformationToGlobal();
  mRayCastShader.setUniform("CameraPos", toVector(camPos));
  mRayCastShader.setUniform("isoSurfaceValue", getIsoSurface());
  
  glActiveTexture(GL_TEXTURE1);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, mHounsfieldLUT.getTextureId());
  mRayCastShader.setUniform("hounsfieldLUT", 1);
  
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
    
//    case Qt::Key_C : mColorThreshold += 0.005; break;
//    case Qt::Key_V : mColorThreshold -= 0.005; break;
    default: break;
  }
  update();
}

//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QDialog(),
  mpViewer(0)
{
  resize(800, 600);
  
  QVBoxLayout* pLyt = new QVBoxLayout(this);
  pLyt->setMargin(5);
  
  //ajout du visualiseur
  mpViewer = new Viewer(this);
  mpViewer->setCameraOrientation(Camera::FREE);
  pLyt->addWidget(mpViewer);
  
  //ajout du slider pour controler l'isosurface
  QSlider* pSlider = new QSlider(Qt::Horizontal, this);
  pSlider->setMinimum(0);
  pSlider->setMaximum(65535); //16 bit volumes
  connect(pSlider, SIGNAL(valueChanged(int)),
    this, SLOT(isoSurfaceValueChanged(int)));
  pLyt->addWidget(pSlider);
  
  //déplace la camera
  Camera c = mpViewer->getCamera();
  c.setPos(Point3d(5.0, 5.0, 5.0));
  Matrix4d m;
  m.setTranslation(Point3d(0.5, 0.5, 0.5));
  m.setRotation(getRotationMatrix(PI_SUR_2, Vector3d(1.0, 0.0, 0.0)));
  //m.setRotation(getRotationMatrix(-PI_SUR_2, Vector3d(1.0, 0.0, 0.0)));
  c.setTransformationToGlobal(m);
  mpViewer->setCamera(c, false);
}

//-----------------------------------------------------------------------------
void MainDialog::isoSurfaceValueChanged(int iValue)
{
  //on ramene la valeur entre 0 et 1
  float v = iValue / 65535.0;
  mpViewer->setIsoSurface(v);
  mpViewer->update();
}