/******************************************************************************
* Author: Pierre-Olivier Beaudoin                                                                    
* File name: Widget3d
* Description: Opengl Widget  to display 3d
*
*
******************************************************************************/

#ifndef Realisim_Widget3d_hh
#define Realisim_Widget3d_hh

#include "Camera.h"

#include <map>

namespace Realisim
{ 
  class Widget3d;
  
  namespace Primitive3d
  {
    class Primitive3dBase;
  }
}

#include <QGLWidget>

class Realisim::Widget3d : public QGLWidget
{
    Q_OBJECT
public:
  Widget3d( QWidget* ipParent = 0,
            const QGLWidget* shareWidget = 0,
            Qt::WindowFlags iFlags = 0 );

  virtual ~Widget3d();

  const Camera& getCamera() const { return mCam; }

  void setCamera( const Camera& iCam );
  void setCameraMode( Camera::Mode iMode );
  
signals:
    void clicked();

protected:

    void initializeGL();
    virtual void paintGL();
    void resizeGL(int iWidth, int iHeight);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

  QSize minimumSizeHint() const;
  QSize sizeHint() const;
  
private:
  Camera mCam;
  
  typedef std::map<int, Primitive3d::Primitive3dBase*> IdToPrimitiveMap;
  IdToPrimitiveMap mIdToPrimitiveMap;
};

#endif //Realisim_Widget3d_hh