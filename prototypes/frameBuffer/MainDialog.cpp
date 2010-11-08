/*
 *  MainWindow.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 08/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "3d/Camera.h"
#include "3d/Sprite.h"
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
	float kScale = 0.0;
  int kCounter = 60;
  int kStep = 4;
  
  Matrix4d kRot = getRotationMatrix(PI/300.0, Vector3d(1.0, 0.0, 0.0)) *
    getRotationMatrix(PI/480.0, Vector3d(0.0, 1.0, 0.0));

  float e = 2.718;
  float sigma = 0.4;
  float sigmaSquare = sigma*sigma;
  float normalDistribution(float iValue)
  {
    float r = (1.0/sqrt(2.0f*3.1415629f*sigmaSquare)) * pow(e, -(iValue*iValue/(2.0f*sigmaSquare)));
    return r;
  }
  
  int kernel = 25;
  float kernelValues[25];
  void initGaussianKernel()
  {
    int j = 0;
    int i;
    for(i = -kernel / 2; i <= kernel / 2; ++i)
    {
      kernelValues[j] = normalDistribution(i / float(kernel / 2));
      ++j;
    }
  }

}

Viewer::Viewer(QWidget* ipParent /*=0*/) : Widget3d(ipParent),
 mPs(PlatonicSolid::tIsocahedron, 3),
 mFbo(),
 mBlurShader(),
 mOnlyOneTextureShader(),
 mTexture(),
 mTextureBlur(),
 mTexture2(),
 mTextureBlur2(),
 mBoxDisplayList(0),
 mBoxDisplayList2(0)
{
  setFocusPolicy(Qt::StrongFocus);
  initGaussianKernel();
  startTimer(15);
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

//------------------------------------------------------------------------------
void Viewer::drawScene()
{
  for(int i = -5; i <= 5; ++i)
    for(int j = -5; j <= 5; ++j)
      for(int k = -5; k <= 5; ++k)
    {
      glPushMatrix();
      glTranslated(i * 2.5, j * 2.5, k * 2.5);
      if(i % 5 != 0 || j % 3 != 0 || k % 5 != 0)
        glCallList(mBoxDisplayList);
      else
        glCallList(mBoxDisplayList2);
      glPopMatrix();
    }
}

//------------------------------------------------------------------------------
void Viewer::drawGlowOverLay()
{
  //qDebug(QString("storage").toAscii());
  //for(i = 0; i < kernel; ++i)
  //  qDebug(QString::number(kernelValues[i]).toAscii());
    
  int viewportWidth = getCamera().getWindowInfo().getWidth() / 4;
  int viewportHeight = getCamera().getWindowInfo().getHeight() / 4;
  
  pushFrameBuffer(mFbo);
  glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_VIEWPORT_BIT);
  glDisable(GL_LIGHTING);
  glViewport(0, 0, viewportWidth, viewportHeight);
  mFbo.resize(viewportWidth, viewportHeight);
  mFbo.drawTo(0);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  pushShader(mOnlyOneTextureShader);
  mOnlyOneTextureShader.setUniform("texture", 1);
  drawScene();
  popShader();
  
  Texture t1 = mFbo.getTexture(0);
  //QImage _i = mFbo.getImageFrom(0);
  //bool _a = _i.save("/Users/po/Desktop/0.png", "PNG");
  mFbo.drawTo(1);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  Sprite s1;
  s1.setTexture(t1);
  s1.setFullScreen(true);
  pushShader(mBlurShader);
  mBlurShader.setUniform("texture", 0);
  mBlurShader.setUniform("isBlurVertical", false);
  mBlurShader.setUniform("kernel", kernel);
  mBlurShader.setUniform("kernelValues", kernel, &kernelValues[0]);
  mBlurShader.setUniform("scale", kScale);
	s1.draw(getCamera());
  
  Texture t2 = mFbo.getTexture(1);
  //_i = mFbo.getImageFrom(1);
  //_a = _i.save("/Users/po/Desktop/1.png", "PNG");
  mFbo.drawTo(2);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  Sprite s2;
  s2.setTexture(t2);
  s2.setFullScreen(true);
  mBlurShader.setUniform("texture", 0);
  mBlurShader.setUniform("isBlurVertical", true);
  mBlurShader.setUniform("kernel", kernel);
  mBlurShader.setUniform("kernelValues", kernel, &kernelValues[0]);
  mBlurShader.setUniform("scale", kScale);
	s2.draw(getCamera());
  popShader();
  glPopAttrib();
  popFrameBuffer();
  
  Texture t3 = mFbo.getTexture(2);
  //i = mFbo.getImageFrom(2);
  //a = i.save("/Users/po/Desktop/2.png", "PNG");
  glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4d(0.0, 0.0, 0.0, 0.4);
  Sprite s3;
  s3.setTexture(t3);
s3.setFullScreen(true);
//  s3.setAnchorPoint(Sprite::aTopLeft);
//  s3.set2dPositioningOn(true);
// s3.set2dPosition(10, 10, 0);
	s3.draw(getCamera());
  glPopAttrib();
}

