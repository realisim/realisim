/*
 */

#ifndef MainDialog_hh
#define MainDialog_hh

#include "3d/MultisampleFrameBufferObject.h"
#include "3d/Shader.h"
#include "3d/Texture.h"
#include "3d/Widget3d.h"

#include <QCheckBox>
#include <QComboBox>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <utils/Statistics.h>
#include <utils/Timer.h>
#include <vector>

enum antiAliasingMode { aamNoAA, aamSmaa1x, aamSmaaT2x, aamSmaaS2x, aamSmaa4x, aamMSAA2x, aamMSAA4x, aamMSAA8x, aamMSAA16x, aamCount };
enum renderTarget{ rtSRGBA=0, rtRGBA, rtEdge, rtBlendWeight, rtFinal_0,
	rtFinal_1, rtSeparate_0, rtSeparate_1, rtCount};
enum msaaRenderTarget{msaaRtSRGB=0, msaaRtCount};

using namespace realisim;

class MainDialog;
class Viewer : public treeD::Widget3d
{
    friend class MainDialog;

public:
    Viewer(QWidget*, MainDialog*);
    ~Viewer();

private:
    int addFragmentSource(treeD::Shader*, QString iFileName);
    int addVertexSource(treeD::Shader*, QString iFileName);	
	void displayPass(treeD::FrameBufferObject, int);
	void doMsaa(int iX);
	void doReprojection(renderTarget iPreviousFinalRt, renderTarget iFinalRt);
	void doSmaa1x(renderTarget iInput, renderTarget iOutput, int pass = 0);
	void doSmaaSeparate();
    void drawRectangle(int, math::Vector2d);
    void drawStillImage(int, math::Vector2d, const math::Matrix4& iView, const math::Matrix4& iProj);
    void drawScene();
	void drawSceneToColorFbo(renderTarget);
	math::Matrix4 getJitterMatrix() const;
	math::Vector4d getSubsampleIndices(int pass) const;
    virtual void keyPressEvent(QKeyEvent*);
    void loadTextures();
    void loadShaders();
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int, int) override;
	void resolveMsaaTo(renderTarget);
	void saveAllSmaa1xPassToPng(int);
	void tiltMatrix(math::Matrix4*, bool iYaw, bool iPitch) const;

    MainDialog* mpMainDialog;

    treeD::FrameBufferObject mColorFbo;
	treeD::MultisampleFrameBufferObject mMultisampleFbo;
    treeD::Shader mSmaaShader;
    treeD::Shader mSmaa2ndPassShader;
    treeD::Shader mSmaa3rdPassShader;
	treeD::Shader mSmaaReprojectionShader;
	treeD::Shader mSmaaSeparateShader;
    treeD::Shader mSceneShader;
    treeD::Shader mStillImageShader;
    treeD::Shader mGammaCorrection;
    treeD::Texture mSmaaAreaTexture;
    treeD::Texture mSmaaSearchTexture;
    treeD::Texture mUnigine01;
    treeD::Texture mUnigine02;
    treeD::Texture mMandelbrot;

	int mFrameIndex;
	utils::Timer mInterFrameTimer;
	utils::Statistics mTimePerFrameStats;
	utils::Statistics mTimeInterFrameStats;
		
	math::Matrix4 mPreviousWorldView;
	math::Matrix4 mPreviousWorldProj;
};


class MainDialog : public QMainWindow
{
    Q_OBJECT
public:
    MainDialog();
    ~MainDialog() {};
	void displayNextDebugPass();
	void displayPreviousDebugPass();
	antiAliasingMode getAntiAliasingMode() const {return mAntiAliasingMode;}
	renderTarget getPassToDisplay() const;
	bool has3dControlEnabled() const {return mHas3dControlEnabled;}
	bool hasDebugPassEnabled() const {return mHasDebugPassEnabled;}
	bool isCameraPitchNodding() const {return mIsCameraPitchNodding;}
	bool isCameraYawNodding() const {return mIsCameraYawNodding;}
	void updateUi();	
	void resetSaveFboPassFlag();
	bool shouldSaveFboPass() const;

protected slots:
	void antiAliasingModeChanged(int);
	void clearProfilingClicked();
	void enable3dControlsClicked();
	void enableCameraPitchNodding();
	void enableCameraYawNodding();
	void enableDebugPassClicked();
	void saveAllFboPass();

protected:
	void setAntiAliasingMode(antiAliasingMode iM);
	QString toQString(antiAliasingMode) const;
    virtual void timerEvent(QTimerEvent*) override;
	void updateUiHighFrequency();

    //--- ui
    Viewer* mpViewer;

	QComboBox* mpAntiAliasingModeCombo;
	QCheckBox *mpDebugPassEnabled;
    QLabel* mpPassDisplayed;
    QLabel* mpSceneContent;

	//--- camera control
	QCheckBox* mpEnable3dControls;
	QCheckBox* mpEnableCameraPitchNodding;
	QCheckBox* mpEnableCameraYawNodding;

	//--- profile info
	QLabel* mpPerFrameStats;
	QLabel* mpInterFrameStats;

    //--- data
    int mTimerEventId;
	antiAliasingMode mAntiAliasingMode;
	bool mHas3dControlEnabled;
	bool mIsCameraPitchNodding;
	bool mIsCameraYawNodding;
	bool mHasDebugPassEnabled;
	int mDebugPassToDisplay;
	bool mSaveColorFboPassToPng;
};

#endif
