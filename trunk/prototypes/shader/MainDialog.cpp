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
 mPs(PlatonicSolid::tIsocahedron, 3),
 mShader(),
 mShader2(),
 mNoiseShader(),
 mSunShader(),
 mRayCastShader(),
 m3dNoiseTexture()
{
  startTimer(16);
  setFocusPolicy(Qt::StrongFocus);
}

Viewer::~Viewer()
{}

//-----------------------------------------------------------------------------
void Viewer::drawPlatonicSolid(const PlatonicSolid& iPs)
{
  PlatonicSolid::Face f;
  for(unsigned int i = 0; i < iPs.getFaces().size(); ++i)
  {
    f = iPs.getFaces()[i];
//    Vector3d n = Vector3d(iPs.getVertex()[f.index1], iPs.getVertex()[f.index2]) ^
//      Vector3d(iPs.getVertex()[f.index1], iPs.getVertex()[f.index3]);
//    n.normalise();
    Vector3d n1(Point3d(0.0), iPs.getVertex()[f.index1]);
    n1.normalise();

    Vector3d n2(Point3d(0.0), iPs.getVertex()[f.index2]);
    n2.normalise();

    Vector3d n3(Point3d(0.0), iPs.getVertex()[f.index3]);
    n3.normalise();
  	glBegin(GL_TRIANGLES);
      //glNormal3d(n.getX(), n.getY(), n.getZ());
      glNormal3d(n1.getX(), n1.getY(), n1.getZ());
      glVertex3d(iPs.getVertex()[f.index1].getX(),
        iPs.getVertex()[f.index1].getY(),
        iPs.getVertex()[f.index1].getZ());
      
      glNormal3d(n2.getX(), n2.getY(), n2.getZ());  
      glVertex3d(iPs.getVertex()[f.index2].getX(),
        iPs.getVertex()[f.index2].getY(),
        iPs.getVertex()[f.index2].getZ());
        
			glNormal3d(n3.getX(), n3.getY(), n3.getZ());
      glVertex3d(iPs.getVertex()[f.index3].getX(),
        iPs.getVertex()[f.index3].getY(),
        iPs.getVertex()[f.index3].getZ());
    glEnd();
  }
}

void Viewer::initializeGL()
{
  Widget3d::initializeGL();
  
  QString directionalLightFrag, directionalLightVert;
  QString fragmentSource, vertexSource;
  
  //--- lighting shader
  QFile vert(":/directional lighting.vert");
  vert.open(QIODevice::ReadOnly);
  directionalLightVert = vert.readAll();

  QFile frag(":/directional lighting.frag");
  frag.open(QIODevice::ReadOnly);
  directionalLightFrag = frag.readAll();
  
  frag.close();
  frag.setFileName(":/main with light.vert");
  frag.open(QIODevice::ReadOnly);
  fragmentSource = frag.readAll();
  
  frag.close();
  frag.setFileName(":/main with light.frag");
  frag.open(QIODevice::ReadOnly);
  QString fragmentSource2 = frag.readAll();

//  qDebug(vertexSource.toStdString().c_str());
//  qDebug(fragmentSource.toStdString().c_str());
//  qDebug(fragmentSource2.toStdString().c_str());
  mShader.addVertexShaderSource(directionalLightVert);
  mShader.addVertexShaderSource(fragmentSource);
  mShader.addFragmentShaderSource(directionalLightFrag);
  mShader.addFragmentShaderSource(fragmentSource2);
  mShader.link();

  //--- toon shader  
  vert.close();
  vert.setFileName(":/toon.vert");
  vert.open(QIODevice::ReadOnly);
  vertexSource = vert.readAll();
  
  frag.close();
  frag.setFileName(":/toon.frag");
  frag.open(QIODevice::ReadOnly);
  fragmentSource = frag.readAll();
  
//  qDebug(vertexSource.toStdString().c_str());
//  qDebug(fragmentSource.toStdString().c_str());
  mShader2.addVertexShaderSource(vertexSource);
  mShader2.addFragmentShaderSource(fragmentSource);
  mShader2.link();
  
  //--- Noise shader
  vert.close();
  vert.setFileName(":/noise.vert");
  vert.open(QIODevice::ReadOnly);
  vertexSource = vert.readAll();
  
  frag.close();
  frag.setFileName(":/noise.frag");
  frag.open(QIODevice::ReadOnly);
  fragmentSource = frag.readAll();
  
  //  qDebug(vertexSource.toStdString().c_str());
  //  qDebug(fragmentSource.toStdString().c_str());
  mNoiseShader.addVertexShaderSource(vertexSource);
  mNoiseShader.addVertexShaderSource(directionalLightVert);
  mNoiseShader.addFragmentShaderSource(fragmentSource);
  mNoiseShader.addFragmentShaderSource(directionalLightFrag);
  mNoiseShader.link();
  m3dNoiseTexture = treeD::utilities::get3dNoiseTexture(Vector3i(128, 128, 128));
  
  //--- sun shader
  vert.close();
  vert.setFileName(":/noise.vert");
  vert.open(QIODevice::ReadOnly);
  vertexSource = vert.readAll();
  
  frag.close();
  frag.setFileName(":/sunSurface.frag");
  frag.open(QIODevice::ReadOnly);
  fragmentSource = frag.readAll();
  
  //  qDebug(vertexSource.toStdString().c_str());
  //  qDebug(fragmentSource.toStdString().c_str());
  mSunShader.addVertexShaderSource(vertexSource);
  mSunShader.addVertexShaderSource(directionalLightVert);
  mSunShader.addFragmentShaderSource(fragmentSource);
  mSunShader.addFragmentShaderSource(directionalLightFrag);
  mSunShader.link();
}

//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
  Widget3d::paintGL();
  
  pushShader(mShader);
  drawPlatonicSolid(mPs);

  pushShader(mShader2);
  glPushMatrix();
  glTranslated(2, 0, 0);
  drawPlatonicSolid(mPs);
  glPopMatrix();
  popShader();
  
  glPushMatrix();
  glTranslated(4, 0, 0);
  drawPlatonicSolid(mPs);
  glPopMatrix();
  
  
  glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, m3dNoiseTexture.getTextureId());
    pushShader(mNoiseShader);
    mNoiseShader.setUniform("Noise", 0);
    glPushMatrix();
    glTranslated(6, 0, 0);
    drawPlatonicSolid(mPs);
    glPopMatrix();
    
    pushShader(mSunShader);
    mSunShader.setUniform("Noise", 0);
    glPushMatrix();
    glTranslated(8, 0, 0);
    drawPlatonicSolid(mPs);
    glPopMatrix();
    popShader();
  glPopAttrib();
  
  popShader();
  popShader();
}

//-----------------------------------------------------------------------------
void Viewer::timerEvent(QTimerEvent* ipEvent)
{
  updateGL();
}

//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QDialog(),
  mpViewer(0)
{
  resize(800, 600);
  
  QHBoxLayout* pLyt = new QHBoxLayout(this);
  pLyt->setMargin(5);
  mpViewer = new Viewer(this);
  mpViewer->setCameraOrientation(Camera::FREE);
  pLyt->addWidget(mpViewer);
  
  Camera c = mpViewer->getCamera();
  Matrix4d m;
  m.setTranslation(Point3d(8.0, 0.0, 0.0));
  c.setTransformationToGlobal(m);
  mpViewer->setCamera(c, false);
}