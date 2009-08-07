/******************************************************************************
* Author: Pierre-Olivier Beaudoin                                                                    
* File name: RePluginMgr
* Description: Basic QT OpenGL canvas
*
*
******************************************************************************/

#include "MathUtils.h"
#include <QMouseEvent>
#include "Widget3d.h"

using namespace realisim;
using namespace realisim::treeD;
using namespace std;

namespace
{
  const int kCameraAnimationTime = 1000; //ms
  const int kFramesToComputeFps = 10;
}

//-----------------------------------------------------------------------------
Widget3d::Widget3d( QWidget* ipParent /*= 0*/,
                    const QGLWidget* shareWidget /*= 0*/,
                    Qt::WindowFlags  iFlags /*= 0*/ )
: QGLWidget( ipParent, shareWidget, iFlags),
mCam(),
mOldCam(),
mNewCam(),
mAnimationTimer(),
mAnimationTimerId(),
mDefaultHandler( mCam ),
mFps(0.0),
mFpsFrameCount(0),
mFpsTimer(),
mpInputHandler( 0 ),
mShowFps(true)
{
  setInputHandler( mDefaultHandler );
  //on s'assure que opneGL est initialisé pour ce context
  glInit();
}

//-----------------------------------------------------------------------------
Widget3d::~Widget3d()
{}

//-----------------------------------------------------------------------------
/* Voir Primitives.h pour la signification des flags
*/
void Widget3d::applyDisplayFlag(const Primitives& iP) const
{
  switch (iP.getPositionFlag()) 
  {
    case Primitives::pViewport:
    {
      GLdouble modelView[16];
      glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
      
      GLdouble projection[16];
      glGetDoublev(GL_PROJECTION_MATRIX, projection);
      
      GLint viewport[4];
      glGetIntegerv(GL_VIEWPORT, viewport);
      
      // get 3D coordinates based on window coordinates
      double x,y,z;
      gluUnProject(iP.getPosition().getX(),
        getCamera().getWindowInfo().getHeight() - iP.getPosition().getY(),
        0.01,
        modelView, projection, viewport,
        &x, &y, &z);
    
      glTranslated(x, y, z);
    }
      break;
    case Primitives::pScene:
    {
      glTranslated(iP.getPosition().getX(),
        iP.getPosition().getY(),
        iP.getPosition().getZ());
    }
    default:
      break;
  }
  
  switch (iP.getZoomFlag()) 
  {
    case Primitives::zViewport:
      {
        double perspectiveCompensation = 1.0;
        
//!!!Ca ne fonctionne pas et c'est peut etre pas utile        
//        if (getCamera().getMode() == Camera::PERSPECTIVE)
//        {
//          double winX, winY, winZ;
//          double winX2, winY2, winZ2;
//          GLdouble modelView[16];
//          glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
//          
//          GLdouble projection[16];
//          glGetDoublev(GL_PROJECTION_MATRIX, projection);
//          
//          GLint viewport[4];
//          glGetIntegerv(GL_VIEWPORT, viewport);
//          
//          gluProject(iP.getPosition().getX(),
//                     iP.getPosition().getY(),
//                     iP.getPosition().getZ(),
//                     modelView,
//                     projection,
//                     viewport,
//                     &winX, &winY, &winZ);
//          
//          Vector3d v(Point3d(0.0), iP.getPosition());
//          v += getCamera().getLat();
//          gluProject(v.getX(),
//                     v.getY(),
//                     v.getZ(),
//                     modelView,
//                     projection,
//                     viewport,
//                     &winX2, &winY2, &winZ2);
//          
//          Vector3d v2(winX2 - winX, winY2 - winY, winZ2 - winZ);
//          perspectiveCompensation =  getCamera().getPixelPerGLUnit() / v2.norm();
//          cout<<"persp comp: "<<perspectiveCompensation<<endl;
//        }        
        
        double z = 1 / getCamera().getPixelPerGLUnit() * perspectiveCompensation;
        glScaled(z, z, z);
      }
      break;
    case Primitives::zScene:
    default:
      break;
  }
}

//-----------------------------------------------------------------------------
void
Widget3d::initializeGL()
{
    //useful for lights 
    GLfloat shininess[] = {80.0};
    GLfloat position[]  = {50.0, 50.0, 50.0, 1.0};
    GLfloat ambiant[]   = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat diffuse[]   = {0.5, 0.5, 0.5, 1.0};
    GLfloat specular[]  = {1.0, 1.0, 1.0, 1.0};

    GLfloat mat_ambiant[] = {0.6f, 0.6f, 0.6f, 1.0f};
    GLfloat mat_diffuse[] = {0.6f, 0.6f, 0.6f, 1.0f};
    GLfloat mat_specular[]  = {0.5f, 0.5f, 0.5f, 1.0f};

    // Let OpenGL clear background to Grey
    glClearColor(125/255.0f, 125/255.0f, 125/255.0f, 0.0);

    glShadeModel(GL_SMOOTH);

    //define material props
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambiant);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    //init lights
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambiant);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);  
}

//-----------------------------------------------------------------------------
QSize
Widget3d::minimumSizeHint() const
{
  return QSize(50, 50);
}