//------------------------------------------------------------------------------
void Viewer::initDisplayList()
{
  QImage a(":/images/three.png");
  //QImage a("/Users/po/work/prototypes/frameBuffer/images/three.tif");
  assert(!a.isNull());
	//mTexture.set(QImage("/Users/po/work/prototypes/frameBuffer/images/three.tif"));
  mTexture.set(QImage(":/images/three.png"));
  mTextureBlur.set(QImage(":/images/three_blur.png"));
  mTexture2.set(QImage(":/images/three_red.png"));
  mTextureBlur2.set(QImage(":/images/three_red_blur.png"));
  glBindTexture(GL_TEXTURE_2D, mTexture.getTextureId());
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindTexture(GL_TEXTURE_2D, mTextureBlur.getTextureId());
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindTexture(GL_TEXTURE_2D, mTexture2.getTextureId());
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindTexture(GL_TEXTURE_2D, mTextureBlur2.getTextureId());
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glBindTexture(GL_TEXTURE_2D, 0);
  
  float t0[2] = { 0.0f, 0.0f};
  float t1[2] = { 0.0f, 1.0f};
  float t2[2] = { 1.0f, 1.0f};
  float t3[2] = { 1.0f, 0.0f};

  float v0[3] = { 1.0f, 1.0f, 0.0f};
  float v1[3] = { 0.0f, 1.0f, 0.0f};
  float v2[3] = { 0.0f, 1.0f, 1.0f};
  float v3[3] = { 1.0f, 1.0f, 1.0f};

  float v4[3] = { 1.0f, 0.0f, 0.0f};
  float v5[3] = { 0.0f, 0.0f, 0.0f};
  float v6[3] = { 0.0f, 0.0f, 1.0f};
  float v7[3] = { 1.0f, 0.0f, 1.0f};
  
  mBoxDisplayList = glGenLists(1);
  glNewList(mBoxDisplayList, GL_COMPILE);
  
  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, mTexture.getTextureId());
  glActiveTexture(GL_TEXTURE1);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, mTextureBlur.getTextureId());
  glBegin(GL_QUADS);
    //top
    glNormal3d(0.0, 1.0, 0.0);
    glMultiTexCoord2fv(GL_TEXTURE0, t0);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v0);
    glMultiTexCoord2fv(GL_TEXTURE0, t1);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v1);
    glMultiTexCoord2fv(GL_TEXTURE0, t2);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v2);
    glMultiTexCoord2fv(GL_TEXTURE0, t3);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v3);
    
    //bottom
    glNormal3d(0.0, -1.0, 0.0);
    glMultiTexCoord2fv(GL_TEXTURE0, t0);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v4);
    glMultiTexCoord2fv(GL_TEXTURE0, t1);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v7);
    glMultiTexCoord2fv(GL_TEXTURE0, t2);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v6);
    glMultiTexCoord2fv(GL_TEXTURE0, t3);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v5);
    
    //right
    glNormal3d(1.0, 0.0, 0.0);
    glMultiTexCoord2fv(GL_TEXTURE0, t0);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v4);
    glMultiTexCoord2fv(GL_TEXTURE0, t1);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v0);
    glMultiTexCoord2fv(GL_TEXTURE0, t2);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v3);
    glMultiTexCoord2fv(GL_TEXTURE0, t3);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v7);
    
    //left
    glNormal3d(-1.0, 0.0, 0.0);
    glMultiTexCoord2fv(GL_TEXTURE0, t0);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v5);
    glMultiTexCoord2fv(GL_TEXTURE0, t1);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v6);
    glMultiTexCoord2fv(GL_TEXTURE0, t2);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v2);
    glMultiTexCoord2fv(GL_TEXTURE0, t3);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v1);
    
    //front
    glNormal3d(0.0, 0.0, 1.0);
    glMultiTexCoord2fv(GL_TEXTURE0, t0);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v4);
    glMultiTexCoord2fv(GL_TEXTURE0, t1);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v5);
    glMultiTexCoord2fv(GL_TEXTURE0, t2);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v1);
    glMultiTexCoord2fv(GL_TEXTURE0, t3);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v0);
    
    //back
    glNormal3d(0.0, 0.0, -1.0);
    glMultiTexCoord2fv(GL_TEXTURE0, t0);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v7);
    glMultiTexCoord2fv(GL_TEXTURE0, t1);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v3);
    glMultiTexCoord2fv(GL_TEXTURE0, t2);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v2);
    glMultiTexCoord2fv(GL_TEXTURE0, t3);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v6);
        
  glEnd();
  
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
  glEndList();
  
  mBoxDisplayList2 = glGenLists(1);
  glNewList(mBoxDisplayList2, GL_COMPILE);
  
  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, mTexture2.getTextureId());
  glActiveTexture(GL_TEXTURE1);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, mTextureBlur2.getTextureId());
  glBegin(GL_QUADS);
    //top
    glNormal3d(0.0, 1.0, 0.0);
    glMultiTexCoord2fv(GL_TEXTURE0, t0);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v0);
    glMultiTexCoord2fv(GL_TEXTURE0, t1);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v1);
    glMultiTexCoord2fv(GL_TEXTURE0, t2);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v2);
    glMultiTexCoord2fv(GL_TEXTURE0, t3);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v3);
    
    //bottom
    glNormal3d(0.0, -1.0, 0.0);
    glMultiTexCoord2fv(GL_TEXTURE0, t0);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v4);
    glMultiTexCoord2fv(GL_TEXTURE0, t1);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v7);
    glMultiTexCoord2fv(GL_TEXTURE0, t2);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v6);
    glMultiTexCoord2fv(GL_TEXTURE0, t3);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v5);
    
    //right
    glNormal3d(1.0, 0.0, 0.0);
    glMultiTexCoord2fv(GL_TEXTURE0, t0);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v4);
    glMultiTexCoord2fv(GL_TEXTURE0, t1);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v0);
    glMultiTexCoord2fv(GL_TEXTURE0, t2);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v3);
    glMultiTexCoord2fv(GL_TEXTURE0, t3);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v7);
    
    //left
    glNormal3d(-1.0, 0.0, 0.0);
    glMultiTexCoord2fv(GL_TEXTURE0, t0);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v5);
    glMultiTexCoord2fv(GL_TEXTURE0, t1);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v6);
    glMultiTexCoord2fv(GL_TEXTURE0, t2);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v2);
    glMultiTexCoord2fv(GL_TEXTURE0, t3);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v1);
    
    //front
    glNormal3d(0.0, 0.0, 1.0);
    glMultiTexCoord2fv(GL_TEXTURE0, t0);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v4);
    glMultiTexCoord2fv(GL_TEXTURE0, t1);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v5);
    glMultiTexCoord2fv(GL_TEXTURE0, t2);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v1);
    glMultiTexCoord2fv(GL_TEXTURE0, t3);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v0);
    
    //back
    glNormal3d(0.0, 0.0, -1.0);
    glMultiTexCoord2fv(GL_TEXTURE0, t0);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v7);
    glMultiTexCoord2fv(GL_TEXTURE0, t1);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v3);
    glMultiTexCoord2fv(GL_TEXTURE0, t2);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v2);
    glMultiTexCoord2fv(GL_TEXTURE0, t3);
    glMultiTexCoord2fv(GL_TEXTURE1, t0);
    glVertex3fv(v6);
        
  glEnd();
  
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
  glEndList();
}

