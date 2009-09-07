/******************************************************************************
* Author: Pierre-Olivier Beaudoin                                                                    
* File name: Widget3d
* Description: Opengl Widget to display 3d
*
******************************************************************************/

#ifndef Realisim_Widget3d_hh
#define Realisim_Widget3d_hh

#include "Camera.h"
#include "Primitives.h"
#include <map>
#include <QTime>
#include <QGLWidget>

namespace realisim
{
namespace treeD
{

class Widget3d : public QGLWidget
{
    Q_OBJECT
public:
  Widget3d( QWidget* ipParent = 0,
            const QGLWidget* shareWidget = 0,
            Qt::WindowFlags iFlags = 0 );
  virtual ~Widget3d() = 0;

  void applyDisplayFlag(const Primitives& iP) const;
  const Camera& getCamera() const { return mCam; }
  void setCamera( const Camera& iCam, bool iAnimate = true );
  void setCameraMode( Camera::Mode iMode );
  void setCameraOrientation( Camera::Orientation iO );
  
signals:
    void clicked();

protected:
  void initializeGL();
  virtual void paintGL();
  void resizeGL(int iWidth, int iHeight);
  virtual void mouseDoubleClickEvent( QMouseEvent* e );
  virtual void mouseMoveEvent( QMouseEvent* e );
  virtual void mousePressEvent( QMouseEvent* e );
  virtual void mouseReleaseEvent( QMouseEvent* e );
  virtual void wheelEvent ( QWheelEvent* e );
  QSize minimumSizeHint() const;
  QSize sizeHint() const;
  virtual void timerEvent( QTimerEvent* ipE );

  Camera mCam;
  Camera mOldCam; //used to interpolate camera during animation
  Camera mNewCam; //used to interpolate camera during animation
  
private:  
  QTime mAnimationTimer;
  int mAnimationTimerId;
  double mFps;
  int mFpsFrameCount;
  QTime mFpsTimer;
  bool mShowFps;
  
  bool mMousePressed;
  int mMousePosX;
  int mMousePosY;
};

} //treeD
} //realisim

#endif //Realisim_Widget3d_hh