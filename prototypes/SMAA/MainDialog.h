/*
 */

#ifndef MainDialog_hh
#define MainDialog_hh

#include "3d/Shader.h"
#include "3d/Texture.h"
#include "3d/Widget3d.h"

#include <QKeyEvent>
#include <QMainWindow>
#include <QLabel>
#include <vector>

class MainDialog;
class Viewer : public realisim::treeD::Widget3d
{
	friend class MainDialog;

public:
	Viewer(QWidget*, MainDialog*);
	~Viewer();

private:
    void blitToScreen(realisim::treeD::Texture, realisim::math::Vector2d);
	virtual void draw() override;
    void drawScene();
	virtual void keyPressEvent(QKeyEvent*);
    void loadTextures();
	void loadShaders();
	virtual void initializeGL() override;
	virtual void resizeGL(int, int) override;

    MainDialog* mpMainDialog;

	realisim::treeD::FrameBufferObject mFbo;
    realisim::treeD::Shader mMLAAShader;
    realisim::treeD::Shader mGammaCorrection;
    realisim::treeD::Texture mUnigine01;
    realisim::treeD::Texture mUnigine02;
};


class MainDialog : public QMainWindow
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog() {};

    void updateUi();

protected:
    virtual void timerEvent(QTimerEvent*) override;

	//--- ui
	Viewer* mpViewer;
    QLabel* mpRotationState;
    QLabel* mpPostProcessingState;
    QLabel* mpPassDisplayed;
    QLabel* mpSceneContent;

	//--- data
    int mTimerEventId;
};

#endif
