/**/

#include <3d/Camera.h>
#include <3d/Utilities.h>
#include <math/Point.h>
#include <math/Vect.h>
#include "MainDialog.h"
#include <QCoreApplication>
#include <QHBoxLayout>

//include jiminez smaa lookup textures
#include <JimenezSmaa/71c806a/include/AreaTex.h>
#include <JimenezSmaa/71c806a/include/SearchTex.h>

using namespace realisim;
using namespace math;
using namespace treeD;

namespace
{
    double gRotx = 0.0;
    double gRoty = 0.0;
    double gRotz = 0.0;

    bool gRotateCube = false;
    
    bool gMlaaActivated = true;
    int gFrameBufferToDisplay = 0;

    enum sceneContent{ sc3d = 0, scUnigine01, scUnigine02, scContentCount };
    sceneContent gSceneContent = sc3d;
}

Viewer::Viewer(QWidget* ipParent, MainDialog* ipM) : Widget3d(ipParent),
    mpMainDialog(ipM)
{
	setFocusPolicy(Qt::StrongFocus);
}

Viewer::~Viewer()
{}

//-----------------------------------------------------------------------------
//draws a rectancle in screenspace starting at 0.0 to size. If a texure
//isbound to GL_TEXTURE0, the rectangle will be textured
void Viewer::blitToScreen(Vector2d size)
{
    ScreenSpaceProjection sp(size);
    {        
        drawRectangle(Point2d(0.0), size);
    }
}

//-----------------------------------------------------------------------------
void Viewer::draw()
{
	Widget3d::draw();

    if(!gMlaaActivated)
    {
        drawScene();
    }
    else
    {
        const Camera& cam = getCamera();        

        mFbo.begin();
        {
            //-- drawScene offscreen
            mFbo.drawTo(0);
            {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                drawScene();
            }
            
            glDisable(GL_LIGHTING);
            //disable all depth test for the post processing stages...
            glDisable(GL_DEPTH_TEST);
            //--- post processing
            //1- gamma correction
            mFbo.drawTo(1);
            {
                glClear(GL_COLOR_BUFFER_BIT);
                mGammaCorrection.begin();
                mGammaCorrection.setUniform("uOffScreen", 0);

                Texture t = mFbo.getColorAttachment(0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, t.getId());
                blitToScreen(cam.getViewport().getSize());       
                glBindTexture(GL_TEXTURE_2D, 0);

                mGammaCorrection.end();
            }

            //post processing
            //SMAA - Luma edge detection edgeTex
            mFbo.drawTo(2);
            {
                glClear(GL_COLOR_BUFFER_BIT);
                mMLAAShader.begin();
                mMLAAShader.setUniform("uColorTex", 0);
                mMLAAShader.setUniform("uAreaTex", 1);
                mMLAAShader.setUniform("uSearchTex", 2);
                mMLAAShader.setUniform("uFirstPass", 1);
                mMLAAShader.setUniform("uSecondPass", 0);
                mMLAAShader.setUniform("uThirdPass", 0);

                Texture t = mFbo.getColorAttachment(1);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, t.getId());
                blitToScreen(cam.getViewport().getSize());
                glBindTexture(GL_TEXTURE_2D, 0);

                mMLAAShader.end();
            }

            //SMAA - Luma edge detection blentex
            //mFbo.drawTo(3);
            //{
            //    glClear(GL_COLOR_BUFFER_BIT);
            //    mMLAAShader.begin();
            //    mMLAAShader.setUniform("uColorTex", 0);
            //    mMLAAShader.setUniform("uAreaTex", 1);
            //    mMLAAShader.setUniform("uSearchTex", 2);
            //    mMLAAShader.setUniform("uFirstPass", 0);
            //    mMLAAShader.setUniform("uSecondPass", 1);
            //    mMLAAShader.setUniform("uThirdPass", 0);

            //    glActiveTexture(GL_TEXTURE1);
            //    glBindTexture(GL_TEXTURE_2D, mSmaaAreaTexture.getId());

            //    glActiveTexture(GL_TEXTURE2);
            //    glBindTexture(GL_TEXTURE_2D, mSmaaSearchTexture.getId());

            //    Texture t = mFbo.getColorAttachment(2);
            //    glActiveTexture(GL_TEXTURE0);
            //    glBindTexture(GL_TEXTURE_2D, t.getId());
            //    blitToScreen(cam.getViewport().getSize());
            //    glBindTexture(GL_TEXTURE_2D, 0);

            //    mMLAAShader.end();
            //}
            
        }
        mFbo.end();

        //Draw final result has full screen quad
        Texture finalT = mFbo.getColorAttachment(gFrameBufferToDisplay);        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, finalT.getId());
        blitToScreen(cam.getViewport().getSize());
        glBindTexture(GL_TEXTURE_2D, 0);

        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);
    }
}

