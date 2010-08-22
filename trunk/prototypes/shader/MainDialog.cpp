/*
 *  MainWindow.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 08/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "MainDialog.h"
#include <qlayout.h>
#include "3d/Camera.h"
#include <QFile.h>

using namespace realisim;
  using namespace math;
  using namespace treeD;

Viewer::Viewer(QWidget* ipParent /*=0*/) : Widget3d(ipParent),
 mPs(PlatonicSolid::tIsocahedron, 3),
 mShader(),
 mShader2()
{
  startTimer(16);
}

Viewer::~Viewer()
{}

//-----------------------------------------------------------------------------
void Viewer::drawPlatonicSolid(const PlatonicSolid& iPs)
{
  PlatonicSolid::Face f;
  for(unsigned int i = 0; i < iPs.getFaces().size(); ++i)
  {
    f = mPs.getFaces()[i];
//    Vector3d n = Vector3d(mPs.getVertex()[f.index2], mPs.getVertex()[f.index1]) ^
//      Vector3d(mPs.getVertex()[f.index2], mPs.getVertex()[f.index3]);
//    n.normalise();
    Vector3d n1(Point3d(0.0), mPs.getVertex()[f.index1]);
    n1.normalise();

    Vector3d n2(Point3d(0.0), mPs.getVertex()[f.index2]);
    n2.normalise();

    Vector3d n3(Point3d(0.0), mPs.getVertex()[f.index3]);
    n3.normalise();
  	glBegin(GL_TRIANGLES);
      glNormal3d(n1.getX(), n1.getY(), n1.getZ());
      glVertex3d(mPs.getVertex()[f.index1].getX(),
        mPs.getVertex()[f.index1].getY(),
        mPs.getVertex()[f.index1].getZ());
      
      glNormal3d(n2.getX(), n2.getY(), n2.getZ());  
      glVertex3d(mPs.getVertex()[f.index2].getX(),
        mPs.getVertex()[f.index2].getY(),
        mPs.getVertex()[f.index2].getZ());
        
			glNormal3d(n3.getX(), n3.getY(), n3.getZ());
      glVertex3d(mPs.getVertex()[f.index3].getX(),
        mPs.getVertex()[f.index3].getY(),
        mPs.getVertex()[f.index3].getZ());
    glEnd();
  }
}

void Viewer::initializeGL()
{
  Widget3d::initializeGL();
  
  QString fragmentSource, vertexSource;
  
  //--- lighting shader
  QFile vert(":/directional lighting.vert");
  vert.open(QIODevice::ReadOnly);
  vertexSource = vert.readAll();

  QFile frag(":/directional lighting.frag");
  frag.open(QIODevice::ReadOnly);
  fragmentSource = frag.readAll();
  
  frag.close();
  frag.setFileName(":/main with light.frag");
  frag.open(QIODevice::ReadOnly);
  QString fragmentSource2 = frag.readAll();

  qDebug(vertexSource.toStdString().c_str());
  qDebug(fragmentSource.toStdString().c_str());
  qDebug(fragmentSource2.toStdString().c_str());
  mShader.addVertexShaderSource(vertexSource);
  mShader.addFragmentShaderSource(fragmentSource);
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
  
  qDebug(vertexSource.toStdString().c_str());
  qDebug(fragmentSource.toStdString().c_str());
  mShader2.addVertexShaderSource(vertexSource);
  mShader2.addFragmentShaderSource(fragmentSource);
  mShader2.link();  
}

//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
  Widget3d::paintGL();
  
  pushShader(mShader);
  mShader.setUniform("myInput", Vector3f(1.0, 0.0, 0.0));
  drawPlatonicSolid(mPs);

  pushShader(mShader2);
  mShader.setUniform("myInput", Vector3f(0.0, 0.0, 1.0));
  glPushMatrix();
  glTranslated(5, 10, 3);
  drawPlatonicSolid(mPs);
  glPopMatrix();
  popShader();
  
  glPushMatrix();
  glTranslated(11, 2, 8);
  mShader.setUniform("myInput", Vector3f(0.0, 1.0, 0.0));
  drawPlatonicSolid(mPs);
  glPopMatrix();
  
  pushShader(mShader2);
  mShader.setUniform("myInput", Vector3f(0.0, 1.0, 1.0));
  glPushMatrix();
  glTranslated(-4, -2, -3);
  drawPlatonicSolid(mPs);
  glPopMatrix();
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
}