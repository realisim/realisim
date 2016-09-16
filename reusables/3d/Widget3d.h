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
#include <QtOpenGL/QGLWidget>

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

	enum controlType{ ctNone, ctPan, ctRotateAround, ctFree };

  virtual const Camera& getCamera() const;
  virtual double getCameraSpeed() const { return mCameraSpeed; }
  virtual controlType getControlType() const;
  virtual void pushFrameBuffer(const FrameBufferObject& = FrameBufferObject());
  virtual void pushShader(const Shader& = Shader());
  virtual void popFrameBuffer();
  virtual void popShader();
  virtual void setCamera( const Camera& iCam, bool iAnimate = true, int iDuration = 1000 );
  virtual void setCameraSpeed( double iS ) { mCameraSpeed = iS; }
  virtual void setControlType( controlType );

protected:
	virtual void beginFrame();
  virtual void draw() {;}
	virtual void drawSceneForPicking() {;}
  virtual void initializeGL();
  virtual bool isAnimatingCamera() const;
  virtual bool isKeyPressed( int ) const;
  virtual void keyPressEvent(QKeyEvent*);
  virtual void keyReleaseEvent(QKeyEvent*);
  virtual void mouseDoubleClickEvent( QMouseEvent* e );
  virtual void mouseMoveEvent( QMouseEvent* e );
  virtual void mousePressEvent( QMouseEvent* e );
  virtual void mouseReleaseEvent( QMouseEvent* e );
  virtual void wheelEvent ( QWheelEvent* e );
  QSize minimumSizeHint() const;
  virtual void paintGL();
  vector<unsigned int> pick(int, int, int = 1,int= 1 );
  virtual void resizeGL(int iWidth, int iHeight);
  virtual QSize sizeHint() const;
  virtual void showFps();
  virtual void timerEvent( QTimerEvent* ipE );

  Camera mCam;
  Camera mOldCam; //used to interpolate camera during animation
  Camera mNewCam; //used to interpolate camera during animation
  controlType mControlType;
   
  QTime mAnimationTimer;
  int mAnimationTimerId;
  int mCameraControlTimerId;
  int mAnimationDuration;
  double mFps;
  int mFpsFrameCount;
  QTime mFpsTimer;
  bool mMousePressed;
  int mMousePosX;
  int mMousePosY;
  double mCameraSpeed;
  std::map< int, bool > mKeys;
  std::vector<FrameBufferObject> mFrameBuffers;
  std::vector<Shader> mShaders;
};

} //treeD
} //realisim

#endif //Realisim_Widget3d_hh