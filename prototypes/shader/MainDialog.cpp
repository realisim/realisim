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
 mPs(PlatonicSolid::tIsocahedron),
 mShader(),
 mShader2(),
 mNoiseShader(),
 mSunShader(),
 mRayCastShader(),
 m3dNoiseTexture()
{
  //startTimer(16);
  setFocusPolicy(Qt::StrongFocus);
}

Viewer::~Viewer()
{}

//-----------------------------------------------------------------------------
//void Viewer::drawPlatonicSolid(const PlatonicSolid& iPs)
//{
//  PlatonicSolid::Face f;
//  for(unsigned int i = 0; i < iPs.getFaces().size(); ++i)
//  {
//    f = iPs.getFaces()[i];
////    Vector3d n = Vector3d(iPs.getVertex()[f.index1], iPs.getVertex()[f.index2]) ^
////      Vector3d(iPs.getVertex()[f.index1], iPs.getVertex()[f.index3]);
////    n.normalise();
//    Vector3d n1(Point3d(0.0), iPs.getVertex()[f.index1]);
//    n1.normalise();
//
//    Vector3d n2(Point3d(0.0), iPs.getVertex()[f.index2]);
//    n2.normalise();
//
//    Vector3d n3(Point3d(0.0), iPs.getVertex()[f.index3]);
//    n3.normalise();
//  	glBegin(GL_TRIANGLES);
//      //glNormal3d(n.x(), n.y(), n.z());
//      glNormal3d(n1.x(), n1.y(), n1.z());
//      glVertex3d(iPs.getVertex()[f.index1].x(),
//        iPs.getVertex()[f.index1].y(),
//        iPs.getVertex()[f.index1].z());
//      
//      glNormal3d(n2.x(), n2.y(), n2.z());  
//      glVertex3d(iPs.getVertex()[f.index2].x(),
//        iPs.getVertex()[f.index2].y(),
//        iPs.getVertex()[f.index2].z());
//        
//			glNormal3d(n3.x(), n3.y(), n3.z());
//      glVertex3d(iPs.getVertex()[f.index3].x(),
//        iPs.getVertex()[f.index3].y(),
//        iPs.getVertex()[f.index3].z());
//    glEnd();
//  }
//}

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
  mShader.addVertexSource(directionalLightVert);
  mShader.addVertexSource(fragmentSource);
  mShader.addFragmentSource(directionalLightFrag);
  mShader.addFragmentSource(fragmentSource2);
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
  mShader2.addVertexSource(vertexSource);
  mShader2.addFragmentSource(fragmentSource);
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
  mNoiseShader.addVertexSource(vertexSource);
  mNoiseShader.addVertexSource(directionalLightVert);
  mNoiseShader.addFragmentSource(fragmentSource);
  mNoiseShader.addFragmentSource(directionalLightFrag);
  mNoiseShader.link();
  m3dNoiseTexture = treeD::get3dNoiseTexture(Vector3i(128, 128, 128));
  
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
  mSunShader.addVertexSource(vertexSource);
  mSunShader.addVertexSource(directionalLightVert);
  mSunShader.addFragmentSource(fragmentSource);
  mSunShader.addFragmentSource(directionalLightFrag);
  mSunShader.link();
}

//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
  Widget3d::paintGL();
  
  pushShader(mShader);
  treeD::draw(mPs);

  pushShader(mShader2);
  glPushMatrix();
  glTranslated(2, 0, 0);
  treeD::draw(mPs);
  glPopMatrix();
  popShader();
  
  glPushMatrix();
  glTranslated(4, 0, 0);
  treeD::draw(mPs);
  glPopMatrix();
  
  
  glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, m3dNoiseTexture.getId());
    pushShader(mNoiseShader);
    mNoiseShader.setUniform("Noise", 0);
    glPushMatrix();
    glTranslated(6, 0, 0);
    treeD::draw(mPs);
    glPopMatrix();
    
    pushShader(mSunShader);
    mSunShader.setUniform("Noise", 0);
    glPushMatrix();
    glTranslated(8, 0, 0);
    treeD::draw(mPs);
    glPopMatrix();
    popShader();
  glPopAttrib();
  
  popShader();
  popShader();
}

//-----------------------------------------------------------------------------
void Viewer::timerEvent(QTimerEvent* ipEvent)
{
	Widget3d::timerEvent( ipEvent );
  updateGL();
}

//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QMainWindow(),
  mpViewer(0)
{
  resize(800, 600);
  
  QHBoxLayout* pLyt = new QHBoxLayout(this);
  pLyt->setMargin(5);
  mpViewer = new Viewer(this);
  pLyt->addWidget(mpViewer, 1);
  setCentralWidget(mpViewer);

	Camera c = mpViewer->getCamera();
  c.set( Point3d(0.0, 0.0, 100),
  	Point3d(), Vector3d(0, 1, 0) );
  mpViewer->setCamera( c );
	mpViewer->setControlType( Widget3d::ctFree );
}