//-----------------------------------------------------------------------------
void Viewer::drawScene()
{
    switch(gSceneContent)
    {
    case sc3d: //cube
    {
        glEnable(GL_LIGHTING);
        glPushMatrix();
        glRotated(gRotx, 1, 0, 0);
        glRotated(gRoty, 0, 1, 0);
        glRotated(gRotz, 0, 0, 1);
        treeD::drawRectangularPrism(math::Point3d(-5), math::Point3d(5));
        glPopMatrix();
    }break;
    case scUnigine01: //uEngine test image 1
    {
        Vector2d size(mUnigine01.width(), mUnigine01.height());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mUnigine01.getId());
        blitToScreen(size);
        glBindTexture(GL_TEXTURE_2D, 0);
    }break;
    case scUnigine02: //uEngine test image 2
    {
        Vector2d size(mUnigine02.width(), mUnigine02.height());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mUnigine02.getId());
        blitToScreen(size);
        glBindTexture(GL_TEXTURE_2D, 0);
    }break;
    }
    
}

//-----------------------------------------------------------------------------
void Viewer::initializeGL()
{
	Widget3d::initializeGL();
	glClearColor(0.0, 0.0, 0.0, 0.0);
    //glClearColor(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

	//init mFbo
	mFbo.addColorAttachment();
    mFbo.addColorAttachment();
    mFbo.addColorAttachment();
    mFbo.addColorAttachment();
    mFbo.getColorAttachment(0).setFilter(GL_LINEAR);
    mFbo.getColorAttachment(0).setWrapMode(GL_CLAMP);
    mFbo.getColorAttachment(1).setFilter(GL_LINEAR);
    mFbo.getColorAttachment(1).setWrapMode(GL_CLAMP);
    mFbo.getColorAttachment(2).setFilter(GL_LINEAR);
    mFbo.getColorAttachment(2).setWrapMode(GL_CLAMP);
    mFbo.getColorAttachment(3).setFilter(GL_LINEAR);
    mFbo.getColorAttachment(3).setWrapMode(GL_CLAMP);

    mFbo.addDepthAttachment();

    //init shaders
	loadShaders();

    //init textures
    loadTextures();

    gFrameBufferToDisplay = mFbo.getNumColorAttachment() - 1;

    mpMainDialog->updateUi();
}

//-----------------------------------------------------------------------------
void Viewer::keyPressEvent(QKeyEvent* ipE)
{
	switch (ipE->key())
	{
    case Qt::Key_C: gSceneContent = (sceneContent)((gSceneContent + 1) % scContentCount); break;
    case Qt::Key_F: gFrameBufferToDisplay = (gFrameBufferToDisplay + 1) % mFbo.getNumColorAttachment(); break;
    case Qt::Key_P: gMlaaActivated = !gMlaaActivated; break;
    case Qt::Key_R: gRotateCube = !gRotateCube; break;
	case Qt::Key_F5: loadShaders(); break;
    default: Widget3d::keyPressEvent(ipE); break;
	}
	update();
    mpMainDialog->updateUi();
}

//-----------------------------------------------------------------------------
void Viewer::loadShaders()
{
    const QString cwd = QCoreApplication::applicationDirPath();
    printf("cwd: %s\n", cwd.toStdString().c_str());
    //init shaders
    {
        mMLAAShader.clear();

        QFile f2(cwd + "/../assets/SMAA.frag");
        f2.open(QIODevice::ReadOnly);
        QString content2 = f2.readAll();
        mMLAAShader.addFragmentSource(content2);

        QFile f(cwd + "/../assets/SMAA.hlsl");
        f.open(QIODevice::ReadOnly);
        QString content = f.readAll();

        mMLAAShader.addFragmentSource(content);
        mMLAAShader.link();
    }

    {
        mGammaCorrection.clear();

        QFile f(cwd + "/../assets/gammaCorrection.frag");
        f.open(QIODevice::ReadOnly);
        QString content = f.readAll();

        mGammaCorrection.addFragmentSource(content);
        mGammaCorrection.link();
    }

    {
        mTestShader.clear();

        QFile f(cwd + "/../assets/test.frag");
        f.open(QIODevice::ReadOnly);
        QString content = f.readAll();

        mTestShader.addFragmentSource(content);
        mTestShader.link();
    }
}

