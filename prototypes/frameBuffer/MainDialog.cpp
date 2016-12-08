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
    
    float e = 2.718f;
    float sigma = 0.4f;
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
    
    
    void drawRectangleOgl2_1(const Point2d& iO, const Vector2d& iS)
    {
        glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0);
        glVertex2d(iO.x(), iO.y());
        
        glTexCoord2d(1.0, 0.0);
        glVertex2d(iO.x() + iS.x(), iO.y());
        
        glTexCoord2d(1.0, 1.0);
        glVertex2d(iO.x() + iS.x(), iO.y() + iS.y());
        
        glTexCoord2d(0.0, 1.0);
        glVertex2d(iO.x(), iO.y() + iS.y());
        glEnd();
    }
    
    void applyViewProjection(const Matrix4& iView, const Matrix4& iProjection)
    {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadMatrixd(iProjection.getDataPointer());
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixd(iView.getDataPointer());
    }
    
    void popViewProjection()
    {
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }
}

Viewer::Viewer(QWidget* ipParent /*=0*/) : Widget3d(ipParent),
mPs(PlatonicSolid::tIsocahedron),
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
    
    int viewportWidth = getCamera().getViewport().getWidth() / 4;
    int viewportHeight = getCamera().getViewport().getHeight() / 4;
    
    Vector2d screenSize = getCamera().getViewport().getSize();
    Vector2d offScreenSize(viewportWidth, viewportHeight);
    
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    
    pushFrameBuffer(mFbo);
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_VIEWPORT_BIT);
    glDisable(GL_LIGHTING);
    glViewport(0, 0, offScreenSize.x(), offScreenSize.y());
    mFbo.resize(offScreenSize.x(), offScreenSize.y());
    mFbo.drawTo(0);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    pushShader(mOnlyOneTextureShader);
    mOnlyOneTextureShader.setUniform("texture", 1);
    drawScene();
    popShader();
    
    Texture t1 = mFbo.getColorAttachment(0);
    Texture depthTexture = mFbo.getDepthAttachment().copy();
    //QImage _i = mFbo.getImageFrom(0);
    //bool _a = _i.save("/Users/po/Desktop/0.png", "PNG");
    mFbo.drawTo(1);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    pushShader(mBlurShader);
    mBlurShader.setUniform("texture", 0);
    mBlurShader.setUniform("isBlurVertical", false);
    mBlurShader.setUniform("kernel", kernel);
    mBlurShader.setUniform("kernelValues", kernel, &kernelValues[0]);
    mBlurShader.setUniform("scale", kScale);
    {
        ScreenSpaceProjection ssp( offScreenSize );
        applyViewProjection(ssp.mViewMatrix, ssp.mProjectionMatrix);
        glBindTexture(GL_TEXTURE_2D, t1.getId());
        drawRectangleOgl2_1(Point2d(0.0), offScreenSize );
        popViewProjection();
    }
    
    Texture t2 = mFbo.getColorAttachment(1);
    //_i = mFbo.getImageFrom(1);
    //_a = _i.save("/Users/po/Desktop/1.png", "PNG");
    mFbo.drawTo(2);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mBlurShader.setUniform("texture", 0);
    mBlurShader.setUniform("isBlurVertical", true);
    mBlurShader.setUniform("kernel", kernel);
    mBlurShader.setUniform("kernelValues", kernel, &kernelValues[0]);
    mBlurShader.setUniform("scale", kScale);
    {
        ScreenSpaceProjection ssp( offScreenSize );
        applyViewProjection(ssp.mViewMatrix, ssp.mProjectionMatrix);
        glBindTexture(GL_TEXTURE_2D, t2.getId());
        drawRectangleOgl2_1(Point2d(0.0), offScreenSize );
        popViewProjection();
    }
    popShader();
    glPopAttrib();
    popFrameBuffer();
    
    Texture t3 = mFbo.getColorAttachment(2);
    //i = mFbo.getImageFrom(2);
    //a = i.save("/Users/po/Desktop/2.png", "PNG");
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4d(0.0, 0.0, 0.0, 0.4);
    {
        ScreenSpaceProjection ssp( offScreenSize );
        applyViewProjection(ssp.mViewMatrix, ssp.mProjectionMatrix);
        glBindTexture(GL_TEXTURE_2D, t3.getId());
        drawRectangleOgl2_1(Point2d(0.0), offScreenSize );
        popViewProjection();
    }
    
    glDisable(GL_DEPTH_TEST);
    glDisable( GL_LIGHTING );
    glColor3ub(255, 255, 255);
    {
        ScreenSpaceProjection ssp( screenSize );
        applyViewProjection(ssp.mViewMatrix, ssp.mProjectionMatrix);
        
        glBindTexture(GL_TEXTURE_2D, t1.getId());
        drawRectangleOgl2_1( Point2d(5.0), offScreenSize );
        
        glBindTexture(GL_TEXTURE_2D, t2.getId());
        drawRectangleOgl2_1( Point2d(10.0 + offScreenSize.x(), 5.0), offScreenSize );
        
        glBindTexture(GL_TEXTURE_2D, t3.getId());
        drawRectangleOgl2_1( Point2d(15.0 + 2 * offScreenSize.x(), 5.0), offScreenSize );
        
        glBindTexture(GL_TEXTURE_2D, depthTexture.getId());
        drawRectangleOgl2_1( Point2d(20.0 + 3 * offScreenSize.x(), 5.0), offScreenSize );
        
        popViewProjection();
    }
    
    glPopAttrib();
}

