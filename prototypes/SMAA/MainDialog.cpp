/**/

#include <3d/Camera.h>
#include <3d/Utilities.h>
#include <iomanip>
#include <iostream>
//include jiminez smaa lookup textures
#include <JimenezSmaa/71c806a/include/AreaTex.h>
#include <JimenezSmaa/71c806a/include/SearchTex.h>
#include <math/Point.h>
#include <math/Vect.h>
#include "MainDialog.h"
#include <QCoreApplication>
#include <QGroupBox>
#include <QHBoxLayout>
#include <sstream>

using namespace realisim;
using namespace math;
using namespace treeD;

namespace
{
    enum sceneContent { scGeometricGrid = 0, scTexturedGrid, scMillionsOfPolygons, scUnigine01, scUnigine02, scContentCount };
    sceneContent gSceneContent = scGeometricGrid;

    //-----------------------------------------------------------------------------
    QString toQString(smaaPresetQuality iQ)
    {
        QString r="N/A";
        switch (iQ)
        {
        case spqLow: r = "SMAA_PRESET_LOW"; break;
        case spqMedium: r = "SMAA_PRESET_MEDIUM"; break;
        case spqHigh: r = "SMAA_PRESET_HIGH"; break;
        case spqUltra: r = "SMAA_PRESET_ULTRA"; break;
        default: break;
        }
        return r;
    }
}

Viewer::Viewer(QWidget* ipParent, MainDialog* ipM) : Widget3d(ipParent),
mpMainDialog(ipM),
mFrameIndex(0)
{
    setFocusPolicy(Qt::StrongFocus);
}

Viewer::~Viewer()
{}

//-----------------------------------------------------------------------------
int Viewer::addFragmentSource(Shader* ipShader, QString iFileName)
{
    int r = -1;
    QFile f(iFileName);
    f.open(QIODevice::ReadOnly);
    QString content = f.readAll();
    r = ipShader->addFragmentSource(content);
    f.close();
    return r;
}

//-----------------------------------------------------------------------------
int Viewer::addVertexSource(Shader* ipShader, QString iFileName)
{
    int r = -1;
    QFile f(iFileName);
    f.open(QIODevice::ReadOnly);
    QString content = f.readAll();
    r = ipShader->addVertexSource(content);
    f.close();
    return r;
}

//-----------------------------------------------------------------------------
void Viewer::displayPass(FrameBufferObject iFbo, int iPass)
{
    glDisable(GL_LIGHTING);
    //disable all depth test for the post processing stages...
    glDisable(GL_DEPTH_TEST);

    //Draw final result has full screen quad
    //since the fbo is the same size as the screen, we want it to be pixel perfect, so
    //we activate GL_NEAREST filtering on the texture.
    glColor3ub(255, 255, 255);
    Texture t = iFbo.getColorAttachment(iPass);    

    Vector2i textureSize(t.width(), t.height());
    ScreenSpaceProjection sp(textureSize);
    drawStillImage(t.getId(), textureSize, sp.mViewMatrix, sp.mProjectionMatrix);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

//-----------------------------------------------------------------------------
void Viewer::doMsaa(int iX)
{
    mMultisampleFbo.setNumberOfSamples(iX);
    mMultisampleFbo.begin();
    {
        //-- drawScene offscreen into the multisample buffer.
        //pass 0 - no gamma correction
        mMultisampleFbo.drawTo(msaaRtSRGB);
        {
            drawScene();
        }
    }
    mMultisampleFbo.end();
}

//-----------------------------------------------------------------------------
void Viewer::doReprojection(renderTarget iPreviousFinalRt, renderTarget iFinalRt)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    mColorFbo.begin();
    {
        mColorFbo.drawTo(iFinalRt);
        ScreenSpaceProjection sp(getCamera().getViewport().getSize());
        mSmaaReprojectionShader.begin();
        mSmaaReprojectionShader.setUniform("modelViewMatrix", sp.mViewMatrix);
        mSmaaReprojectionShader.setUniform("projectionMatrix", sp.mProjectionMatrix);
        mSmaaReprojectionShader.setUniform("uColorTex", 0);
        mSmaaReprojectionShader.setUniform("uPreviousColorTex", 1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mColorFbo.getColorAttachment(iPreviousFinalRt).getId());

        Texture t = mColorFbo.getColorAttachment(iFinalRt);
        drawRectangle(t.getId(), Vector2d(t.width(), t.height()));

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);

        mSmaaReprojectionShader.end();
    }
    mColorFbo.end();

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

