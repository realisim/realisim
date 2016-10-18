/*
 */

#ifndef MainDialog_hh
#define MainDialog_hh

#include "3d/MultisampleFrameBufferObject.h"
#include "3d/Shader.h"
#include "3d/Texture.h"
#include <3d/VertexBufferObject.h>
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

enum antiAliasingMode { aamNoAA, aamSmaa1x, aamSmaaT2x, aamSmaaS2x, aamSmaa4x, aamMSAA2x, aamMSAA4x, aamMSAA8x, aamMSAA16x, aamMSAA32x, aamCount };
//enum renderTarget{ rtSRGBA=0, rtRGBA, rtEdge, rtBlendWeight, rtFinal_0,
//	rtFinal_1, rtSeparate_0, rtSeparate_1, rtCount};

enum renderTargetColor{ rtSceneColor=0, rtFinalColor };
enum renderTargetSmaa{ rtEdge=0, rtBlendWeight, rtFinal_0,
		rtFinal_1, rtSeparate_0, rtSeparate_1, rtCount};

enum msaaRenderTarget{msaaRtScene=0, msaaRtCount};
enum smaaPresetQuality{spqLow=0, spqMedium, spqHigh, spqUltra, spqCount};

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
	void doReprojection(renderTargetSmaa iPreviousFinalRt, renderTargetSmaa iFinalRt, renderTargetColor iColorOutput);
	void doSmaa1x( treeD::FrameBufferObject iInput, int iInputColorAttachment, renderTargetSmaa iOutput, int pass = 0);
	void doSmaaSeparate();
    void drawRectangle(int, math::Vector2d);
    void drawStillImage(int, math::Vector2d, const math::Matrix4& iView, const math::Matrix4& iProj);
    void drawScene();
	void drawSceneToColorFbo();
	math::Matrix4 getJitterMatrix() const;
	math::Vector4d getSubsampleIndices(int pass) const;
    virtual void keyPressEvent(QKeyEvent*);
	void loadShaders();
    void loadTextures();
	void loadVbos();
    virtual void initializeGL() override;
    void initSmallHouses();
    virtual void paintGL() override;
    void resetCamera();
    virtual void resizeGL(int, int) override;
	void saveAllSmaa1xPassToPng(int);
    void toggleFullScreen();
	void tiltMatrix(math::Matrix4*, bool iYaw, bool iPitch) const;

    MainDialog* mpMainDialog;

    treeD::FrameBufferObject mColorFbo;
    treeD::FrameBufferObject mSmaaFbo;
	treeD::MultisampleFrameBufferObject mMultisampleFbo;
    treeD::Shader mSmaaShader;
    treeD::Shader mSmaa2ndPassShader;
    treeD::Shader mSmaa3rdPassShader;
	treeD::Shader mSmaaReprojectionShader;
	treeD::Shader mSmaaSeparateShader;
    treeD::Shader mSceneShader;
    treeD::Shader mOneTextureShader;
    treeD::Texture mSmaaAreaTexture;
    treeD::Texture mSmaaSearchTexture;
    treeD::Texture mUnigine01;
    treeD::Texture mUnigine02;
	treeD::Texture mTextureGrid;
	treeD::VertexBufferObject mTexturedGridVbo;
    treeD::VertexBufferObject mMillionsOfPolygonVbo;
    std::vector<math::Vector3d> mHousesPosition;
    std::vector<treeD::VertexBufferObject> mHouses;

	int mFrameIndex;
	utils::Timer mInterFrameTimer;
	utils::Statistics mTimePerFrameStats;
	utils::Statistics mTimeInterFrameStats;
	utils::Statistics mTimeToAntialias;
		
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
	renderTargetSmaa getPassToDisplay() const;
    smaaPresetQuality getSmaaPresetQuality() const {return mSmaaPresetQuality;}
	bool has3dControlEnabled() const {return mHas3dControlEnabled;}
	bool hasDebugPassEnabled() const {return mHasDebugPassEnabled;}
	bool hasShading() const {return mHasShading;}
	bool isCameraPitchNodding() const {return mIsCameraPitchNodding;}
	bool isCameraYawNodding() const {return mIsCameraYawNodding;}
    bool isWireFrameShown() const {return mIsWireFrameShown; }
	void updateUi();
	void resetSaveFboPassFlag();
    void setSmaaPresetQuality(smaaPresetQuality iQ);
	bool shouldSaveFboPass() const;
    void toggleFullScreen();


protected slots:
	void antiAliasingModeChanged(int);
	void applyShadingClicked();
	void clearProfilingClicked();
	void enable3dControlsClicked();
	void enableCameraPitchNodding();
	void enableCameraYawNodding();
	void enableDebugPassClicked();
	void saveAllFboPass();
    void smaaPresetQualityChanged(int);
    void showWireFrameClicked();
	void useMipMapsClicked();

protected:
	treeD::Texture getTextureFromSceneContent();
	void setAntiAliasingMode(antiAliasingMode iM);
	QString toQString(antiAliasingMode) const;
    virtual void timerEvent(QTimerEvent*) override;
	void updateUiHighFrequency();

    //--- ui
    Viewer* mpViewer;

    QFrame* mpLeftPanel;

	QComboBox* mpAntiAliasingModeCombo;
    QComboBox* mpSmaaPresetQuality;
	QCheckBox *mpDebugPassEnabled;
    QLabel* mpPassDisplayed;
    QLabel* mpSceneContent;
	QCheckBox* mpUseMipmaps;
	QCheckBox* mpApplyShading;
    QCheckBox* mpShowWireFrame;

	//--- camera control
	QCheckBox* mpEnable3dControls;
	QCheckBox* mpEnableCameraPitchNodding;
	QCheckBox* mpEnableCameraYawNodding;

	//--- profile info
	QLabel* mpPerFrameStats;
	QLabel* mpInterFrameStats;
	QLabel* mpTimeToAntialias;

    //--- data
    int mTimerEventId;
	antiAliasingMode mAntiAliasingMode;
	bool mHas3dControlEnabled;
	bool mIsCameraPitchNodding;
	bool mIsCameraYawNodding;
	bool mHasDebugPassEnabled;
	int mDebugPassToDisplay;
	bool mSaveColorFboPassToPng;
	bool mHasShading;
    bool mIsWireFrameShown;
    smaaPresetQuality mSmaaPresetQuality;
};

#endif