//------------------------------------------------------------------------------
void Viewer::initDisplayList()
{
    QImage a(":/images/three.png");
    //QImage a("/Users/po/work/prototypes/frameBuffer/images/three.tif");
    assert(!a.isNull());
    //mTexture.set(QImage("/Users/po/work/prototypes/frameBuffer/images/three.tif"));
    mTexture.set(QImage(":/images/three.png"), GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
    mTextureBlur.set(QImage(":/images/three_blur.png"), GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
    mTexture2.set(QImage(":/images/three_red.png"), GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
    mTextureBlur2.set(QImage(":/images/three_red_blur.png"), GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
    
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
    glBindTexture(GL_TEXTURE_2D, mTexture.getId());
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mTextureBlur.getId());
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
    glBindTexture(GL_TEXTURE_2D, mTexture2.getId());
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mTextureBlur2.getId());
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
    
    mFbo.addColorAttachment();
    mFbo.addColorAttachment();
    mFbo.addColorAttachment();
    mFbo.addDepthAttachment();
    
    //change texture param
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mFbo.getColorAttachment(2).getId());
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glBindTexture(GL_TEXTURE_2D, 0);
    
    QFile f(":/shaders/main.frag");
    f.open(QIODevice::ReadOnly);
    mBlurShader.addFragmentSource(f.readAll());
    f.close();
    
    f.setFileName(":/shaders/blur.frag");
    f.open(QIODevice::ReadOnly);
    mBlurShader.addFragmentSource(f.readAll());
    mBlurShader.link();
    f.close();
    
    f.setFileName(":/shaders/onlyOneTexture.frag");
    f.open(QIODevice::ReadOnly);
    mOnlyOneTextureShader.addFragmentSource(f.readAll());
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
    Widget3d::timerEvent( ipEvent );
    
    //0 a 0.2;
    if(kCounter >= 180 || kCounter < 60 )
        kStep *= -1;
    kCounter += kStep;
    kScale = (kCounter) / 1000.0;
    
    update();
}

//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QMainWindow(),
mpViewer(0)
{
    resize(800, 600);
    
    QHBoxLayout* pLyt = new QHBoxLayout(this);
    pLyt->setMargin(5);
    mpViewer = new Viewer(this);
    pLyt->addWidget(mpViewer);
    setCentralWidget(mpViewer);
    
    mpViewer->setControlType( Widget3d::ctRotateAround );
    Camera c = mpViewer->getCamera();
    c.set( Point3d(0.0, 0.0, 1),
          Point3d(), Vector3d(0, 1, 0) );
    mpViewer->setCamera( c, false ); 
}