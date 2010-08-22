/******************************************************************************
* Author: Pierre-Olivier Beaudoin                                                                    
* File name: Widget3d
* Description: Basic QT OpenGL canvas
*
*
******************************************************************************/

#include "math/MathUtils.h"
#include <QMouseEvent>
#include "3d/OpenGLInfo.h"
#include "3d/Shader.h"
#include "3d/Widget3d.h"

using namespace realisim;
using namespace realisim::treeD;
using namespace std;

namespace
{
  const int kCameraAnimationTime = 1000; //ms
  const int kFramesToComputeFps = 10;
  
  const double kMaxZoom = 1/128.0;
  const double kMinZoom = 128;
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
mFps(0.0),
mFpsFrameCount(0),
mFpsTimer(),
mShowFps(true),
mMousePressed( false ),
mMousePosX( 0 ),
mMousePosY( 0 ),
mShaders()
{}

//-----------------------------------------------------------------------------
Widget3d::~Widget3d()
{}

//-----------------------------------------------------------------------------
void
Widget3d::initializeGL()
{
  QGLWidget::initializeGL();
  
  //print openGL info
  OpenGLInfo i;
  i.print();
  
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
  //glClearColor(125/255.0f, 125/255.0f, 125/255.0f, 0.0);
  glClearColor(0.3, 0.3, 0.3, 0.0);

  glShadeModel(GL_SMOOTH);

  //define material props
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambiant);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

  //init lights
  glLightfv(GL_LIGHT0, GL_POSITION, position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambiant);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_TEXTURE_3D);
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
}

//-----------------------------------------------------------------------------
void Widget3d::mouseMoveEvent(QMouseEvent *e)
{
  makeCurrent();

  if( mMousePressed )
  {
    int deltaX = e->x() - mMousePosX;
    int deltaY = e->y() - mMousePosY;
    
    //on met un - devant le deltaY parce que le systeme de fenetrage QT
    //a laxe Y vers le bas et notre systeme de fenetrage GL a l'axe y vers le
    //haut.
    Vector3d delta = getCamera().pixelDeltaToGLDelta( deltaX, -deltaY );
    
    //On met un - devant le delta pour donner l'impression qu'on ne 
    //déplace pas la camera, mais le model. Si on ne mettait pas de -,
    //la caméra se déplacerait en suivant la souris et ce qu'on voit a l'écran
    //s'en irait dans le sens contraire de la souris. En mettant le - on
    //donne l'impression de déplacer le contenu de l'écran.
    getCamera().move( -delta );
  }
  
  mMousePosX = e->x();
  mMousePosY = e->y();

  update();
}

//-----------------------------------------------------------------------------
void Widget3d::mousePressEvent(QMouseEvent *e)
{
  makeCurrent();

  mMousePressed = true;
  mMousePosX = e->x();
  mMousePosY = e->y();
}

//-----------------------------------------------------------------------------
void Widget3d::mouseReleaseEvent(QMouseEvent *e)
{
  makeCurrent();

  mMousePressed = false;
}

//-----------------------------------------------------------------------------
void
Widget3d::paintGL()
{
  makeCurrent();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  
  //On place la caméra en coordonnée absolue.
  Point3d absolutePos = getCamera().getPos() * getCamera().getTransformationToGlobal();
  Point3d absoluteLook = getCamera().getLook() * getCamera().getTransformationToGlobal();
  Vector3d absoluteUp = getCamera().getUp() * getCamera().getTransformationToGlobal();
  
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
}

//-----------------------------------------------------------------------------
void Widget3d::pushShader(const Shader& iS)
{
  mShaders.push_back(iS);
  if(iS.isValid())
  {
    glUseProgram(iS.getProgramId());
  }
}

//-----------------------------------------------------------------------------
void Widget3d::popShader()
{
  mShaders.pop_back();
  GLint programId = 0;
  if(!mShaders.empty())
    programId = mShaders.back().getProgramId();
  glUseProgram(programId);
}

