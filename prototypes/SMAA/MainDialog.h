/*
 */

#ifndef MainDialog_hh
#define MainDialog_hh

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

enum antiAliasingMode { aamNoAA, aamSmaa1x, aamSmaaT2x, aamFSAA2x, aamCount };
enum renderTarget{ rtSRGB=0, rtRGB, rtEdge, rtBlendWeight, rtFinal_0,
	rtFinal_1, rtCount};

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
	void displayPass(int);
	void doNoSmaa();
	void doReprojection(renderTarget iPreviousFinalRt, renderTarget iFinalRt);
	void doSmaa1x(renderTarget iFinalRt);
    void drawRectangle(int, math::Vector2d);
    void drawStillImage(int, math::Vector2d, const math::Matrix4& iView, const math::Matrix4& iProj);
    void drawScene();
	math::Matrix4 getJitterMatrix() const;
	math::Vector4d getSubsampleIndices() const;
    virtual void keyPressEvent(QKeyEvent*);
    void loadTextures();
    void loadShaders();
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int, int) override;
	void tiltMatrix(math::Matrix4*) const;

    MainDialog* mpMainDialog;

    treeD::FrameBufferObject mSmaaFbo;
	treeD::FrameBufferObject mFsaaFbo;
    treeD::Shader mSmaaShader;
    treeD::Shader mSmaa2ndPassShader;
    treeD::Shader mSmaa3rdPassShader;
	treeD::Shader mReprojectionShader;
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
	bool isCameraNodding() const {return mIsCameraNodding;}
	void updateUi();

protected slots:
	void clearProfilingClicked();
	void enable3dControlsClicked();
	void enableCameraNodding();
	void enableDebugPassClicked();
	void antiAliasingModeChanged(int);

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
	QCheckBox* mpEnableCameraNodding;

	//--- profile info
	QLabel* mpPerFrameStats;
	QLabel* mpInterFrameStats;

    //--- data
    int mTimerEventId;
	antiAliasingMode mAntiAliasingMode;
	bool mHas3dControlEnabled;
	bool mIsCameraNodding;
	bool mHasDebugPassEnabled;
	int mDebugPassToDisplay;
};

#endif
