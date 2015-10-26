
//--- glew.h doit être inclus avant qgl.h (a cause de gl.h)
#ifdef WIN32
   #ifndef __glew_h__
      #include <GL/glew.h> //nécessaire pour l'utilisation de GL_BGRA
   #endif   
#endif
//#include "qgl.h"
#include <QGLWidget>
#include <QTime>
#include <map>
#include "coreLibrary, glCamera.h"

namespace resonant { namespace coreLibrary { class glViewer; } }

namespace resonant
{
namespace coreLibrary
{

class glViewer : public QGLWidget
{
   Q_OBJECT
public:
   glViewer( QWidget* ipParent = 0, const QGLWidget* shareWidget = 0 );
   virtual ~glViewer();

   enum cameraControl{ ccNone, ccFreeFlight, ccRotation, ccPan };

   struct mouse
   {
      enum mouseState{ msIdle, msPressed };

      mouse();

      mouseState mMouseState;
      double mX;
      double mY;
   }; 

   virtual void draw();
   virtual glCamera getCamera() const;
   virtual double getCameraMoveIncrement() const;
   virtual void setCamera(glCamera, int = 0);
   virtual void setCameraControl( cameraControl );
   virtual void setCameraMoveIncrement( double );

protected:
   virtual void beginFrame();
   virtual void handleKeyboardInput();
   virtual void initializeGL();
   virtual void keyPressEvent(QKeyEvent*);
   virtual void keyReleaseEvent(QKeyEvent*);
   virtual void mouseDoubleClickEvent(QMouseEvent*);
   virtual void mouseMoveEvent(QMouseEvent*);
   virtual void mousePressEvent(QMouseEvent*);
   virtual void mouseReleaseEvent(QMouseEvent*);
   virtual void wheelEvent (QWheelEvent*);
   virtual void paintGL();
   virtual void resizeGL(int, int);
   virtual QSize sizeHint() const;
   virtual void timerEvent(QTimerEvent*);

   glCamera mCurrentCamera;
   glCamera mPreviousCamera;
   glCamera mNextCamera;
   cameraControl mCameraControl;
   int mAnimationTime;
   QTime mAnimationTimer;
   int mAnimationTimerId;
   int mInputTimerId;
   mouse mMouse;
   double mCameraMoveIncrement;

   std::map< int, bool > mKeyboard;
};

}
}