//-----------------------------------------------------------------------------
void Widget3d::mouseDoubleClickEvent( QMouseEvent* e )
{
  makeCurrent();
  mpInputHandler->mouseDoubleClickEvent( e );
}

//-----------------------------------------------------------------------------
void Widget3d::mouseMoveEvent(QMouseEvent *e)
{
  makeCurrent();
  mpInputHandler->mouseMoveEvent( e );
  update();
}

//-----------------------------------------------------------------------------
void Widget3d::mousePressEvent(QMouseEvent *e)
{
  makeCurrent();
  mpInputHandler->mousePressEvent( e );
}

//-----------------------------------------------------------------------------
void Widget3d::mouseReleaseEvent(QMouseEvent *e)
{
  makeCurrent();
  mpInputHandler->mouseReleaseEvent( e );
}

//-----------------------------------------------------------------------------
void
Widget3d::paintGL()
{
  makeCurrent();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  
  //affiche le nombre de frame par seconde
  if(mShowFps)
  {
    if(mFpsFrameCount >= kFramesToComputeFps)
    {
      mFps = mFpsFrameCount / (double)mFpsTimer.elapsed() * 1000.0;
      mFpsTimer = QTime::currentTime();
      mFpsFrameCount = 0;
    }
    renderText(5, 15, QString("fps: ") + QString::number(mFps, 'f', 2) );
    ++mFpsFrameCount;
  }
  
  //On place la caméra en coordonnée absolue.
  Point3d absolutePos = mCam.getPos() * mCam.getTransformation();
  absolutePos += mCam.getTransformation().getTranslation();
  Point3d absoluteLook = mCam.getLook() * mCam.getTransformation();
  absoluteLook += mCam.getTransformation().getTranslation();
  Vector3d absoluteUp = mCam.getUp() * mCam.getTransformation();
  
  gluLookAt( absolutePos.getX(),
             absolutePos.getY(), 
             absolutePos.getZ(),
             absoluteLook.getX(),
             absoluteLook.getY(),
             absoluteLook.getZ(),
             absoluteUp.getX(),
             absoluteUp.getY(),
             absoluteUp.getZ() );
             
  //replacer les lumieres
  GLfloat position[]  = {50.0, 50.0, 50.0, 1.0};
  glLightfv(GL_LIGHT0, GL_POSITION, position);
             
            
  //Ici on dessine les objets graphiques de la scene privée du widget.
  //drawPrivateScene();
}

//-----------------------------------------------------------------------------
void
Widget3d::resizeGL(int iWidth, int iHeight)
{
  mCam.projectionGL(iWidth, iHeight);
  update();
}

//-----------------------------------------------------------------------------
void
Widget3d::setCamera( const Camera& iCam, bool iAnimate /*= true*/ )
{
  mOldCam = mCam;
  mNewCam = iCam;
  
  if( iAnimate )
  {
    //we use a QTime to track animation time
    mAnimationTimer.start();
    //start a timer that will timeout as quick as possible which will trigger
    //the overloaded method timerEvent( QTimerEvent* )
    mAnimationTimerId = startTimer( 0 );
  }
  else
  {
    mCam = mNewCam;
    update();
  }
}

//-----------------------------------------------------------------------------
void Widget3d::setCameraMode( Camera::Mode iMode )
{
  mCam.setMode( iMode );
  update();
}

//-----------------------------------------------------------------------------
void Widget3d::setCameraOrientation( Camera::Orientation iO )
{
  mCam.setOrientation( iO );
  update();
}

//-----------------------------------------------------------------------------
void Widget3d::setInputHandler( InputHandler& iHandler )
{
  mpInputHandler = &iHandler;
}

//-----------------------------------------------------------------------------
QSize
Widget3d::sizeHint() const
{
  return QSize(200, 200);
}

//-----------------------------------------------------------------------------
//Animate the camera in the timer event function
void Widget3d::timerEvent( QTimerEvent* ipE )
{
  if ( ipE->timerId() == mAnimationTimerId )
  {
    double animationTime = min( kCameraAnimationTime, mAnimationTimer.elapsed() );
    double t = animationTime / (double)kCameraAnimationTime;
    t = inversePower(t, 3);
    
    Quat4d q1( mOldCam.getTransformation() );
    Quat4d q2( mNewCam.getTransformation() );
    //on compare avec le conjugate pour prendre le plus petit angle
    if( (-q2-q1).getLength() < (q2-q1).getLength() )
    {
      q2 = -q2;
    }
    
    q2 = q1*( 1 - t ) + q2*t;
    Matrix4d iterationMatrix = q2.getUnitRotationMatrix();
    
    //trouver la translation totale a effectuer
    iterationMatrix.setTranslation( mOldCam.getTransformation().getTranslation()*( 1 - t ) + 
      mNewCam.getTransformation().getTranslation()*( t ) );
    
    mCam.setTransformation(iterationMatrix,false);
    
    update();
    
    if ( animationTime >= kCameraAnimationTime )
    {
      killTimer( mAnimationTimerId );
    }
  }
  else
  {
    QWidget::timerEvent( ipE );
  }
}

//-----------------------------------------------------------------------------
void Widget3d::wheelEvent ( QWheelEvent * event )
{
  makeCurrent();
  mpInputHandler->wheelEvent(event);
  update();
}