//-----------------------------------------------------------------------------
void
Widget3d::resizeGL(int iWidth, int iHeight)
{
  QGLWidget::resizeGL(iWidth, iHeight);
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  getCamera().projectionGL(iWidth, iHeight);
  glMatrixMode( GL_MODELVIEW );
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
    /*L'animation de la camera va interpoler la transformation entre la
      la vielle camera (mOldCam) et la nouvelle camera (mNewCame). Cette
      matrice interpolé sera appliqué a la camera courante (mCam). Cette
      méthode permet a l'usagé de modifié la position relative de la caméra 
      durant l'animation (parce que la position globale de la camera est
      mCam.getPos() * mCam.getTransformationToGlobal() ). On ne veut pas
      interpolé la position, le point visé et le up de la caméra locale parce
      que ca empêcherait l'utilisateur de les modifier durant l'animation. Par
      contre, on veut quand même permettre de modifier la position locale de la
      caméra, via la méthode Camera::set(Point3d, Poin3d, Vector3d), c'est 
      pourquoi on applique la position locale de la nouvelle caméra à la caméra
      courante. 
      
      Note: Pour donner l'effet d'interpoler le vecteur up (donner une 
      impression de tanguage) ou d'interpoler sur le point visé (donner 
      l'impression de déplace la caméra), il est très facile de le faire en
      modifiant la transformation locale ou globale.*/
  	mCam.set(mNewCam.getPos(), mNewCam.getLook(), mNewCam.getUp());
  
    //we use a QTime to track animation time
    mAnimationTimer.start();
    //start a timer that will timeout as quick as possible which will trigger
    //the overloaded method timerEvent( QTimerEvent* )
    killTimer(mAnimationTimerId);
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
  getCamera().setMode( iMode );
  update();
}

//-----------------------------------------------------------------------------
void Widget3d::setCameraOrientation( Camera::Orientation iO )
{
  getCamera().setOrientation( iO );
  update();
}

//-----------------------------------------------------------------------------
QSize
Widget3d::sizeHint() const
{
  return QSize(200, 200);
}

//-----------------------------------------------------------------------------
void Widget3d::showFps()
{
  //affiche le nombre de frame par seconde
  if(mShowFps)
  {
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    if(mFpsFrameCount >= kFramesToComputeFps)
    {
      mFps = mFpsFrameCount / (double)mFpsTimer.elapsed() * 1000.0;
      mFpsTimer = QTime::currentTime();
      mFpsFrameCount = 0;
    }
    renderText(5, 15, QString("fps: ") + QString::number(mFps, 'f', 2) );
    ++mFpsFrameCount;
    glPopAttrib();
  }
}

//-----------------------------------------------------------------------------
/*Animate the camera in the timer event function
  Note: La transformation de la camera est le seul parametre a etre animee. 
  Changer la position (ou le vecteur lateral ou up) ne fera rien lors de 
  l'animation.*/
void Widget3d::timerEvent( QTimerEvent* ipE )
{
  if ( ipE->timerId() == mAnimationTimerId )
  {
    double animationTime = min( kCameraAnimationTime, mAnimationTimer.elapsed() );
    double t = animationTime / (double)kCameraAnimationTime;
    t = inversePower(t, 3);
    
    Quat4d q1( mOldCam.getTransformationToLocal() );
    Quat4d q2( mNewCam.getTransformationToLocal() );
    //on compare avec les longueurs pour prendre le plus petit angle
    if( (-q2-q1).getLength() < (q2-q1).getLength() )
    {
      q2 = -q2;
    }
    
    q2 = q1*( 1 - t ) + q2*t;
    Matrix4d iterationMatrix = q2.getUnitRotationMatrix();
    
    //trouver la translation totale a effectuer
    iterationMatrix.setTranslation( mOldCam.getTransformationToLocal().getTranslation()*( 1 - t ) + 
      mNewCam.getTransformationToLocal().getTranslation()*( t ) );
    
    //getCamera().set(p2, l2, u2);
    getCamera().setTransformationToLocal(iterationMatrix);
    
    if ( animationTime >= kCameraAnimationTime )
      killTimer( mAnimationTimerId );
    update();
  }
  else
  {
    QWidget::timerEvent( ipE );
  }
}

//-----------------------------------------------------------------------------
void Widget3d::wheelEvent(QWheelEvent* ipE)
{
  makeCurrent();

  if(getCamera().getMode() == Camera::ORTHOGONAL)
  {
    double zoom = 1 / 1.15;
    if(ipE->delta() < 0)
      zoom = 1.15;
    double finalZoom = getCamera().getZoom() * zoom;
    if(finalZoom >= kMaxZoom && finalZoom <= kMinZoom)
      getCamera().setZoom(finalZoom);
  }
  else
  {
    int wheelDir = ipE->delta() > 0 ? 1.0 : -1.0;
    Vector3d lookDirection(getCamera().getPos(), getCamera().getLook());
    Point3d p = getCamera().getPos() + lookDirection * 0.2 * wheelDir;
    Camera c = getCamera();
    c.setPos(p);
    setCamera(c, false);
  }


  update();
}