//-----------------------------------------------------------------------------
void Viewer::doSmaa1x(renderTarget iInput, renderTarget iOutput, int pass)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    const Camera& cam = getCamera();
    mColorFbo.begin();
    {

        //--- post processing
        //pass 1
        //gamma corretion 
        mColorFbo.drawTo(rtRGBA);
        {
            glClear(GL_COLOR_BUFFER_BIT);
            ScreenSpaceProjection sp(cam.getViewport().getSize());
            mGammaCorrection.begin();
            mGammaCorrection.setUniform("modelViewMatrix", sp.mViewMatrix);
            mGammaCorrection.setUniform("projectionMatrix", sp.mProjectionMatrix);
            mGammaCorrection.setUniform("uColorTex", 0);

            Texture t = mColorFbo.getColorAttachment(iInput);
            drawRectangle(t.getId(), Vector2d(t.width(), t.height()));

            mGammaCorrection.end();
        }

        //pass 2
        //SMAA - Luma edge detection edgeTex
        //this requires a gamma corrected input texture
        mColorFbo.drawTo(rtEdge);
        {
            glClear(GL_COLOR_BUFFER_BIT);
            mSmaaShader.begin();
            ScreenSpaceProjection sp(cam.getViewport().getSize());
            mSmaaShader.setUniform("modelViewMatrix", sp.mViewMatrix);
            mSmaaShader.setUniform("projectionMatrix", sp.mProjectionMatrix);
            mSmaaShader.setUniform("uColorTex", 0);

            Texture t = mColorFbo.getColorAttachment(rtRGBA);
            drawRectangle(t.getId(), Vector2d(t.width(), t.height()));

            mSmaaShader.end();
        }

        //pass 3
        //SMAA - 2nd pass - blend weight
        mColorFbo.drawTo(rtBlendWeight);
        {
            glClear(GL_COLOR_BUFFER_BIT);
            mSmaa2ndPassShader.begin();

            ScreenSpaceProjection sp(cam.getViewport().getSize());
            mSmaa2ndPassShader.setUniform("modelViewMatrix", sp.mViewMatrix);
            mSmaa2ndPassShader.setUniform("projectionMatrix", sp.mProjectionMatrix);
            mSmaa2ndPassShader.setUniform("uSubsampleIndices", getSubsampleIndices(pass));
            mSmaa2ndPassShader.setUniform("uEdgeTex", 0);
            mSmaa2ndPassShader.setUniform("uAreaTex", 1);
            mSmaa2ndPassShader.setUniform("uSearchTex", 2);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, mSmaaAreaTexture.getId());

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, mSmaaSearchTexture.getId());

            Texture t = mColorFbo.getColorAttachment(rtEdge);
            drawRectangle(t.getId(), Vector2d(t.width(), t.height()));

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 0);

            glActiveTexture(GL_TEXTURE0);

            mSmaa2ndPassShader.end();
        }

        //pass 4
        //SMAA - 3rd pass perform the neighbour blending and gamma correction again.
        //		
        mColorFbo.drawTo(iOutput);
        {
            if (pass == 0)
            {
                glClear(GL_COLOR_BUFFER_BIT);
            }

            mSmaa3rdPassShader.begin();

            ScreenSpaceProjection sp(cam.getViewport().getSize());
            mSmaa3rdPassShader.setUniform("modelViewMatrix", sp.mViewMatrix);
            mSmaa3rdPassShader.setUniform("projectionMatrix", sp.mProjectionMatrix);
            mSmaa3rdPassShader.setUniform("uColorTex", 0);
            mSmaa3rdPassShader.setUniform("uBlendTex", 1);
            mSmaa3rdPassShader.setUniform("uBlendFactor", pass == 0 ? 1.0 : 0.5);
            if (pass == 1)
            {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, mColorFbo.getColorAttachment(rtBlendWeight).getId());

            //the texture required must be in linear space (no-gamma)
            Texture t = mColorFbo.getColorAttachment(iInput);
            drawRectangle(t.getId(), Vector2d(t.width(), t.height()));

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE0);

            mSmaa3rdPassShader.end();
        }
        glDisable(GL_BLEND);
    }
    mColorFbo.end();

    if (mpMainDialog->shouldSaveFboPass())
    {
        saveAllSmaa1xPassToPng(pass);
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

//-----------------------------------------------------------------------------
void Viewer::doSmaaSeparate()
{
    //mMultisampleFbo.resolveTo(msaaRtSRGB, mColorFbo, rtSRGB);
    //mColorFbo.getColorAttachment(rtSRGB).asQImage().save("f:/msaa2xResolved.png", "PNG");

        //output each sample to separate0 and separate0 colorAttachment
    const Vector2i size(getCamera().getViewport().getSize());

    ScreenSpaceProjection sp(size);
    mSmaaSeparateShader.begin();
    mSmaaSeparateShader.setUniform("modelViewMatrix", sp.mViewMatrix);
    mSmaaSeparateShader.setUniform("projectionMatrix", sp.mProjectionMatrix);
    mSmaaSeparateShader.setUniform("uColorTexMS", 0);

    //tell glsl where the ouput frag are going
    glBindFragDataLocation(mSmaaSeparateShader.getProgramId(), 0, "frag_color0");
    glBindFragDataLocation(mSmaaSeparateShader.getProgramId(), 1, "frag_color1");

    //bind input frame buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mMultisampleFbo.getFrameBufferId());

    //bind output frame buffer for separation
    GLenum drawBufs[2] = { GL_COLOR_ATTACHMENT0 + rtSeparate_0, GL_COLOR_ATTACHMENT0 + rtSeparate_1 };
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mColorFbo.getFrameBufferId());
    glDrawBuffers(2, &drawBufs[0]);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    //bind Multisample texture to draw
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mMultisampleFbo.getColorAttachmentId(msaaRtSRGB));
    treeD::drawRectangle(Point2d(0.0), size);

    mSmaaSeparateShader.end();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    //unbind all framebuffers.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
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

    if( mpMainDialog->isWireFrameShown())
    { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }    

    //--- get view and projection matrices for the scene
    // here a jitter is added depending on the antiAliasingMode
    ScreenSpaceProjection sp(getCamera().getViewport().getSize());
    Matrix4 view = sp.mViewMatrix;
    Matrix4 proj = sp.mProjectionMatrix;
    if (mpMainDialog->has3dControlEnabled())
    {
        view = getCamera().getViewMatrix();
        proj = getCamera().getProjectionMatrix();
    }
    view = view * getJitterMatrix();

    //adjust view when camera is nodding
    tiltMatrix(&view, mpMainDialog->isCameraYawNodding(), mpMainDialog->isCameraPitchNodding());

    switch (gSceneContent)
    {
    case scGeometricGrid: //3d grid
    {		
        mSceneShader.begin();
        mSceneShader.setUniform("uApplyShading", mpMainDialog->hasShading());
        mSceneShader.setUniform("projectionMatrix", proj);
        mSceneShader.setUniform("uUseTexture", false);

        /*mSceneShader.setUniform("modelViewMatrix", view);
        mSceneShader.setUniform("normalMatrix", (view.inverse()).transpose() );
        VertexBufferObject vbo = treeD::getRectangularPrism(
            Point3d(-20, -20, -20),
            Point3d(20, 20, 20) );
        vbo.draw();*/

        const int kNumInstancesPerAxis = 80;
        const double displacementPerIter = 1;
        const double thickness = 0.1;
        const double length = (kNumInstancesPerAxis*displacementPerIter);
        VertexBufferObject vbo = treeD::getRectangularPrism(
            Point3d(-length, -thickness / 2.0, -thickness / 2.0),
            Point3d(length, thickness / 2.0, thickness / 2.0));

        for (int i = -kNumInstancesPerAxis; i <= kNumInstancesPerAxis; ++i)
        {
            Matrix4 t(Vector3d(0, 0, i * displacementPerIter));
            Matrix4 view2 = view*t;
            mSceneShader.setUniform("modelViewMatrix", view2);
            mSceneShader.setUniform("normalMatrix", (view2.inverse()).transpose());
            vbo.draw();
        }

        Matrix4 r(3.14156 / 2.0, Vector3d(0.0, 1.0, 0.0));
        for (int i = -kNumInstancesPerAxis; i <= kNumInstancesPerAxis; ++i)
        {
            Matrix4 t(Vector3d(i * displacementPerIter, 0.0, 0.0));
            Matrix4 view2 = view*t*r;
            mSceneShader.setUniform("modelViewMatrix", view2);
            mSceneShader.setUniform("normalMatrix", (view2.inverse()).transpose());
            vbo.draw();
        }
        mSceneShader.end();
    }break;
    case scTexturedGrid:
    {
        mOneTextureShader.begin();
        mOneTextureShader.setUniform("projectionMatrix", proj);
        mOneTextureShader.setUniform("modelViewMatrix", view);
        mOneTextureShader.setUniform("uTexture0", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mTextureGrid.getId());
        mTexturedGridVbo.draw();
        glBindTexture(GL_TEXTURE_2D, 0);

        mOneTextureShader.end();
    }break;
    case scMillionsOfPolygons:
    {
        mSceneShader.begin();
        mSceneShader.setUniform("uApplyShading", mpMainDialog->hasShading());
        mSceneShader.setUniform("projectionMatrix", proj);
        mSceneShader.setUniform("modelViewMatrix", view);
        mSceneShader.setUniform("normalMatrix", (view.inverse()).transpose());
        mSceneShader.setUniform("uUseTexture", true);
        mSceneShader.setUniform("uAlbedo", 0);

        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mTextureGrid.getId());
        mMillionsOfPolygonVbo.draw();
        glBindTexture(GL_TEXTURE_2D, 0);
        mSceneShader.end();
    } break;
    case scUnigine01: //uEngine test image 1
    {
        Vector2d size(mUnigine01.width(), mUnigine01.height());
        drawStillImage(mUnigine01.getId(), size, view, proj);

    }break;
    case scUnigine02: //uEngine test image 2
    {
        Vector2d size(mUnigine02.width(), mUnigine02.height());
        drawStillImage(mUnigine02.getId(), size, view, proj);
    }break;

    default: break;
    }

    if( mpMainDialog->isWireFrameShown())
    { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

    mPreviousWorldView = view;
    mPreviousWorldProj = proj;
}

//-----------------------------------------------------------------------------
void Viewer::drawSceneToColorFbo(renderTarget iTarget)
{
    const Camera& cam = getCamera();
    mColorFbo.begin();
    {
        //-- drawScene offscreen
        mColorFbo.drawTo(iTarget);
        {
            drawScene();
        }
    }
    mColorFbo.end();
}

//-----------------------------------------------------------------------------
void Viewer::drawStillImage(int iTextureId, realisim::math::Vector2d iSize,
    const realisim::math::Matrix4& iView, const realisim::math::Matrix4& iProj)
{
    pushShader(mOneTextureShader);
    mOneTextureShader.setUniform("modelViewMatrix", iView);
    mOneTextureShader.setUniform("projectionMatrix", iProj);
    mOneTextureShader.setUniform("uTexture0", 0);
    drawRectangle(iTextureId, iSize);
    popShader();
}

//-----------------------------------------------------------------------------
realisim::math::Vector4d Viewer::getSubsampleIndices(int pass) const
{
    Vector4d r(0.0);

    const int frameIndex = mFrameIndex % 2;
    switch (mpMainDialog->getAntiAliasingMode())
    {
    case aamSmaa1x:
        r.set(0.0, 0.0, 0.0, 0.0);
        break;
    case aamSmaaT2x:
        if (frameIndex == 0)
            r.set(1.0, 1.0, 1.0, 0.0);
        else
            r.set(2.0, 2.0, 2.0, 0.0);
        break;
    case aamSmaaS2x:
        if (pass == 0)
            r.set(1.0, 1.0, 1.0, 0.0);
        else
            r.set(2.0, 2.0, 2.0, 0.0);
        break;
    case aamSmaa4x:
    {
        Vector4d v[4] = {
            Vector4d(5.0, 3.0, 1.0, 3.0),
            Vector4d(4.0, 6.0, 2.0, 3.0),
            Vector4d(3.0, 5.0, 1.0, 4.0),
            Vector4d(6.0, 4.0, 2.0, 4.0) };
        int i = 2 * frameIndex + pass;
        r = v[i];
    }break;
    default: break;
    }

    return r;
}

