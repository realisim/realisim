/**/

#include <3d/Camera.h>
#include <3d/Utilities.h>
#include <math/Point.h>
#include <math/Vect.h>
#include "MainDialog.h"
#include <QCoreApplication>
#include <QHBoxLayout>


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
void Viewer::blitToScreen(Texture t, Vector2d size)
{
    glDisable(GL_LIGHTING);
    ScreenSpaceProjection sp(size);
    {
        //glColor3ub(255, 255, 255);
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, t.getId());
        drawRectangle(Point2d(0.0), size);
        glBindTexture(GL_TEXTURE_2D, 0);
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
            
            //disable all depth test for the post processing stages...
            glDisable(GL_DEPTH_TEST);
            //--- post processing
            //1- gamma correction
            mFbo.drawTo(1);
            {
                glClear(GL_COLOR_BUFFER_BIT);
                mGammaCorrection.begin();
                mGammaCorrection.setUniform("uOffScreen", 0);

                Texture t0 = mFbo.getColorAttachment(0);
                blitToScreen(t0, cam.getViewport().getSize());       

                mGammaCorrection.end();
            }

            //post processing
            //SMAA - Luma edge detection edgeTex
            mFbo.drawTo(2);
            {
                glClear(GL_COLOR_BUFFER_BIT);
                mMLAAShader.begin();
                mMLAAShader.setUniform("uOffScreen", 0);

                Texture t = mFbo.getColorAttachment(1);
                blitToScreen(t, cam.getViewport().getSize());
                mMLAAShader.end();
            }
            
        }
        mFbo.end();

        //Draw final result has full screen quad
        Texture finalT = mFbo.getColorAttachment(gFrameBufferToDisplay);
        blitToScreen(finalT, cam.getViewport().getSize());
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
        blitToScreen(mUnigine01, size);
    }break;
    case scUnigine02: //uEngine test image 2
    {
        Vector2d size(mUnigine02.width(), mUnigine02.height());
        blitToScreen(mUnigine02, size);
    }break;
    }
    
}

//-----------------------------------------------------------------------------
void Viewer::initializeGL()
{
	Widget3d::initializeGL();
	glClearColor(0.0, 0.0, 0.0, 0.0);
    //glClearColor(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

	//init mFbo
	mFbo.addColorAttachment();
    mFbo.addColorAttachment();
    mFbo.addColorAttachment();
    mFbo.getColorAttachment(0).setFilter(GL_LINEAR);
    mFbo.getColorAttachment(0).setWrapMode(GL_CLAMP);
    mFbo.getColorAttachment(1).setFilter(GL_LINEAR);
    mFbo.getColorAttachment(1).setWrapMode(GL_CLAMP);
    mFbo.getColorAttachment(2).setFilter(GL_LINEAR);
    mFbo.getColorAttachment(2).setWrapMode(GL_CLAMP);

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
}

//-----------------------------------------------------------------------------
void Viewer::loadTextures()
{
    const QString cwd = QCoreApplication::applicationDirPath();
    QImage im(cwd + "/../assets/Unigine01.png", "PNG");
    if (!im.isNull())
    {
        mUnigine01.set(im, GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE);
        mUnigine01.setFilter(GL_LINEAR);
        mUnigine01.setWrapMode(GL_CLAMP);
    }

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