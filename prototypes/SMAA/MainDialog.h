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
    void addFragmentSource(realisim::treeD::Shader*, QString iFileName);
    void addVertexSource(realisim::treeD::Shader*, QString iFileName);
    void drawRectangle(int, realisim::math::Vector2d);
    void drawStillImage(int, realisim::math::Vector2d);
    void drawScene();
    virtual void keyPressEvent(QKeyEvent*);
    void loadTextures();
    void loadShaders();
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int, int) override;

    MainDialog* mpMainDialog;

    realisim::treeD::FrameBufferObject mFbo;
    realisim::treeD::Shader mSmaaShader;
    realisim::treeD::Shader mSmaa2ndPassShader;
    realisim::treeD::Shader mSceneShader;
    realisim::treeD::Shader mStillImageShader;
    realisim::treeD::Texture mSmaaAreaTexture;
    realisim::treeD::Texture mSmaaSearchTexture;
    realisim::treeD::Texture mUnigine01;
    realisim::treeD::Texture mUnigine02;
    realisim::treeD::Texture mMandelbrot;
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