//-----------------------------------------------------------------------------
realisim::math::Matrix4 Viewer::getJitterMatrix() const
{
    const int passIndex = mFrameIndex % 2;
    Vector3d jitter;
    switch (mpMainDialog->getAntiAliasingMode())
    {
    case aamSmaa1x: jitter = Vector3d(0.0, 0.0, 0.0); break;
    case aamSmaaT2x:
        if (passIndex == 0)
            jitter = Vector3d(-0.25, 0.25, 0.0);
        else
            jitter = Vector3d(0.25, -0.25, 0.0);
        break;
    case aamSmaaS2x: jitter = Vector3d(0.0, 0.0, 0.0); break;
    case aamSmaa4x:
        if (passIndex == 0)
            jitter = Vector3d(-0.125, -0.125, 0.0);
        else
            jitter = Vector3d(0.125, 0.125, 0.0);
        break;
    default: break;
    }

    jitter = Vector3d(2 * jitter.x() / (double)width(), 2 * jitter.y() / (double)height(), 0.0);
    //printf("jitter: %s\n", jitter.toString(6).c_str() );
    return Matrix4(jitter);
}

//-----------------------------------------------------------------------------
void Viewer::initializeGL()
{
    Widget3d::initializeGL();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    //glClearColor(1.0, 1.0, 1.0, 1.0);

	glDepthRangedNV(-1,1);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    //--- init mFbo

    //--- color fbo
    mColorFbo.addColorAttachment(GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE);
    mColorFbo.getColorAttachment(rtSRGBA).setFilter(GL_LINEAR);
    mColorFbo.getColorAttachment(rtSRGBA).setWrapMode(GL_CLAMP_TO_EDGE);
    for (int i = rtRGBA; i < rtCount; ++i)
    {
        mColorFbo.addColorAttachment(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
        mColorFbo.getColorAttachment(i).setFilter(GL_LINEAR);
        mColorFbo.getColorAttachment(i).setWrapMode(GL_CLAMP_TO_EDGE);
    }
    mColorFbo.addDepthAttachment();

    //multiSample fbo
    for (int i = 0; i < msaaRtCount; ++i)
    {
        mMultisampleFbo.addColorAttachment(GL_SRGB8_ALPHA8);
        //mMsaaFbo.getColorAttachment(i).setFilter(GL_LINEAR);
        //mMsaaFbo.getColorAttachment(i).setWrapMode(GL_CLAMP_TO_EDGE);
    }
    mMultisampleFbo.addDepthAttachment();

    //init shaders
    loadShaders();

    //init textures
    loadTextures();

    //init vbo
    loadVbos();

    mpMainDialog->updateUi();
}

//-----------------------------------------------------------------------------
void Viewer::keyPressEvent(QKeyEvent* ipE)
{
    switch (ipE->key())
    {
    case Qt::Key_C: 
        gSceneContent = (sceneContent)((gSceneContent + 1) % scContentCount);
        resetCamera();
        break;
    case Qt::Key_F: mpMainDialog->displayPreviousDebugPass(); break;
    case Qt::Key_G: mpMainDialog->displayNextDebugPass(); break;
    case Qt::Key_F5: loadShaders(); loadTextures(); break;
    case Qt::Key_F11: toggleFullScreen(); break;
    default: Widget3d::keyPressEvent(ipE); break;
    }
    mpMainDialog->updateUi();
}

//-----------------------------------------------------------------------------
void Viewer::loadShaders()
{
    const QString cwd = QCoreApplication::applicationDirPath();
    printf("cwd: %s\n", cwd.toStdString().c_str());

    char smaaRtMetrics[512];
    const double w = width(), h = height();
    sprintf(&smaaRtMetrics[0], "float4(1.0 / %f, 1.0 / %f, %f, %f)", w, h, w, h);

    char smaaPresetQuality[100];
    sprintf(&smaaPresetQuality[0], "%s", toQString(mpMainDialog->getSmaaPresetQuality()).toStdString().c_str() );
    //init shaders
    {
        mSmaaShader.clear();
        mSmaaShader.setName("SMAA_firstPass");
        int smaaVertexSourceId = addVertexSource(&mSmaaShader, cwd + "/../assets/SMAA.hlsl");
        mSmaaShader.addDefineToVertexSource(smaaVertexSourceId, smaaPresetQuality, "");
        mSmaaShader.addDefineToVertexSource(smaaVertexSourceId, "SMAA_INCLUDE_VS", "1");
        mSmaaShader.addDefineToVertexSource(smaaVertexSourceId, "SMAA_INCLUDE_PS", "0");
        mSmaaShader.addDefineToVertexSource(smaaVertexSourceId, "SMAA_RT_METRICS", smaaRtMetrics);
        addVertexSource(&mSmaaShader, cwd + "/../assets/utilities.vert");
        addVertexSource(&mSmaaShader, cwd + "/../assets/SMAA_firstPass.vert");

        int smaaFragmentSourceId = addFragmentSource(&mSmaaShader, cwd + "/../assets/SMAA.hlsl");
        mSmaaShader.addDefineToFragmentSource(smaaFragmentSourceId, smaaPresetQuality, "");
        mSmaaShader.addDefineToFragmentSource(smaaFragmentSourceId, "SMAA_INCLUDE_VS", "0");
        mSmaaShader.addDefineToFragmentSource(smaaFragmentSourceId, "SMAA_INCLUDE_PS", "1");
        mSmaaShader.addDefineToFragmentSource(smaaFragmentSourceId, "SMAA_RT_METRICS", smaaRtMetrics);
        addFragmentSource(&mSmaaShader, cwd + "/../assets/SMAA_firstPass.frag");
        mSmaaShader.link();
    }

    {
        mSmaa2ndPassShader.clear();
        mSmaa2ndPassShader.setName("SMAA_secondPass");
        addVertexSource(&mSmaa2ndPassShader, cwd + "/../assets/utilities.vert");
        addVertexSource(&mSmaa2ndPassShader, cwd + "/../assets/SMAA_secondPass.vert");
        int smaaVertexSourceId = addVertexSource(&mSmaa2ndPassShader, cwd + "/../assets/SMAA.hlsl");
        mSmaa2ndPassShader.addDefineToVertexSource(smaaVertexSourceId, smaaPresetQuality, "");
        mSmaa2ndPassShader.addDefineToVertexSource(smaaVertexSourceId, "SMAA_INCLUDE_VS", "1");
        mSmaa2ndPassShader.addDefineToVertexSource(smaaVertexSourceId, "SMAA_INCLUDE_PS", "0");
        mSmaa2ndPassShader.addDefineToVertexSource(smaaVertexSourceId, "SMAA_RT_METRICS", smaaRtMetrics);

        addFragmentSource(&mSmaa2ndPassShader, cwd + "/../assets/SMAA_secondPass.frag");
        int smaaFragmentSourceId = addFragmentSource(&mSmaa2ndPassShader, cwd + "/../assets/SMAA.hlsl");
        mSmaa2ndPassShader.addDefineToFragmentSource(smaaFragmentSourceId, smaaPresetQuality, "");
        mSmaa2ndPassShader.addDefineToFragmentSource(smaaFragmentSourceId, "SMAA_INCLUDE_VS", "0");
        mSmaa2ndPassShader.addDefineToFragmentSource(smaaFragmentSourceId, "SMAA_INCLUDE_PS", "1");
        mSmaa2ndPassShader.addDefineToFragmentSource(smaaFragmentSourceId, "SMAA_RT_METRICS", smaaRtMetrics);
        mSmaa2ndPassShader.link();
    }

    {
        mSmaa3rdPassShader.clear();
        mSmaa3rdPassShader.setName("SMAA_thirdPass");
        addVertexSource(&mSmaa3rdPassShader, cwd + "/../assets/utilities.vert");
        addVertexSource(&mSmaa3rdPassShader, cwd + "/../assets/SMAA_thirdPass.vert");
        int smaaVertexSourceId = addVertexSource(&mSmaa3rdPassShader, cwd + "/../assets/SMAA.hlsl");
        mSmaa3rdPassShader.addDefineToVertexSource(smaaVertexSourceId, smaaPresetQuality, "");
        mSmaa3rdPassShader.addDefineToVertexSource(smaaVertexSourceId, "SMAA_INCLUDE_VS", "1");
        mSmaa3rdPassShader.addDefineToVertexSource(smaaVertexSourceId, "SMAA_INCLUDE_PS", "0");
        mSmaa3rdPassShader.addDefineToVertexSource(smaaVertexSourceId, "SMAA_RT_METRICS", smaaRtMetrics);

        addFragmentSource(&mSmaa3rdPassShader, cwd + "/../assets/utilities.frag");
        addFragmentSource(&mSmaa3rdPassShader, cwd + "/../assets/SMAA_thirdPass.frag");
        int smaaFragmentSourceId = addFragmentSource(&mSmaa3rdPassShader, cwd + "/../assets/SMAA.hlsl");
        mSmaa3rdPassShader.addDefineToFragmentSource(smaaFragmentSourceId, smaaPresetQuality, "");
        mSmaa3rdPassShader.addDefineToFragmentSource(smaaFragmentSourceId, "SMAA_INCLUDE_VS", "0");
        mSmaa3rdPassShader.addDefineToFragmentSource(smaaFragmentSourceId, "SMAA_INCLUDE_PS", "1");
        mSmaa3rdPassShader.addDefineToFragmentSource(smaaFragmentSourceId, "SMAA_RT_METRICS", smaaRtMetrics);
        mSmaa3rdPassShader.link();
    }

    {
        mSmaaReprojectionShader.clear();
        mSmaaReprojectionShader.setName("SMAA_reprojection");
        addVertexSource(&mSmaaReprojectionShader, cwd + "/../assets/utilities.vert");
        addVertexSource(&mSmaaReprojectionShader, cwd + "/../assets/SMAA_reprojection.vert");
        int smaaVertexSourceId = addVertexSource(&mSmaaReprojectionShader, cwd + "/../assets/SMAA.hlsl");
        mSmaaReprojectionShader.addDefineToVertexSource(smaaVertexSourceId, smaaPresetQuality, "");
        mSmaaReprojectionShader.addDefineToVertexSource(smaaVertexSourceId, "SMAA_INCLUDE_VS", "1");
        mSmaaReprojectionShader.addDefineToVertexSource(smaaVertexSourceId, "SMAA_INCLUDE_PS", "0");
        mSmaaReprojectionShader.addDefineToVertexSource(smaaVertexSourceId, "SMAA_RT_METRICS", smaaRtMetrics);

        addFragmentSource(&mSmaaReprojectionShader, cwd + "/../assets/SMAA_reprojection.frag");
        int smaaFragmentSourceId = addFragmentSource(&mSmaaReprojectionShader, cwd + "/../assets/SMAA.hlsl");
        mSmaaReprojectionShader.addDefineToFragmentSource(smaaFragmentSourceId, smaaPresetQuality, "");
        mSmaaReprojectionShader.addDefineToFragmentSource(smaaFragmentSourceId, "SMAA_INCLUDE_VS", "0");
        mSmaaReprojectionShader.addDefineToFragmentSource(smaaFragmentSourceId, "SMAA_INCLUDE_PS", "1");
        mSmaaReprojectionShader.addDefineToFragmentSource(smaaFragmentSourceId, "SMAA_RT_METRICS", smaaRtMetrics);
        mSmaaReprojectionShader.link();
    }

    {
        mSmaaSeparateShader.clear();
        mSmaaSeparateShader.setName("SMAA_separate");
        addVertexSource(&mSmaaSeparateShader, cwd + "/../assets/utilities.vert");
        addVertexSource(&mSmaaSeparateShader, cwd + "/../assets/SMAA_separate.vert");
        int smaaVertexSourceId = addVertexSource(&mSmaaSeparateShader, cwd + "/../assets/SMAA.hlsl");
        mSmaaSeparateShader.addDefineToVertexSource(smaaVertexSourceId, smaaPresetQuality, "");
        mSmaaSeparateShader.addDefineToVertexSource(smaaVertexSourceId, "SMAA_INCLUDE_VS", "1");
        mSmaaSeparateShader.addDefineToVertexSource(smaaVertexSourceId, "SMAA_INCLUDE_PS", "0");
        mSmaaSeparateShader.addDefineToVertexSource(smaaVertexSourceId, "SMAA_RT_METRICS", smaaRtMetrics);

        addFragmentSource(&mSmaaSeparateShader, cwd + "/../assets/SMAA_separate.frag");
        int smaaFragmentSourceId = addFragmentSource(&mSmaaSeparateShader, cwd + "/../assets/SMAA.hlsl");
        mSmaaSeparateShader.addDefineToFragmentSource(smaaFragmentSourceId, smaaPresetQuality, "");
        mSmaaSeparateShader.addDefineToFragmentSource(smaaFragmentSourceId, "SMAA_INCLUDE_VS", "0");
        mSmaaSeparateShader.addDefineToFragmentSource(smaaFragmentSourceId, "SMAA_INCLUDE_PS", "1");
        mSmaaSeparateShader.addDefineToFragmentSource(smaaFragmentSourceId, "SMAA_RT_METRICS", smaaRtMetrics);

        mSmaaSeparateShader.link();
    }

    {
        mSceneShader.clear();
        mSceneShader.setName("Scene shader");
        addVertexSource(&mSceneShader, cwd + "/../assets/utilities.vert");
        addVertexSource(&mSceneShader, cwd + "/../assets/main.vert");
        addFragmentSource(&mSceneShader, cwd + "/../assets/shadedMaterial.frag");
        mSceneShader.link();
    }

    {
        mOneTextureShader.clear();
        mOneTextureShader.setName("still image shader");
        addVertexSource(&mOneTextureShader, cwd + "/../assets/utilities.vert");
        addVertexSource(&mOneTextureShader, cwd + "/../assets/main.vert");
        addFragmentSource(&mOneTextureShader, cwd + "/../assets/texturedMaterial.frag");
        mOneTextureShader.link();
    }

    {
        mGammaCorrection.clear();
        mGammaCorrection.setName("gamma correction");
        addVertexSource(&mGammaCorrection, cwd + "/../assets/utilities.vert");
        addVertexSource(&mGammaCorrection, cwd + "/../assets/main.vert");

        addFragmentSource(&mGammaCorrection, cwd + "/../assets/utilities.frag");
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
        mUnigine01.generateMipmap(true);
        mUnigine01.setMagnificationFilter(GL_LINEAR);
        mUnigine01.setMinificationFilter(GL_LINEAR_MIPMAP_LINEAR);
        //mUnigine01.setFilter(GL_LINEAR);
        mUnigine01.setWrapMode(GL_CLAMP_TO_EDGE);

    }

    //demo Unigine01
    im = QImage(cwd + "/../assets/Unigine02.png", "PNG");
    if (!im.isNull())
    {
        //mUnigine02.set(im, GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE);
        mUnigine02.set(im, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
        mUnigine02.setFilter(GL_LINEAR);
        mUnigine02.setWrapMode(GL_CLAMP_TO_EDGE);
    }

    //texture grid
    im = QImage(cwd + "/../assets/grid.gif", "GIF");
    if (!im.isNull())
    {
        mTextureGrid.set(im, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
        mTextureGrid.setFilter(GL_LINEAR);
        mTextureGrid.setWrapMode(GL_REPEAT);
    }
}

//-----------------------------------------------------------------------------
void Viewer::loadVbos()
{
    //--------------------------
    //--- textured grid
    {
        const double kRectHalfSize = 75000; //75km

        float v[12] = {
            -kRectHalfSize, 0.0f, kRectHalfSize,
            kRectHalfSize, 0.0f, kRectHalfSize,
            kRectHalfSize, 0.0f, -kRectHalfSize,
            -kRectHalfSize, 0.0f, -kRectHalfSize };

        int i[6] = {
            0, 1, 3,
            1, 2, 3 };

        const double kNumRepeat = 400;
        float t[8] = {
            0.0, 0.0,
            kNumRepeat, 0.0,
            kNumRepeat, kNumRepeat,
            0.0, kNumRepeat
        };

        mTexturedGridVbo.setVertices(12, v);
        mTexturedGridVbo.setIndices(6, i);
        mTexturedGridVbo.set2dTextureCoordinates(8, t);
        mTexturedGridVbo.bake();
    }
    

    //--------------------------
    //--- millions of polygon
    {
        const int kNumQuadPerSide = 1000;
        const double kRadius = 200;
        const int kNumVertexPerSide = kNumQuadPerSide + 1;
        const double kIncrementPerVertex = 2.0;

        //each vertex has 3 floats.
        const int kNumVertices = kNumVertexPerSide * kNumVertexPerSide * 3;
        float *vertices = new float[kNumVertices];

        //one quad requires 6 index, because 2 triangles
        const int kNumIndices = kNumQuadPerSide * kNumQuadPerSide * 6;
        int *indices = new int[kNumIndices];

        //one normal per vertex, 3 floats per normal
        const int kNumNormals = kNumVertexPerSide * kNumVertexPerSide * 3;
        float *normals = new float[kNumNormals];

        const int kNumTextureCoords = kNumVertexPerSide * kNumVertexPerSide * 2;
        float *texCoords = new float[kNumTextureCoords];

        //add all vertex and translate a bit so the origin is in the middle of the patch
        for (int j = 0; j < kNumVertexPerSide; ++j)
        {
            for (int i = 0; i < kNumVertexPerSide; ++i)
            {
                //add a vertex
                Vector3d a0( i * kIncrementPerVertex, kRadius, j * kIncrementPerVertex );
                a0 += Vector3d( -(kNumQuadPerSide/2.0)*kIncrementPerVertex, 0, -(kNumQuadPerSide/2.0)*kIncrementPerVertex );
                a0.normalise();
                a0 *= kRadius;
                //translate back to origin
                a0 += Vector3d(0, -kRadius, 0);
                
                const int vIndex = (j*kNumVertexPerSide*3) + i*3;
                vertices[vIndex + 0] = a0.x();
                vertices[vIndex + 1] = a0.y();
                vertices[vIndex + 2] = a0.z();

                normals[vIndex + 0] = a0.x();
                normals[vIndex + 1] = a0.y();
                normals[vIndex + 2] = a0.z();

                const int kNumTexRepeat = 400;
                const int tIndex = (j*kNumVertexPerSide*2) + i*2;;
                texCoords[tIndex + 0] = i/(double)kNumVertexPerSide * kNumTexRepeat;
                texCoords[tIndex + 1] = j/(double)kNumVertexPerSide * kNumTexRepeat;
            }
        }

        //perform mesh indices
        for(int j = 0; j < kNumQuadPerSide; ++j)
            for(int i = 0; i < kNumQuadPerSide; ++i)
            {
                //2 triangles per quad
                const int index0 = j*kNumQuadPerSide*2*3 + i*2*3;
                const int index1 = index0 + 3 ;

                //triangle 0
                indices[ index0 + 0 ] = j*kNumVertexPerSide + i;
                indices[ index0 + 1 ] = (j+1)*kNumVertexPerSide + i;
                indices[ index0 + 2 ] = j*kNumVertexPerSide + (i+1);

                //triangle 1
                indices[ index1 + 0 ] = (j+1)*kNumVertexPerSide + i;
                indices[ index1 + 1 ] = (j+1)*kNumVertexPerSide + (i+1);
                indices[ index1 + 2 ] = j*kNumVertexPerSide + (i+1);
            }

        mMillionsOfPolygonVbo.setVertices(kNumVertices, vertices);
        mMillionsOfPolygonVbo.setIndices(kNumIndices, indices);
        mMillionsOfPolygonVbo.setNormals(kNumNormals, normals);
        mMillionsOfPolygonVbo.set2dTextureCoordinates(kNumTextureCoords, texCoords);
        mMillionsOfPolygonVbo.bake();

        delete[] vertices;
        delete[] indices;
        delete[] normals;
    }
    
}

//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
    utils::Timer perFrameTimer;
    utils::Timer antiAliasTimer;

    renderTarget finalRt[2] = { rtFinal_0, rtFinal_1 };
    int finalRtIndex = mFrameIndex % 2;
    int previousFinalRtIndex = (finalRtIndex + 1) % 2;

    switch (mpMainDialog->getAntiAliasingMode())
    {
    case aamNoAA:
        drawSceneToColorFbo(rtRGBA);
        break;
    case aamSmaa1x:
    {
        drawSceneToColorFbo(rtSRGBA);

        antiAliasTimer.start();
        doSmaa1x(rtSRGBA, rtFinal_0);
        mTimeToAntialias.add(antiAliasTimer.getElapsed());
    }break;
    case aamSmaaT2x:
    {
        drawSceneToColorFbo(rtSRGBA);

        antiAliasTimer.start();
        doSmaa1x(rtSRGBA, finalRt[finalRtIndex]);
        doReprojection(finalRt[previousFinalRtIndex], finalRt[finalRtIndex]);
        mTimeToAntialias.add(antiAliasTimer.getElapsed());
    } break;
    case aamSmaaS2x:
    {
        glEnable(GL_MULTISAMPLE);
        doMsaa(2);

        antiAliasTimer.start();
        doSmaaSeparate();
        doSmaa1x(rtSeparate_0, rtFinal_0, 0);
        doSmaa1x(rtSeparate_1, rtFinal_0, 1);
        mTimeToAntialias.add(antiAliasTimer.getElapsed());

        glDisable(GL_MULTISAMPLE);
    } break;
    case aamSmaa4x:
        glEnable(GL_MULTISAMPLE);
        doMsaa(2);

        antiAliasTimer.start();
        doSmaaSeparate();
        doSmaa1x(rtSeparate_0, finalRt[finalRtIndex], 0);
        doSmaa1x(rtSeparate_1, finalRt[finalRtIndex], 1);
        doReprojection(finalRt[previousFinalRtIndex], finalRt[finalRtIndex]);
        mTimeToAntialias.add(antiAliasTimer.getElapsed());

        glDisable(GL_MULTISAMPLE);
        break;
    case aamMSAA2x:
    {
        glEnable(GL_MULTISAMPLE);
        doMsaa(2);

        antiAliasTimer.start();
        resolveMsaaTo(rtFinal_0);
        mTimeToAntialias.add(antiAliasTimer.getElapsed());

        glDisable(GL_MULTISAMPLE);
    }break;
    case aamMSAA4x:
    {
        glEnable(GL_MULTISAMPLE);
        doMsaa(4);

        antiAliasTimer.start();
        resolveMsaaTo(rtFinal_0);
        mTimeToAntialias.add(antiAliasTimer.getElapsed());

        glDisable(GL_MULTISAMPLE);
    }break;
    case aamMSAA8x:
    {
        glEnable(GL_MULTISAMPLE);
        doMsaa(8);

        antiAliasTimer.start();
        resolveMsaaTo(rtFinal_0);
        mTimeToAntialias.add(antiAliasTimer.getElapsed());

        glDisable(GL_MULTISAMPLE);
    }break;
    case aamMSAA16x:
    {
        glEnable(GL_MULTISAMPLE);
        doMsaa(16);

        antiAliasTimer.start();
        resolveMsaaTo(rtFinal_0);
        mTimeToAntialias.add(antiAliasTimer.getElapsed());

        glDisable(GL_MULTISAMPLE);
    }break;
	case aamMSAA32x:
	{
		glEnable(GL_MULTISAMPLE);
		doMsaa(32);

		antiAliasTimer.start();
		resolveMsaaTo(rtFinal_0);
		mTimeToAntialias.add(antiAliasTimer.getElapsed());

		glDisable(GL_MULTISAMPLE);
	}break;
    default: break;
    }

    displayPass(mColorFbo, mpMainDialog->getPassToDisplay());

    ++mFrameIndex;

    mTimeInterFrameStats.add(mInterFrameTimer.getElapsed());
    mTimePerFrameStats.add(perFrameTimer.getElapsed());
    mInterFrameTimer.start();

    if (mpMainDialog->shouldSaveFboPass())
    {
        mpMainDialog->resetSaveFboPassFlag();
    }
    std::string glErrors;
    if (treeD::hasGlError(&glErrors))
        printf("Gl error: %s\n", glErrors.c_str());
}

//-----------------------------------------------------------------------------
void Viewer::resetCamera()
{
    Camera c = getCamera();

    const Vector3d yUp(0.0, 1.0, 0.0);
    const Point3d origin(0.0, 0.0, 0.0);
    switch (gSceneContent)
    {
    case scGeometricGrid: c.set( Point3d(0.0, 5.0, 100.0), origin, yUp ); break;
    case scTexturedGrid: c.set( Point3d(0.0, 5.0, 100.0), origin, yUp ); break;
    case scMillionsOfPolygons: c.set( Point3d(0.0, 100.0, 800), origin, yUp ); break;
    case scUnigine01: c.set( Point3d(500.0, 500.0, 5000.0), Point3d(500.0, 500.0, 0), yUp ); break;
    case scUnigine02: c.set( Point3d(500.0, 500.0, 5000.0), Point3d(500.0, 500.0, 0), yUp ); break;
    default: break;
    }

    setCamera(c, true, 500);
}

//-----------------------------------------------------------------------------
void Viewer::resolveMsaaTo(renderTarget iRt)
{
    //resolve the multisample buffer to normal renderable buffer
    mMultisampleFbo.resolveTo(msaaRtSRGB, mColorFbo, rtSRGBA);

    //perform gamma correction
    mColorFbo.begin();
    {
        glDisable(GL_LIGHTING);
        //disable all depth test for the post processing stages...
        glDisable(GL_DEPTH_TEST);

        //gamma corretion  to final
        mColorFbo.drawTo(iRt);
        {
            //glClear(GL_COLOR_BUFFER_BIT);
            ScreenSpaceProjection sp(getCamera().getViewport().getSize());
            mGammaCorrection.begin();
            mGammaCorrection.setUniform("modelViewMatrix", sp.mViewMatrix);
            mGammaCorrection.setUniform("projectionMatrix", sp.mProjectionMatrix);
            mGammaCorrection.setUniform("uColorTex", 0);

            Texture t = mColorFbo.getColorAttachment(rtSRGBA);
            drawRectangle(t.getId(), Vector2d(t.width(), t.height()));

            mGammaCorrection.end();
        }
    }
    mColorFbo.end();

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

//-----------------------------------------------------------------------------
void Viewer::resizeGL(int iW, int iH)
{
    Widget3d::resizeGL(iW, iH);

    if (!(mColorFbo.getWidth() == iW && mColorFbo.getHeight() == iH))
    {
        mColorFbo.resize(iW, iH);
        mMultisampleFbo.resize(iW, iH);
        loadShaders();
        printf("FBO size: %d, %d\n", iW, iH);
    }
}

//-----------------------------------------------------------------------------
void Viewer::saveAllSmaa1xPassToPng(int iPass)
{
    QString s("F:/pass_");
    s += QString::number(iPass) + " ";

    mColorFbo.getColorAttachment(rtSRGBA).asQImage().save(s + " - 1 - no gamma correction.bmp", "BMP");
    mColorFbo.getColorAttachment(rtRGBA).asQImage().save(s + " - 2 - gamma corection.bmp", "BMP");
    mColorFbo.getColorAttachment(rtEdge).asQImage().save(s + " - 3 -  edge.bmp", "BMP");
    mColorFbo.getColorAttachment(rtBlendWeight).asQImage().save(s + " - 4 - blendweight.bmp", "BMP");
    mColorFbo.getColorAttachment(rtSeparate_0).asQImage().save(s + " - 5 - separate_0.bmp", "BMP");
    mColorFbo.getColorAttachment(rtSeparate_1).asQImage().save(s + " - 6 - separate_1.bmp", "BMP");
    mColorFbo.getColorAttachment(rtFinal_0).asQImage().save(s + " - 7 - final_0.bmp", "BMP");
    mColorFbo.getColorAttachment(rtFinal_1).asQImage().save(s + " - 8 - final_1.bmp", "BMP");
}

static double sPitchNoddingIncrement = 0.0001;
static double sYawNoddingIncrement = sPitchNoddingIncrement;
static double sCameraPitchNoddingAngle = 0.0;
static double sCameraYawNoddingAngle = 0.0;
//-----------------------------------------------------------------------------
void Viewer::tiltMatrix(realisim::math::Matrix4* iM, bool iYaw, bool iPitch) const
{
    const double maxAngle = 0.05;
    if (sCameraPitchNoddingAngle > maxAngle)
        sPitchNoddingIncrement *= -1;
    if (sCameraPitchNoddingAngle < -maxAngle)
        sPitchNoddingIncrement *= -1;

    if (sCameraYawNoddingAngle > maxAngle)
        sYawNoddingIncrement *= -1;
    if (sCameraYawNoddingAngle < -maxAngle)
        sYawNoddingIncrement *= -1;

    sCameraPitchNoddingAngle = iPitch ? sCameraPitchNoddingAngle + sPitchNoddingIncrement : 0;
    sCameraYawNoddingAngle = iYaw ? sCameraYawNoddingAngle + sYawNoddingIncrement : 0;


    *iM = Matrix4(sCameraPitchNoddingAngle, Vector3d(1.0, 0.0, 0.0)) * (*iM);
    *iM = Matrix4(sCameraYawNoddingAngle, Vector3d(0.0, 1.0, 0.0)) * (*iM);
}

//-----------------------------------------------------------------------------
void Viewer::toggleFullScreen()
{ mpMainDialog->toggleFullScreen(); }

//-----------------------------------------------------------------------------
//--- MainDialog
//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QMainWindow(),
mpViewer(0),
mTimerEventId(0),
mAntiAliasingMode(aamSmaa1x),
mHas3dControlEnabled(false),
mIsCameraPitchNodding(false),
mIsCameraYawNodding(false),
mHasDebugPassEnabled(false),
mDebugPassToDisplay(rtRGBA),
mSaveColorFboPassToPng(false),
mHasShading(false),
mIsWireFrameShown(false),
mSmaaPresetQuality(spqHigh)
{
    resize(1280, 720);

    QWidget *pCentralWidget = new QWidget(this);
    setCentralWidget(pCentralWidget);

    QHBoxLayout* pLyt = new QHBoxLayout(pCentralWidget);
    pLyt->setMargin(2);
    pLyt->setSpacing(5);
    {
        mpLeftPanel = new QFrame(pCentralWidget);
        QVBoxLayout *pControlLyt = new QVBoxLayout(mpLeftPanel);
        {
            //--- anti aliasing mode
            QGroupBox *pAntiAlisingMode = new QGroupBox(mpLeftPanel);
            {
                QVBoxLayout *pLyt = new QVBoxLayout(pAntiAlisingMode);
                {
                    QHBoxLayout *pL_1 = new QHBoxLayout();
                    {
                        QLabel* l = new QLabel("AA mode:", pAntiAlisingMode);
                        mpAntiAliasingModeCombo = new QComboBox(pAntiAlisingMode);
                        for (int i = 0; i < aamCount; ++i)
                        {
                            mpAntiAliasingModeCombo->addItem(toQString((antiAliasingMode)i));
                        }
                        connect(mpAntiAliasingModeCombo, SIGNAL(activated(int)),
                            this, SLOT(antiAliasingModeChanged(int)));

                        pL_1->addWidget(l);
                        pL_1->addWidget(mpAntiAliasingModeCombo);
                        pL_1->addStretch(1);
                    }

                    QHBoxLayout *pL_1_1 = new QHBoxLayout();
                    {
                        QLabel* l = new QLabel("Smaa preset quality:", pAntiAlisingMode);
                        mpSmaaPresetQuality = new QComboBox(pAntiAlisingMode);
                        for (int i = 0; i < spqCount; ++i)
                        {
                            mpSmaaPresetQuality->addItem( ::toQString((smaaPresetQuality)i));
                        }
                        connect(mpSmaaPresetQuality, SIGNAL(activated(int)),
                            this, SLOT(smaaPresetQualityChanged(int)));

                        pL_1_1->addWidget(l);
                        pL_1_1->addWidget(mpSmaaPresetQuality);
                        pL_1_1->addStretch(1);
                    }

                    mpDebugPassEnabled = new QCheckBox("Debug pass", pAntiAlisingMode);
                    connect(mpDebugPassEnabled, SIGNAL(clicked()), this, SLOT(enableDebugPassClicked()));

                    QHBoxLayout *pL1 = new QHBoxLayout();
                    {
                        QLabel* l = new QLabel("Pass displayed (F/G):", pAntiAlisingMode);
                        mpPassDisplayed = new QLabel("0", pAntiAlisingMode);

                        pL1->addWidget(l);
                        pL1->addWidget(mpPassDisplayed);
                        pL1->addStretch(1);
                    }

                    QHBoxLayout *pL2 = new QHBoxLayout();
                    {
                        QLabel* l = new QLabel("Scene content (C):", pAntiAlisingMode);
                        mpSceneContent = new QLabel("3d", pAntiAlisingMode);

                        pL2->addWidget(l);
                        pL2->addWidget(mpSceneContent);
                        pL2->addStretch(1);
                    }

                    mpUseMipmaps = new QCheckBox("Use mipmaps", pAntiAlisingMode);
                    connect(mpUseMipmaps, SIGNAL(clicked()), this, SLOT(useMipMapsClicked()));

                    mpApplyShading = new QCheckBox("apply shading", pAntiAlisingMode);
                    connect(mpApplyShading, SIGNAL(clicked()), this, SLOT(applyShadingClicked()));

                    mpShowWireFrame = new QCheckBox("show wireframe", pAntiAlisingMode);
                    connect(mpShowWireFrame, SIGNAL(clicked()), this, SLOT(showWireFrameClicked()));

                    pLyt->addLayout(pL_1);
                    pLyt->addLayout(pL_1_1);
                    pLyt->addWidget(mpDebugPassEnabled);
                    pLyt->addLayout(pL1);
                    pLyt->addLayout(pL2);
                    pLyt->addWidget(mpUseMipmaps);
                    pLyt->addWidget(mpApplyShading);
                    pLyt->addWidget(mpShowWireFrame);
                }
            }

            //--- camera controls
            QGroupBox *pCameraControls = new QGroupBox("Camera controls", mpLeftPanel);
            {
                QVBoxLayout *pL = new QVBoxLayout(pCameraControls);
                {
                    mpEnable3dControls = new QCheckBox("Enable 3d controls", pCameraControls);
                    connect(mpEnable3dControls, SIGNAL(clicked()), this, SLOT(enable3dControlsClicked()));

                    mpEnableCameraPitchNodding = new QCheckBox("Enable camera pitch nodding", pCameraControls);
                    connect(mpEnableCameraPitchNodding, SIGNAL(clicked()), this, SLOT(enableCameraPitchNodding()));

                    mpEnableCameraYawNodding = new QCheckBox("Enable camera yaw nodding", pCameraControls);
                    connect(mpEnableCameraYawNodding, SIGNAL(clicked()), this, SLOT(enableCameraYawNodding()));

                    pL->addWidget(mpEnable3dControls);
                    pL->addWidget(mpEnableCameraPitchNodding);
                    pL->addWidget(mpEnableCameraYawNodding);
                }
            }

            QGroupBox *pProfiling = new QGroupBox("Profiling", mpLeftPanel);
            {
                QVBoxLayout *pL = new QVBoxLayout(pProfiling);
                {
                    QLabel *info = new QLabel("timing - [mean (ms), std (ms)]", pProfiling);

                    QHBoxLayout *pL1 = new QHBoxLayout();
                    {
                        QLabel *l = new QLabel("per frame:");
                        mpPerFrameStats = new QLabel("n/a");

                        pL1->addWidget(l);
                        pL1->addWidget(mpPerFrameStats);
                    }

                    QHBoxLayout *pL2 = new QHBoxLayout();
                    {
                        QLabel *l = new QLabel("inter frame:");
                        mpInterFrameStats = new QLabel("n/a");

                        pL2->addWidget(l);
                        pL2->addWidget(mpInterFrameStats);
                    }

                    QHBoxLayout *pL2_1 = new QHBoxLayout();
                    {
                        QLabel *l = new QLabel("time to antialias:");
                        mpTimeToAntialias = new QLabel("n/a");

                        pL2_1->addWidget(l);
                        pL2_1->addWidget(mpTimeToAntialias);
                    }

                    QHBoxLayout *pL3 = new QHBoxLayout();
                    {
                        QPushButton *pClear = new QPushButton("Clear");
                        connect(pClear, SIGNAL(clicked()), this, SLOT(clearProfilingClicked()));

                        pL3->addStretch(1);
                        pL3->addWidget(pClear);
                    }

                    QHBoxLayout *pL4 = new QHBoxLayout();
                    {
                        QPushButton *pSaveAllFbo = new QPushButton("Save all fbo pass");
                        connect(pSaveAllFbo, SIGNAL(clicked()), this, SLOT(saveAllFboPass()));

                        pL4->addWidget(pSaveAllFbo);
                        pL4->addStretch(1);
                    }

                    pL->addWidget(info);
                    pL->addLayout(pL1);
                    pL->addLayout(pL2);
                    pL->addLayout(pL2_1);
                    pL->addLayout(pL3);
                    pL->addLayout(pL4);
                }
            }

            pControlLyt->addWidget(pAntiAlisingMode);
            pControlLyt->addStretch(1);
            pControlLyt->addWidget(pCameraControls);
            pControlLyt->addWidget(pProfiling);
        }

        mpViewer = new Viewer(pCentralWidget, this);

        pLyt->addWidget(mpLeftPanel, 1);
        pLyt->addWidget(mpViewer, 4);
    }

    treeD::Camera c = mpViewer->getCamera();
    c.set(Point3d(0.0, 0.0, 50),
        Point3d(), Vector3d(0, 1, 0));
    Camera::Projection p = c.getProjection();
    p.mNear = 1;
    p.mFar = 150000; //150km
    c.setProjection(p);
    mpViewer->setCamera(c);
    mpViewer->setControlType(treeD::Widget3d::ctFree);

    mTimerEventId = startTimer(16);
    updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::antiAliasingModeChanged(int iIndex)
{
    setAntiAliasingMode((antiAliasingMode)iIndex);

    //clear all color buffer
    mpViewer->mColorFbo.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mpViewer->mMultisampleFbo.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//-----------------------------------------------------------------------------
void MainDialog::applyShadingClicked()
{
    mHasShading = mpApplyShading->isChecked();
    updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::clearProfilingClicked()
{
    mpViewer->mTimePerFrameStats.clear();
    mpViewer->mTimeInterFrameStats.clear();
    mpViewer->mTimeToAntialias.clear();
    updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::displayNextDebugPass()
{
    mDebugPassToDisplay = (mDebugPassToDisplay + 1) % rtCount;
}

//-----------------------------------------------------------------------------
void MainDialog::displayPreviousDebugPass()
{
    --mDebugPassToDisplay;
    if (mDebugPassToDisplay < 0)
    {
        mDebugPassToDisplay = rtCount - 1;
    }
    updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::enable3dControlsClicked()
{
    mHas3dControlEnabled = mpEnable3dControls->isChecked();
    updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::enableCameraPitchNodding()
{
    mIsCameraPitchNodding = mpEnableCameraPitchNodding->isChecked();
    updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::enableCameraYawNodding()
{
    mIsCameraYawNodding = mpEnableCameraYawNodding->isChecked();
    updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::enableDebugPassClicked()
{
    mHasDebugPassEnabled = mpDebugPassEnabled->isChecked();
    updateUi();
}

//-----------------------------------------------------------------------------
renderTarget MainDialog::getPassToDisplay() const
{
    renderTarget rt;

    if (hasDebugPassEnabled())
    {
        rt = (renderTarget)mDebugPassToDisplay;
    }
    else
    {
        switch (getAntiAliasingMode())
        {
        case aamNoAA: rt = rtRGBA; break;
        case aamSmaa1x: rt = rtFinal_0; break;
        case aamSmaaT2x:
        {
            rt = (renderTarget)(rtFinal_0 + (mpViewer->mFrameIndex % 2));
        }break;
        case aamSmaaS2x:
        {
            rt = rtFinal_0;
        }break;
        case aamSmaa4x:
        {
            rt = (renderTarget)(rtFinal_0 + (mpViewer->mFrameIndex % 2));
        }break;
        case aamMSAA2x:
        case aamMSAA4x:
        case aamMSAA8x:
        case aamMSAA16x:
		case aamMSAA32x:
            rt = rtFinal_0;
            break;
        default: rt = rtFinal_0; break;
        }
    }
    return rt;
}

//-----------------------------------------------------------------------------
Texture MainDialog::getTextureFromSceneContent()
{
    Texture t;
    switch (gSceneContent)
    {
    case scGeometricGrid: break;
    case scTexturedGrid: t = mpViewer->mTextureGrid; break;
    case scMillionsOfPolygons: t = mpViewer->mTextureGrid; break;
    case scUnigine01: t = mpViewer->mUnigine01; break;
    case scUnigine02: t = mpViewer->mUnigine02; break;
    default: break;
    }
    return t;
}

//-----------------------------------------------------------------------------
void MainDialog::resetSaveFboPassFlag()
{
    mSaveColorFboPassToPng = false;
}

//-----------------------------------------------------------------------------
void MainDialog::saveAllFboPass()
{
    mSaveColorFboPassToPng = true;
}

//-----------------------------------------------------------------------------
void MainDialog::setAntiAliasingMode(antiAliasingMode iM)
{
    mAntiAliasingMode = iM;
    updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::setSmaaPresetQuality(smaaPresetQuality iQ)
{ 
    mSmaaPresetQuality = iQ;
    mpViewer->loadShaders();
    updateUi();
}

//-----------------------------------------------------------------------------
bool MainDialog::shouldSaveFboPass() const
{
    return mSaveColorFboPassToPng;
}

//-----------------------------------------------------------------------------
void MainDialog::showWireFrameClicked()
{
    mIsWireFrameShown = mpShowWireFrame->isChecked();       

    updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::smaaPresetQualityChanged(int iQ)
{
    setSmaaPresetQuality( (smaaPresetQuality)iQ );
}

//-----------------------------------------------------------------------------
void MainDialog::timerEvent(QTimerEvent *ipE)
{
    if (ipE->timerId() == mTimerEventId)
    {
    }

    updateUiHighFrequency();
}

//-----------------------------------------------------------------------------
void MainDialog::toggleFullScreen()
{
    if (isFullScreen())
    {
        mpLeftPanel->show();
        showNormal();
    }
    else
    {
        mpLeftPanel->hide();
        showFullScreen();
    }
}

//-----------------------------------------------------------------------------
QString MainDialog::toQString(antiAliasingMode iMode) const
{
    QString r("unknown");
    switch (iMode)
    {
    case aamNoAA: r = "No AA"; break;
    case aamSmaa1x: r = "SMAA 1X"; break;
    case aamSmaaT2x: r = "SMAA T2x"; break;
    case aamSmaaS2x: r = "SMAA S2x"; break;
    case aamSmaa4x: r = "SMAA 4x"; break;
    case aamMSAA2x: r = "MSAA 2X"; break;
    case aamMSAA4x: r = "MSAA 4X"; break;
    case aamMSAA8x: r = "MSAA 8X"; break;
    case aamMSAA16x: r = "MSAA 16X"; break;
	case aamMSAA32x: r = "MSAA 32X"; break;
    default: break;
    }
    return r;
}

//-----------------------------------------------------------------------------
void MainDialog::updateUi()
{
    //--- smaa mode
    const int index = mpAntiAliasingModeCombo->currentIndex();
    if (index != getAntiAliasingMode())
    {
        mpAntiAliasingModeCombo->setCurrentIndex(getAntiAliasingMode());
    }

    const int smaaPresetQuality = mpSmaaPresetQuality->currentIndex();
    if (smaaPresetQuality != getSmaaPresetQuality())
    {
        mpSmaaPresetQuality->setCurrentIndex(getSmaaPresetQuality());
    }

    //--- show pass name
    QString passName = "Raw scene";

    switch (getPassToDisplay())
    {
    case rtSRGBA: passName = "No Gamma correction"; break;
    case rtRGBA: passName = "Gamma correction"; break;
    case rtEdge: passName = "Edge detection"; break;
    case rtBlendWeight: passName = "Blend weight"; break;
    case rtFinal_0: passName = "Final_0"; break;
    case rtFinal_1: passName = "Final_1"; break;
    case rtSeparate_0: passName = "Separate_0"; break;
    case rtSeparate_1: passName = "Separate_1"; break;
    default: passName = "Unknown"; break;
    }

    mpPassDisplayed->setText(passName);

    //--- show scene content
    QString sceneContent = "3d";
    switch (gSceneContent)
    {
    case scGeometricGrid: sceneContent = "geometric grid"; break;
    case scTexturedGrid: sceneContent = "textured grid"; break;
    case scMillionsOfPolygons: sceneContent = "millions of polygons"; break;
    case scUnigine01: sceneContent = "Unigine01"; break;
    case scUnigine02: sceneContent = "Unigine02"; break;

    default: sceneContent = "unknown"; break;
    }
    mpSceneContent->setText(sceneContent);

    //mipmap associated to texture
    Texture t = getTextureFromSceneContent();
    mpUseMipmaps->setChecked(t.hasMipMaps());
    mpUseMipmaps->setEnabled(t.isValid());

    //shading associated to scene content
    mpApplyShading->setChecked(mHasShading);
    mpApplyShading->setVisible(gSceneContent == scGeometricGrid || 
        gSceneContent == scMillionsOfPolygons);

    mpShowWireFrame->setChecked( isWireFrameShown() );

    //--- camera control
    mpEnableCameraPitchNodding->setEnabled(mpEnable3dControls->isChecked());
    mpEnableCameraYawNodding->setEnabled(mpEnable3dControls->isChecked());

    //the profiling is in updateHighFrequency
    updateUiHighFrequency();

    update();
    mpViewer->update();
}

//-----------------------------------------------------------------------------
void MainDialog::updateUiHighFrequency()
{
    //--- profiling
    stringstream iss;
    iss << setprecision(3) << fixed;
    iss << mpViewer->mTimePerFrameStats.getMean() * 1000.0 << ", " << mpViewer->mTimePerFrameStats.getStandardDeviation() * 1000.0;
    mpPerFrameStats->setText(QString::fromStdString(iss.str()));

    iss = stringstream();
    iss << setprecision(3) << fixed;
    iss << mpViewer->mTimeInterFrameStats.getMean() * 1000.0 << ", " << mpViewer->mTimeInterFrameStats.getStandardDeviation() * 1000.0;
    mpInterFrameStats->setText(QString::fromStdString(iss.str()));

    iss = stringstream();
    iss << setprecision(3) << fixed;
    iss << mpViewer->mTimeToAntialias.getMean() * 1000.0 << ", " << mpViewer->mTimeToAntialias.getStandardDeviation() * 1000.0;
    mpTimeToAntialias->setText(QString::fromStdString(iss.str()));

    update();
    mpViewer->update();
}

//-----------------------------------------------------------------------------
void MainDialog::useMipMapsClicked()
{
    Texture t = getTextureFromSceneContent();

    t.generateMipmap(mpUseMipmaps->isChecked());

    if (mpUseMipmaps->isChecked())
    {
        t.setMagnificationFilter(GL_LINEAR);
        t.setMinificationFilter(GL_LINEAR_MIPMAP_LINEAR);
    }
    else
    {
        t.setFilter(GL_LINEAR);
    }
}