
#include "coreLibrary, glViewer.h"
#include <math.h>
#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimerEvent>
#include <QWheelEvent>

using namespace resonant;
using namespace coreLibrary;

//------------------------------------------------------------------------------
glViewer::mouse::mouse() :
   mMouseState( msIdle ),
   mX( 0.f ),
   mY( 0.f )
{}

//------------------------------------------------------------------------------
//--- glCamera
//------------------------------------------------------------------------------
glViewer::glViewer( QWidget* ipParent /*=0*/, const QGLWidget* ipShared /*=0*/ ) : 
   QGLWidget( ipParent, ipShared ),
      mCurrentCamera(),
      mPreviousCamera(),
      mNextCamera(),
      mCameraControl( ccFreeFlight ),
      mAnimationTime( 0 ),
      mAnimationTimer(),
      mAnimationTimerId( -1 ),
      mInputTimerId( -1 ),
      mMouse(),
      mCameraMoveIncrement(1.0)
{
   setFocusPolicy( Qt::StrongFocus );
   setMouseTracking( true );
}

//------------------------------------------------------------------------------
glViewer::~glViewer()
{}

//-----------------------------------------------------------------------------
void glViewer::beginFrame()
{
   makeCurrent();
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   getCamera().applyModelViewProjection();
}

//-----------------------------------------------------------------------------
void glViewer::handleKeyboardInput()
{
   switch( mCameraControl )
   {
   case ccNone:
   case ccRotation:
   case ccPan:
      break;
   case ccFreeFlight:
      {
         glCamera c = getCamera();
         vector3 translation;
         if( mKeyboard.find( Qt::Key_D ) != mKeyboard.end()
          && mKeyboard[Qt::Key_D] )
         { translation += vector3( 1, 0, 0 ); }
         if( mKeyboard.find( Qt::Key_A ) != mKeyboard.end()
          && mKeyboard[Qt::Key_A] )
         { translation += vector3( -1, 0, 0 ); }
         if( mKeyboard.find( Qt::Key_W ) != mKeyboard.end()
          && mKeyboard[Qt::Key_W] )
         { translation += vector3( 0, 0, -1 ); }
         if( mKeyboard.find( Qt::Key_S ) != mKeyboard.end()
          && mKeyboard[Qt::Key_S] )
         { translation += vector3( 0, 0, 1 ); }
         if( mKeyboard.find( Qt::Key_Q ) != mKeyboard.end()
            && mKeyboard[Qt::Key_Q] )
         { translation += vector3( 0, 1, 0 ); }
         if( mKeyboard.find( Qt::Key_E ) != mKeyboard.end()
            && mKeyboard[Qt::Key_E] )
         { translation += vector3( 0, -1, 0 ); }

         if( translation.norm() )
         {
            translation = translation.normalize() * getCameraMoveIncrement();
            c.translate( c.cameraToWorld( translation ) );
            setCamera( c );
         }
      }
      break;
   }
   //update();
}

//-----------------------------------------------------------------------------
void glViewer::draw()
{
   //replacer la lumiere par défaut.
   GLfloat position[]  = {50, 50, 50, 1.0};
   //glLightfv(GL_LIGHT0, GL_POSITION, position);
}

//------------------------------------------------------------------------------
void glViewer::initializeGL()
{
   QGLWidget::initializeGL();

   GLenum err = glewInit();
   if (GLEW_OK != err)
   {
      /* Problem: glewInit failed, something is seriously wrong. */
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
   }

   ////print openGL info
   //OpenGLInfo i;
   //i.print();

   //definition lumières par défaut 
   GLfloat shininess[] = {80.0};
   GLfloat position[]  = {50.0, 50.0, 50.0, 1.0};
   GLfloat ambiant[]   = {0.2f, 0.2f, 0.2f, 1.0f};
   GLfloat diffuse[]   = {0.5, 0.5, 0.5, 1.0};
   GLfloat specular[]  = {1.0, 1.0, 1.0, 1.0};

   //définition des matériaux par défaut
   GLfloat mat_ambiant[] = {0.6f, 0.6f, 0.6f, 1.0f};
   GLfloat mat_diffuse[] = {0.6f, 0.6f, 0.6f, 1.0f};
   GLfloat mat_specular[]  = {0.5f, 0.5f, 0.5f, 1.0f};

   //initialisation des matériaux par défaut
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambiant);
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

   //initialisation lumières par défaut
   glLightfv(GL_LIGHT0, GL_POSITION, position);
   glLightfv(GL_LIGHT0, GL_AMBIENT, ambiant);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

   // background gris
   glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

   glShadeModel(GL_FLAT);

   glEnable(GL_COLOR_MATERIAL);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glDisable(GL_CULL_FACE);
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_TEXTURE_3D);
}

//------------------------------------------------------------------------------
glCamera glViewer::getCamera() const
{ return mCurrentCamera; }

//------------------------------------------------------------------------------
double glViewer::getCameraMoveIncrement() const
{ return mCameraMoveIncrement; }

//------------------------------------------------------------------------------
void glViewer::keyPressEvent(QKeyEvent* e)
{
   if(!e->isAutoRepeat())
   {
      makeCurrent();   
      mKeyboard[ e->key() ] = true;
      if( mInputTimerId == -1 )
      {
         mInputTimerId = startTimer( 15 );
      }
   }   
}

//------------------------------------------------------------------------------
void glViewer::keyReleaseEvent(QKeyEvent* e)
{
   if( !e->isAutoRepeat() )
   {
      bool pressed = false;
      makeCurrent();
      mKeyboard[ e->key() ] = false;
      foreach( const auto &it, mKeyboard )
      { pressed |= it.second; }

      if( !pressed )
      {
         killTimer( mInputTimerId );
         mInputTimerId = -1;
      }
      QGLWidget::keyReleaseEvent( e );
   }   
}

