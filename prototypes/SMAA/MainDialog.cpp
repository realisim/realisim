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

    enum sceneContent{ sc3d = 0, scUnigine01, scUnigine02, scMandelbrot, scContentCount };
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
void Viewer::addFragmentSource(Shader* ipShader, QString iFileName)
{
    QFile f(iFileName);
    f.open(QIODevice::ReadOnly);
    QString content = f.readAll();
    ipShader->addFragmentSource(content);
    f.close();
}

//-----------------------------------------------------------------------------
void Viewer::addVertexSource(Shader* ipShader, QString iFileName)
{
    QFile f(iFileName);
    f.open(QIODevice::ReadOnly);
    QString content = f.readAll();
    ipShader->addVertexSource(content);
    f.close();
}

//-----------------------------------------------------------------------------
//draws a rectancle in screenspace starting at 0.0 to size. If a texure
//isbound to GL_TEXTURE0, the rectangle will be textured
void Viewer::drawRectangle(int iTextureId, Vector2d size)
{    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, iTextureId);
    treeD::drawRectangle(Point2d(0.0), size);
    glBindTexture(GL_TEXTURE_2D, 0);
}

//-----------------------------------------------------------------------------
void Viewer::drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    switch(gSceneContent)
    {
    case sc3d: //cube
    {
        mSceneShader.begin();
        mSceneShader.setUniform("modelViewMatrix", getCamera().getViewMatrix());
        mSceneShader.setUniform("projectionMatrix", getCamera().getProjectionMatrix());
        //glDisable(GL_LIGHTING);
        //glPushMatrix();
        //glRotated(gRotx, 1, 0, 0);
        //glRotated(gRoty, 0, 1, 0);
        //glRotated(gRotz, 0, 0, 1);
        treeD::drawRectangularPrism(math::Point3d(-5), math::Point3d(5));

        //treeD::drawRectangle(math::Point2d(-5), math::Vector2d(10));

        //glPopMatrix();

        mSceneShader.end();
    }break;
    case scUnigine01: //uEngine test image 1
    {
		//apply gamma 
		/*ScreenSpaceProjection sp(getCamera().getViewport().getSize());
		mGammaCorrection.begin();
		mGammaCorrection.setUniform("modelViewMatrix", sp.mViewMatrix);
		mGammaCorrection.setUniform("projectionMatrix", sp.mProjectionMatrix);
		mGammaCorrection.setUniform("uOffScreen", 0);
		drawRectangle(mUnigine01.getId(), Vector2d(mUnigine01.width(), mUnigine01.height()));
		mGammaCorrection.end();*/

        Vector2d size(mUnigine01.width(), mUnigine01.height());
        drawStillImage(mUnigine01.getId(), size);
    }break;
    case scUnigine02: //uEngine test image 2
    {
        Vector2d size(mUnigine02.width(), mUnigine02.height());
        drawStillImage(mUnigine02.getId(), size);
    }break;
    case scMandelbrot: //uEngine test image 2
    {
        Vector2d size(mMandelbrot.width(), mMandelbrot.height());
        drawStillImage(mMandelbrot.getId(), size);
    }break;
    
    default: break;
    }

    
}