//------------------------------------------------------------------------------
void Viewer::initializeGL()
{
  Widget3d::initializeGL();
  glClearColor(0.0, 0.0, 0.0, 0.0);
  
  mFbo.addColorAttachment(true);
  mFbo.addColorAttachment(true);
  mFbo.addColorAttachment(true);  
  mFbo.addDepthAttachment(true);
  
  //change texture param
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, mFbo.getTexture(2).getTextureId());
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
  glBindTexture(GL_TEXTURE_2D, 0);
  
  QFile f(":/shaders/main.frag");
  f.open(QIODevice::ReadOnly);
  mBlurShader.addFragmentShaderSource(f.readAll());
  f.close();
  
  f.setFileName(":/shaders/blur.frag");
  f.open(QIODevice::ReadOnly);
  mBlurShader.addFragmentShaderSource(f.readAll());
  mBlurShader.link();
  f.close();
  
  f.setFileName(":/shaders/onlyOneTexture.frag");
  f.open(QIODevice::ReadOnly);
  mOnlyOneTextureShader.addFragmentShaderSource(f.readAll());
  mOnlyOneTextureShader.link();
  f.close();
  
  //initialize les liste d'affichage
  initDisplayList();
}

//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
  Widget3d::paintGL();
  
  pushShader(mOnlyOneTextureShader);
  mOnlyOneTextureShader.setUniform("texture", 0);
  drawScene();
  popShader();
  
  drawGlowOverLay();
  
  showFps();
}

//-----------------------------------------------------------------------------
void Viewer::resizeGL(int iWidth, int iHeight)
{
  Widget3d::resizeGL(iWidth, iHeight);
  mFbo.resize(iWidth, iHeight);
}


//-----------------------------------------------------------------------------
void Viewer::timerEvent(QTimerEvent* ipEvent)
{
  //0 a 0.2;
  if(kCounter >= 180 || kCounter < 60 )
    kStep *= -1;
  kCounter += kStep;
  kScale = (kCounter) / 1000.0;
  
  Camera c = getCamera();
  Matrix4d m = c.getTransformationToGlobal() * kRot;
  c.setTransformationToGlobal(m);
  setCamera(c, false);
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
  mpViewer->setCameraOrientation(Camera::FREE);
  pLyt->addWidget(mpViewer);
  setCentralWidget(mpViewer);
  
  Camera c = mpViewer->getCamera();
  c.setPos(Point3d(10.0, 10.0, 10.0));
  Matrix4d m;
  m.setTranslation(Point3d(0.5, 0.5, 0.5));
  c.setTransformationToGlobal(m);
  mpViewer->setCamera(c, false);
}