//------------------------------------------------------------------------------
void glViewer::mouseDoubleClickEvent( QMouseEvent* e )
{
   makeCurrent();
   QGLWidget::mouseDoubleClickEvent( e );
}

//------------------------------------------------------------------------------
void glViewer::mouseMoveEvent( QMouseEvent* e )
{
   makeCurrent();
   if( mMouse.mMouseState == mouse::msPressed )
   {
      glCamera c = getCamera();
      point2 a( mMouse.mX, height() - mMouse.mY ), b( e->x(), height() - e->y() );
      vector2 m = b - a;
      m.setDx( m.dx() * 90 / width() ); m.setDy( m.dy() * 90 / height() );
      switch( mCameraControl )
      {
      case ccNone:
         break;
      case ccFreeFlight:
         {
            /* La camera rote autours de l'axe lateral de la camera et autours
               de l'axe z du monde selon sa position. */
            c.rotate( m.dy() * -3.14156 / 180,
                                c.cameraToWorld( vector3( 1, 0, 0 ) ),
                                c.getPosition() );
            c.rotate( m.dx() * 3.14156 / 180,
               vector3( 0, 0, 1 ), c.getPosition() );
         }
         break;
      case ccRotation:
         {
            /* La camera rote autours de l'axe lateral de la camera et autours
               de l'axe z du monde selon la position regardée. */
            c.rotate( m.dy() * 3.14156 / 180,
                                c.cameraToWorld( vector3( 1, 0, 0 ) ),
                                c.getLook() );
            c.rotate( m.dx() * -3.14156 / 180,
               vector3( 0, 0, 1 ), c.getLook() );
         }
         break;
      case ccPan:
         {
            vector3 v = c.screenToWorld( b - a );
            c.translate( - v );
         }
         break;
      }
      setCamera( c );
   }
   mMouse.mX = e->x();
   mMouse.mY = e->y();
   QGLWidget::mouseMoveEvent( e );
   update();
}
//------------------------------------------------------------------------------
void glViewer::mousePressEvent( QMouseEvent* e )
{
   makeCurrent();
   mMouse.mMouseState = mouse::msPressed;
   QGLWidget::mousePressEvent( e );
}
//------------------------------------------------------------------------------
void glViewer::mouseReleaseEvent( QMouseEvent* e )
{
   makeCurrent();
   mMouse.mMouseState = mouse::msIdle;
   QGLWidget::mouseReleaseEvent( e );
}
//------------------------------------------------------------------------------
void glViewer::wheelEvent ( QWheelEvent* e )
{
   makeCurrent();
   glCamera c = getCamera();
   switch( mCameraControl )
   {
   case ccNone:
   case ccFreeFlight:
      break;
   case ccPan:
   case ccRotation:
      {
         /* zoom exponentiel */
         double logZoom = log10( c.getZoomFactor() );
         if( e->delta() < 1.0f ) logZoom -= log10(1.1);
         else logZoom += log10(1.1);

         if( c.getProjectionType() == glCamera::ptOrthogonal )
         { c.setZoom( pow( 10.0, logZoom ), index2( e->x(), height() - e->y() ) ); }
         else
         { c.setZoom( pow( 10.0, logZoom ) ); }
         setCamera( c );
      }
   }
   QGLWidget::wheelEvent( e );
   update();
}
//------------------------------------------------------------------------------
void glViewer::paintGL()
{
   beginFrame();
   draw();
}
//------------------------------------------------------------------------------
void glViewer::resizeGL(int w, int h)
{
   QGLWidget::resizeGL(w, h);
   mCurrentCamera.setViewportSize(w, h);
   update();
}
//------------------------------------------------------------------------------
void glViewer::setCamera(glCamera nextCamera, int animationTime /*=0*/)
{
   if( animationTime > 0 )
   {
      mAnimationTime = animationTime;
      mNextCamera = nextCamera;
      mPreviousCamera = getCamera();
      if( mAnimationTimerId == -1 )
      { mAnimationTimerId = startTimer( 15 ); }
      mAnimationTimer.start();
   }
   else
   {
      if( mAnimationTimerId != -1 )
      {
         killTimer( mAnimationTimerId );
         mAnimationTimerId = -1;
      }
      mCurrentCamera = nextCamera;
   }
   update();
}
//------------------------------------------------------------------------------
void glViewer::setCameraControl( cameraControl cameraControl )
{ mCameraControl = cameraControl; }
//------------------------------------------------------------------------------
void glViewer::setCameraMoveIncrement( double i )
{ mCameraMoveIncrement = i; }
//------------------------------------------------------------------------------
QSize glViewer::sizeHint() const
{
   constPolish();
   int w = 100, h = 100;
   return QSize(100, 100).expandedTo(QApplication::globalStrut());
}
//------------------------------------------------------------------------------
void glViewer::timerEvent( QTimerEvent* e )
{
   if( e->timerId() == mInputTimerId )
   { handleKeyboardInput(); }

   if( e->timerId() == mAnimationTimerId )
   {
      double t = mAnimationTimer.elapsed() / (double) mAnimationTime;
      if( t >= 1.0 )
      {
         killTimer( mAnimationTimerId );
         mAnimationTimerId = -1;
         mCurrentCamera = mNextCamera;
      }
      else
      {
         mCurrentCamera = glCamera::interpolate( t, mPreviousCamera, mNextCamera );
      }
      update();
   }
}