//-----------------------------------------------------------------------------
void Viewer::drawStillImage(int iTextureId, realisim::math::Vector2d iSize)
{
    ScreenSpaceProjection sp(getCamera().getViewport().getSize());

    pushShader(mStillImageShader);
    mStillImageShader.setUniform("modelViewMatrix", sp.mViewMatrix);
    mStillImageShader.setUniform("projectionMatrix", sp.mProjectionMatrix);
    mStillImageShader.setUniform("uTexture0", 0);
    drawRectangle(iTextureId, iSize);
    popShader();
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
    mFbo.getColorAttachment(0).setWrapMode(GL_CLAMP_TO_EDGE);
    mFbo.getColorAttachment(1).setFilter(GL_LINEAR);
    mFbo.getColorAttachment(1).setWrapMode(GL_CLAMP_TO_EDGE);
    mFbo.getColorAttachment(2).setFilter(GL_LINEAR);
    mFbo.getColorAttachment(2).setWrapMode(GL_CLAMP_TO_EDGE);
	mFbo.getColorAttachment(3).setFilter(GL_LINEAR);
	mFbo.getColorAttachment(3).setWrapMode(GL_CLAMP_TO_EDGE);

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
	case Qt::Key_D: --gFrameBufferToDisplay; if(gFrameBufferToDisplay < 0) gFrameBufferToDisplay = mFbo.getNumColorAttachment() - 1; break;
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
        mSmaaShader.clear();
        addVertexSource(&mSmaaShader, cwd + "/../assets/utilities.vert");
        addVertexSource(&mSmaaShader, cwd + "/../assets/SMAA.vert");
        addFragmentSource(&mSmaaShader, cwd + "/../assets/SMAA.frag");
        addFragmentSource(&mSmaaShader, cwd + "/../assets/SMAA.hlsl");
        mSmaaShader.link();
    }

    {
        mSmaa2ndPassShader.clear();
        addVertexSource(&mSmaa2ndPassShader, cwd + "/../assets/utilities.vert");
        //mSmaa2ndPassShader.addVertexSource("#define SMAA_INCLUDE_PS 0\n");
        addVertexSource(&mSmaa2ndPassShader, cwd + "/../assets/SMAA_vert.hlsl");
        addVertexSource(&mSmaa2ndPassShader, cwd + "/../assets/SMAA_secondPass.vert");

        addFragmentSource(&mSmaa2ndPassShader, cwd + "/../assets/SMAA_secondPass.frag");
        //mSmaa2ndPassShader.addFragmentSource("#define SMAA_INCLUDE_VS 0\n#define SMAA_INCLUDE_PS 1\n");
        addFragmentSource(&mSmaa2ndPassShader, cwd + "/../assets/SMAA.hlsl");
        mSmaa2ndPassShader.link();
    }

	{
		mSmaa3rdPassShader.clear();
		addVertexSource(&mSmaa3rdPassShader, cwd + "/../assets/utilities.vert");
		//mSmaa2ndPassShader.addVertexSource("#define SMAA_INCLUDE_PS 0\n");
		addVertexSource(&mSmaa3rdPassShader, cwd + "/../assets/SMAA_vert.hlsl");
		addVertexSource(&mSmaa3rdPassShader, cwd + "/../assets/SMAA_thirdPass.vert");

		addFragmentSource(&mSmaa3rdPassShader, cwd + "/../assets/SMAA_thirdPass.frag");
		//mSmaa2ndPassShader.addFragmentSource("#define SMAA_INCLUDE_VS 0\n#define SMAA_INCLUDE_PS 1\n");
		addFragmentSource(&mSmaa3rdPassShader, cwd + "/../assets/SMAA.hlsl");
		mSmaa3rdPassShader.link();
	}
	

    {
        mSceneShader.clear();
        addVertexSource(&mSceneShader, cwd + "/../assets/utilities.vert");
        addVertexSource(&mSceneShader, cwd + "/../assets/main.vert");        
        addFragmentSource(&mSceneShader, cwd + "/../assets/untexturedMaterial.frag");                
        mSceneShader.link();
    }

    {
        mStillImageShader.clear();
        addVertexSource(&mStillImageShader, cwd + "/../assets/utilities.vert");
        addVertexSource(&mStillImageShader, cwd + "/../assets/main.vert");
        addFragmentSource(&mStillImageShader, cwd + "/../assets/texturedMaterial.frag");
        mStillImageShader.link();
    }

    {
        mGammaCorrection.clear();
        addVertexSource(&mGammaCorrection, cwd + "/../assets/utilities.vert");
        addVertexSource(&mGammaCorrection, cwd + "/../assets/main.vert");
        addFragmentSource(&mGammaCorrection, cwd + "/../assets/gammaCorrection.frag");
        mGammaCorrection.link();
    }
}