//-----------------------------------------------------------------------------
void Viewer::loadTextures()
{
    const QString cwd = QCoreApplication::applicationDirPath();
    
    //Area texture
    math::Vector2i areaTexSize(AREATEX_WIDTH, AREATEX_HEIGHT);
    mSmaaAreaTexture.set((void*)areaTexBytes, areaTexSize, GL_RG8, GL_RG, GL_UNSIGNED_BYTE);
    mSmaaAreaTexture.setFilter(GL_LINEAR);
    mSmaaAreaTexture.setWrapMode(GL_CLAMP);

    //SearcTexture
    //Area texture
    math::Vector2i searchTexSize(SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT);
    mSmaaSearchTexture.set((void*)searchTexBytes, searchTexSize, GL_R8, GL_R, GL_UNSIGNED_BYTE);
    mSmaaSearchTexture.setFilter(GL_LINEAR);
    mSmaaSearchTexture.setWrapMode(GL_CLAMP);

    QImage im;
    //demo Unigine01    
    im = QImage(cwd + "/../assets/Unigine01.png", "PNG");
    if (!im.isNull())
    {
        mUnigine01.set(im, GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE);
        mUnigine01.setFilter(GL_LINEAR);
        mUnigine01.setWrapMode(GL_CLAMP);
    }

    //demo Unigine01
    im = QImage(cwd + "/../assets/Unigine02.png", "PNG");
    if (!im.isNull())
    {
        mUnigine02.set(im, GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE);
        mUnigine02.setFilter(GL_LINEAR);
        mUnigine02.setWrapMode(GL_CLAMP);
    }
} 
//-----------------------------------------------------------------------------
void Viewer::resizeGL(int iW, int iH)
{
	Widget3d::resizeGL(iW, iH);

	if (!(mFbo.getWidth() == iW && mFbo.getHeight() == iH))
	{
		mFbo.resize(iW, iH);
	}
}

//-----------------------------------------------------------------------------
//--- MainDialog
//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QMainWindow(),
mpViewer(0),
mTimerEventId(0)
{
	resize(1280, 720);

	QWidget *pCentralWidget = new QWidget(this);
	setCentralWidget(pCentralWidget);

	QHBoxLayout* pLyt = new QHBoxLayout(pCentralWidget);
	pLyt->setMargin(2);
    pLyt->setSpacing(5);
	{
		QVBoxLayout *pControlLyt = new QVBoxLayout();
		{
            QHBoxLayout *pL0 = new QHBoxLayout();
            {
                QLabel* l = new QLabel("Rotate (R):", pCentralWidget);
                mpRotationState = new QLabel("Off", pCentralWidget);

                pL0->addWidget(l);
                pL0->addWidget(mpRotationState);
                pL0->addStretch(1);
            }

            QHBoxLayout *pL1 = new QHBoxLayout();
            {
                QLabel* l = new QLabel("Post processing (P):", pCentralWidget);
                mpPostProcessingState = new QLabel("Deactivated", pCentralWidget);

                pL1->addWidget(l);
                pL1->addWidget(mpPostProcessingState);
                pL1->addStretch(1);
            }

            QHBoxLayout *pL2 = new QHBoxLayout();
            {
                QLabel* l = new QLabel("Pass displayed (F):", pCentralWidget);
                mpPassDisplayed = new QLabel("0", pCentralWidget);

                pL2->addWidget(l);
                pL2->addWidget(mpPassDisplayed);
                pL2->addStretch(1);
            }

            QHBoxLayout *pL3 = new QHBoxLayout();
            {
                QLabel* l = new QLabel("Scene content (C):", pCentralWidget);
                mpSceneContent = new QLabel("3d", pCentralWidget);

                pL3->addWidget(l);
                pL3->addWidget(mpSceneContent);
                pL3->addStretch(1);
            }

            pControlLyt->addLayout(pL0);
            pControlLyt->addLayout(pL1);
            pControlLyt->addLayout(pL2);
            pControlLyt->addLayout(pL3);
            pControlLyt->addStretch(1);
        }

		mpViewer = new Viewer(pCentralWidget, this);

		pLyt->addLayout(pControlLyt, 1);
		pLyt->addWidget(mpViewer, 4);
	}

	treeD::Camera c = mpViewer->getCamera();
	c.set(Point3d(0.0, 0.0, 50),
		Point3d(), Vector3d(0, 1, 0));
	mpViewer->setCamera(c);
	mpViewer->setControlType(treeD::Widget3d::ctFree);

    mTimerEventId = startTimer(16);
	updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::timerEvent(QTimerEvent *ipE)
{
    if(ipE->timerId() == mTimerEventId)
    {
        if(gRotateCube)
        {
            gRotx += 0.5;
            gRoty += 0.5;
            gRotz += 0.5;
        }
    }

    mpViewer->update();
}

//-----------------------------------------------------------------------------
void MainDialog::updateUi()
{
    mpRotationState->setText( gRotateCube ? "On" : "Off" );

    mpPostProcessingState->setText( gMlaaActivated ? "Activated" : "Deactivated" );

    QString passName = "raw scene";
    switch(gFrameBufferToDisplay)
    {
        case 0: break;
        case 1: passName = "Gamma correction"; break;
        case 2: passName = "Edge detection"; break;
        case 3: passName = "Blend weight"; break;
        default: passName = "Unknown"; break;
    }
    mpPassDisplayed->setText(passName);

    QString sceneContent = "3d";
    switch (gSceneContent)
    {
    case sc3d: sceneContent = "3d"; break;
    case scUnigine01: sceneContent = "Unigine01"; break;
    case scUnigine02: sceneContent = "Unigine02"; break;
    default: sceneContent = "unknown"; break;
    }
    mpSceneContent->setText(sceneContent);

	update();
	mpViewer->update();
}