/******************************************************************************
* Author: Pierre-Olivier Beaudoin                                                                    
* File name: Widget3d
* Description: Opengl Widget to display 3d
*
******************************************************************************/

/*

*/

#ifndef Realisim_Widget3d_hh
#define Realisim_Widget3d_hh

#include "3d/FrameBufferObject.h"
#include "3d/Shader.h"
#include "Camera.h"
#include <map>
class QKeyEvent;
#include <QTime>
#include <QGLWidget>

/*
	Documenter la relation entre pick() et drawSceneForPicking()
*/

namespace realisim
{
namespace treeD
{
class Widget3d : public QGLWidget
{
//    Q_OBJECT
public:
  Widget3d( QWidget* ipParent = 0,
            const QGLWidget* shareWidget = 0,
            Qt::WindowFlags iFlags = 0 );
  virtual ~Widget3d() = 0;

  const Camera& getCamera() const { return mCam; }
  void pushFrameBuffer(const FrameBufferObject& = FrameBufferObject());
  void pushShader(const Shader& = Shader());
  void popFrameBuffer();
  void popShader();
  void setCamera( const Camera& iCam, bool iAnimate = true, int iDuration = 1000 );
  void setCameraMode( Camera::Mode iMode );
  void setCameraOrientation( Camera::Orientation iO );
  
//signals:
//    void clicked();

protected:
	virtual void drawSceneForPicking() const {};  
  virtual void initializeGL();
  virtual void keyPressEvent(QKeyEvent*);
  virtual void mouseDoubleClickEvent( QMouseEvent* e );
  virtual void mouseMoveEvent( QMouseEvent* e );
  virtual void mousePressEvent( QMouseEvent* e );
  virtual void mouseReleaseEvent( QMouseEvent* e );
  virtual void wheelEvent ( QWheelEvent* e );
  QSize minimumSizeHint() const;
  virtual void paintGL();
  vector<unsigned int> pick(int, int, int = 1,int= 1 );
  void resizeGL(int iWidth, int iHeight);
  QSize sizeHint() const;
  virtual void showFps();
  virtual void timerEvent( QTimerEvent* ipE );

  Camera mCam;
  Camera mOldCam; //used to interpolate camera during animation
  Camera mNewCam; //used to interpolate camera during animation
  
private:  
  QTime mAnimationTimer;
  int mAnimationTimerId;
  int mAnimationDuration;
  double mFps;
  int mFpsFrameCount;
  QTime mFpsTimer;
  bool mShowFps;
  
  bool mMousePressed;
  int mMousePosX;
  int mMousePosY;
  
  std::vector<FrameBufferObject> mFrameBuffers;
  std::vector<Shader> mShaders;
};

} //treeD
} //realisim

#endif //Realisim_Widget3d_hh