//-----------------------------------------------------------------------------
void Viewer::loadTextures()
{
    const QString cwd = QCoreApplication::applicationDirPath();
    
    //Area texture
	{
		std::vector<unsigned char> tempBuffer(AREATEX_SIZE);
		for (unsigned int y = 0; y < AREATEX_HEIGHT; y++) {
			unsigned int srcY = AREATEX_HEIGHT - 1 - y;
			//unsigned int srcY = y;
			memcpy(&tempBuffer[y * AREATEX_PITCH], areaTexBytes + srcY * AREATEX_PITCH, AREATEX_PITCH);
		}
		math::Vector2i areaTexSize(AREATEX_WIDTH, AREATEX_HEIGHT);
		mSmaaAreaTexture.set((void*)&tempBuffer[0], areaTexSize, GL_RG8, GL_RG, GL_UNSIGNED_BYTE);
		mSmaaAreaTexture.setFilter(GL_LINEAR);
		mSmaaAreaTexture.setWrapMode(GL_CLAMP_TO_EDGE);
	}
	
    //SearcTexture
    //Area texture
	{
		std::vector<unsigned char> tempBuffer(SEARCHTEX_SIZE);
		for (unsigned int y = 0; y < SEARCHTEX_HEIGHT; y++) {
			unsigned int srcY = SEARCHTEX_HEIGHT - 1 - y;
			//unsigned int srcY = y;
			memcpy(&tempBuffer[y * SEARCHTEX_PITCH], searchTexBytes + srcY * SEARCHTEX_PITCH, SEARCHTEX_PITCH);
		}
		math::Vector2i searchTexSize(SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT);
		mSmaaSearchTexture.set((void*)&tempBuffer[0], searchTexSize, GL_R8, GL_RED, GL_UNSIGNED_BYTE);
		mSmaaSearchTexture.setFilter(GL_LINEAR);
		mSmaaSearchTexture.setWrapMode(GL_CLAMP_TO_EDGE);
	}
    

    QImage im;
    //demo Unigine01    
    im = QImage(cwd + "/../assets/Unigine01.png", "PNG");
    if (!im.isNull())
    {
        //mUnigine01.set(im, GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE);
        mUnigine01.set(im, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
        mUnigine01.setFilter(GL_NEAREST);
        mUnigine01.setWrapMode(GL_CLAMP);
    }

    //demo Unigine01
    im = QImage(cwd + "/../assets/Unigine02.png", "PNG");
    if (!im.isNull())
    {
        //mUnigine02.set(im, GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE);
        mUnigine02.set(im, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
        mUnigine02.setFilter(GL_NEAREST);
        mUnigine02.setWrapMode(GL_CLAMP);
    }

    //demo mandelbrot
    im = QImage(cwd + "/../assets/mandelbrot.jpg", "JPG");
    if (!im.isNull())
    {
        mMandelbrot.set(im, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
        mMandelbrot.setFilter(GL_NEAREST);
        mMandelbrot.setWrapMode(GL_CLAMP);
    }
} 

//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
    if (!gMlaaActivated)
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
                drawScene();
            }
			
            glDisable(GL_LIGHTING);
            //disable all depth test for the post processing stages...
            glDisable(GL_DEPTH_TEST);
            //--- post processing

            //post processing
            //SMAA - Luma edge detection edgeTex
            mFbo.drawTo(1);
            {
                glClear(GL_COLOR_BUFFER_BIT);
                pushShader(mSmaaShader);
                ScreenSpaceProjection sp(cam.getViewport().getSize());
                mSmaaShader.setUniform("modelViewMatrix", sp.mViewMatrix);
                mSmaaShader.setUniform("projectionMatrix", sp.mProjectionMatrix);

                mSmaaShader.setUniform("uColorTex", 0);

                Texture t = mFbo.getColorAttachment(0);
                drawRectangle(t.getId(), Vector2d(t.width(), t.height()));

                popShader();

            }

            //SMAA - 2nd pass - blend weight
            mFbo.drawTo(2);
            {
                glClear(GL_COLOR_BUFFER_BIT);
                mSmaa2ndPassShader.begin();

                ScreenSpaceProjection sp(cam.getViewport().getSize());
				mSmaa2ndPassShader.setUniform("modelViewMatrix", sp.mViewMatrix);
				mSmaa2ndPassShader.setUniform("projectionMatrix", sp.mProjectionMatrix);
                mSmaa2ndPassShader.setUniform("uEdgeTex", 0);
                mSmaa2ndPassShader.setUniform("uAreaTex", 1);
                mSmaa2ndPassShader.setUniform("uSearchTex", 2);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, mSmaaAreaTexture.getId());

                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, mSmaaSearchTexture.getId());

                Texture t = mFbo.getColorAttachment(1);
                drawRectangle(t.getId(), Vector2d(t.width(), t.height()));

                mSmaa2ndPassShader.end();
            }

			//SMAA - 3rd pass perform the neighbour blending
			mFbo.drawTo(3);
			{
				glClear(GL_COLOR_BUFFER_BIT);
				mSmaa3rdPassShader.begin();

				ScreenSpaceProjection sp(cam.getViewport().getSize());
				mSmaa3rdPassShader.setUniform("modelViewMatrix", sp.mViewMatrix);
				mSmaa3rdPassShader.setUniform("projectionMatrix", sp.mProjectionMatrix);
				mSmaa3rdPassShader.setUniform("uColorTex", 0);
				mSmaa3rdPassShader.setUniform("uBlendTex", 1);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, mFbo.getColorAttachment(2).getId());

				Texture t = mFbo.getColorAttachment(0);
				drawRectangle(t.getId(), Vector2d(t.width(), t.height()));

				mSmaa3rdPassShader.end();
			}
        }
        mFbo.end();

        //Draw final result has full screen quad
        glColor3ub(255, 255, 255);
        Texture finalT = mFbo.getColorAttachment(gFrameBufferToDisplay);
        GLenum minFilter = finalT.getMinificationFilter();
        GLenum maxFilter = finalT.getMagnificationFilter();
        finalT.setFilter(GL_NEAREST);

        //do not apply gamma to final pass
        drawStillImage(finalT.getId(), Vector2d(finalT.width(), finalT.height()));

        //apply gamma to final pass
        //ScreenSpaceProjection sp(cam.getViewport().getSize());
        //mGammaCorrection.begin();
        //mGammaCorrection.setUniform("modelViewMatrix", sp.mViewMatrix);
        //mGammaCorrection.setUniform("projectionMatrix", sp.mProjectionMatrix);
        //mGammaCorrection.setUniform("uOffScreen", 0);
        //drawRectangle(finalT.getId(), Vector2d(finalT.width(), finalT.height()));
        //mGammaCorrection.end();

        finalT.setMinificationFilter(minFilter);
        finalT.setMagnificationFilter(maxFilter);

        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);
    }
}

//-----------------------------------------------------------------------------
void Viewer::resizeGL(int iW, int iH)
{
    Widget3d::resizeGL(iW, iH);

    if (!(mFbo.getWidth() == iW && mFbo.getHeight() == iH))
    {
        mFbo.resize(iW, iH);
        printf("FBO size: %d, %d\n", iW, iH);
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
        case 1: passName = "Edge detection"; break;
        case 2: passName = "Blend weight"; break;
		case 3: passName = "Final result"; break;
        default: passName = "Unknown"; break;
    }
    mpPassDisplayed->setText(passName);

    QString sceneContent = "3d";
    switch (gSceneContent)
    {
    case sc3d: sceneContent = "3d"; break;
    case scUnigine01: sceneContent = "Unigine01"; break;
    case scUnigine02: sceneContent = "Unigine02"; break;
    case scMandelbrot: sceneContent = "Mandelbrot"; break;
        
    default: sceneContent = "unknown"; break;
    }
    mpSceneContent->setText(sceneContent);

    update();
    mpViewer->update();
}