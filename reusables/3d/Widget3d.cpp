/******************************************************************************
* Author: Pierre-Olivier Beaudoin                                                                    
* File name: Widget3d
* Description: Basic QT OpenGL canvas
*
*
******************************************************************************/

#include <cmath>
#include "math/MathUtils.h"
//#include <qcolor.h>
#include <QKeyEvent>
#include <QMouseEvent>
#include "3d/Utilities.h"
#include "3d/OpenGLInfo.h"
#include "3d/Widget3d.h"

using namespace realisim;
using namespace realisim::treeD;
using namespace std;

namespace
{
  const int kFramesToComputeFps = 50;
  
  const double kMaxZoom = 1/128.0;
  const double kMinZoom = 65536;
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
mAnimationDuration(0),
mFps(0.0),
mFpsFrameCount(0),
mFpsTimer(),
mMousePressed( false ),
mMousePosX( 0 ),
mMousePosY( 0 ),
mFrameBuffers(),
mShaders()
{}

//-----------------------------------------------------------------------------
Widget3d::~Widget3d()
{}
  
//-----------------------------------------------------------------------------
void Widget3d::initializeGL()
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
  glDisable(GL_CULL_FACE);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_TEXTURE_3D);
}

//-----------------------------------------------------------------------------
void Widget3d::keyPressEvent(QKeyEvent* ipE)
{
	//ipE->ignore();
  QGLWidget::keyPressEvent(ipE);
}

//-----------------------------------------------------------------------------
QSize Widget3d::minimumSizeHint() const
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
    mCam.move( -delta );
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
  getCamera().applyModelViewTransformation();
  
  //replacer les lumieres
  GLfloat position[]  = {50.0, 50.0, 50.0, 1.0};
  glLightfv(GL_LIGHT0, GL_POSITION, position);

#ifdef NDEBUG
  if(glGetError())
  {
    QString error("glError: ");
    error += QString::number(glGetError());
    qDebug(error.toStdString().c_str());
  }
#endif
}

//-----------------------------------------------------------------------------
/*voir http://www.lighthouse3d.com/opengl/picking/index.php?color1
  pour plus d'info
  Note: Cette méthode reçoit iX et iY dans le système de coordonnées
    de Qt (ix, iY par rapport au coin supérieur gauche. En ce moment,
    si le viewport openGL à une taille différente de la taille du
    widget, ça ne fonctionne pas, il faut fixed le code ici...*/
vector<unsigned int> Widget3d::pick(int iX, int iY, int iWidth /*= 1*/,
  int iHeight /*= 1*/ )
{
	using namespace utilities; //pour colorToId
  
  GLint viewport[4]; //x, y, width, height
  glGetIntegerv(GL_VIEWPORT,viewport);
  
  /*glReadpixel prend le coin inférieur gauche et la taille, donc
    on va s'assurer de lui passer le coin inférieur gauche de la boite.*/
  int x1,y1,x2,y2;
  //on definit les 4 coins de la fenetre de selection
  //x1 = iX; y1 = iY; x2 = x1 + iWidth; y2 = y1 + iHeight;
  //on trouve le coin inférieur gauche de la boite de selection
  x1 = min(iX, iX + iWidth);
  y1 = max(iY, iY + iHeight);
  //le coin superieur droit
  x2 = max(iX, iX + iWidth);
  y2 = min(iY, iY + iHeight);

  //on cap le coin inferieur gauche sur le viewport
  x1 = max(x1, viewport[0]);
  y1 = max(y1, viewport[1]);
  x1 = min(x1, viewport[0] + viewport[2]);
  y1 = min(y1, viewport[1] + viewport[3]);
  //on cap le coin superieur droit sur la taille du viewport.
  x2 = max(x2, viewport[0]);
  y2 = max(y2, viewport[1]);
	x2 = min(x2, viewport[0] + viewport[2]);
  y2 = min(y2, viewport[1] + viewport[3]);
  
	int absWidth = x2 - x1;
  int absHeight = y1 - y2;
  vector<unsigned int> hits;
	GLubyte pixels[absWidth * absHeight * 4];

  glPushAttrib(GL_COLOR_BUFFER_BIT | GL_POLYGON_BIT);
  //draw the scene in picking mode...
  /*On s'assure que le clear color est completement noir parce qu'il
    représentera le id 0 et n'interferera donc pas dans la sélection.*/
  glClearColor(1, 1, 1, 1);
  Widget3d::paintGL();
  drawSceneForPicking();
  
  glReadPixels(x1, viewport[3] - y1, absWidth, absHeight,
		GL_RGBA,GL_UNSIGNED_BYTE,(void *)pixels);
  for(int i = 0; i < absHeight; ++i)
    for(int j = 0; j < absWidth; ++j)    
      if(pixels[i*absWidth*4 + j*4] != 255 || pixels[i*absWidth*4 + j*4 + 1] != 255 || pixels[i*absWidth*4 + j*4 + 2] != 255 || pixels[i*absWidth*4 + j*4 + 3] != 255)
        hits.push_back(colorToId(QColor(pixels[i*absWidth*4 + j*4],pixels[i*absWidth*4 + j*4 + 1],pixels[i*absWidth*4 + j*4 + 2],pixels[i*absWidth*4 + j*4 + 3])));
  
  /*Quand la boite fait plus de 1x1, on fais une selection sur les back facing
    polygones aussi, ainsi permettant une selection qui passe au travers
    de la surface.*/
  if(absWidth > 1 || absHeight > 1 )
  {
  	glEnable(GL_CULL_FACE);
  	glCullFace(GL_FRONT);
    Widget3d::paintGL();
    drawSceneForPicking();
    
    glReadPixels(x1, viewport[3] - y1, absWidth, absHeight,
  		GL_RGBA,GL_UNSIGNED_BYTE,(void *)pixels);
    for(int i = 0; i < absHeight; ++i)
      for(int j = 0; j < absWidth; ++j)    
        if(pixels[i*absWidth*4 + j*4] != 255 || pixels[i*absWidth*4 + j*4 + 1] != 255 || pixels[i*absWidth*4 + j*4 + 2] != 255 || pixels[i*absWidth*4 + j*4 + 3] != 255)
          hits.push_back(colorToId(QColor(pixels[i*absWidth*4 + j*4],pixels[i*absWidth*4 + j*4 + 1],pixels[i*absWidth*4 + j*4 + 2],pixels[i*absWidth*4 + j*4 + 3])));    

  }
  glPopAttrib();
      
  //on s'assure que les hits sont unique
  sort(hits.begin(), hits.end());
  hits.erase(unique(hits.begin(), hits.end()), hits.end());
  
  return hits;
}

//-----------------------------------------------------------------------------
void Widget3d::pushFrameBuffer(const FrameBufferObject& iF /*=FrameBufferObject()*/)
{
  mFrameBuffers.push_back(iF);
  GLint frameBufferId = 0;
  if(iF.isValid())
    frameBufferId = iF.getFrameBufferId();
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferId);  
}

//-----------------------------------------------------------------------------
void Widget3d::pushShader(const Shader& iS /*=Shader()*/)
{
  mShaders.push_back(iS);
  GLint programId = 0;
  if(iS.isValid())
    programId = iS.getProgramId();
  glUseProgram(programId);
  
}

//-----------------------------------------------------------------------------
void Widget3d::popFrameBuffer()
{
  mFrameBuffers.pop_back();
  GLint frameBufferId = 0;
  if(!mFrameBuffers.empty())
    frameBufferId = mFrameBuffers.back().getFrameBufferId();
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferId);
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
  mCam.setWindowSize(iWidth, iHeight);
  mCam.applyProjectionTransformation();
  update();
}

//-----------------------------------------------------------------------------
void
Widget3d::setCamera( const Camera& iCam, bool iAnimate /*= true*/,
  int iDuration /*=1000ms*/ )
{
	/*On empêche de changer la camera lorsqu'on est en train
    de faire une animation.*/
	if(mAnimationTimerId != 0)
    return;
    
  mOldCam = mCam;
  mNewCam = iCam;
  
  if( iAnimate )
  {
    /*L'animation de la camera va interpoler la transformation entre la
      la vielle camera (mOldCam) et la nouvelle camera (mNewCame). Cette
      matrice interpolé sera appliqué a la camera courante (mCam).*/
      
    //we use a QTime to track animation time
    mAnimationDuration = iDuration;
    mAnimationTimer.start();
    //start a timer that will timeout as quick as possible which will trigger
    //the overloaded method timerEvent( QTimerEvent* )
    mAnimationTimerId = startTimer( 15 );
  }
  else
  {
    mCam = mNewCam;
    mCam.applyProjectionTransformation();  
    //update();
  }
}

//-----------------------------------------------------------------------------
//void Widget3d::setCameraMode( Camera::Mode iMode )
//{
//  mCam.setMode( iMode );
//  update();
//}

//-----------------------------------------------------------------------------
void Widget3d::setCameraOrientation( Camera::Orientation iO )
{
  mCam.setOrientation( iO );
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
  glDisable(GL_DEPTH_TEST);
  if(mFpsFrameCount >= kFramesToComputeFps)
  {
    mFps = mFpsFrameCount / (double)mFpsTimer.elapsed() * 1000.0;
    mFpsTimer = QTime::currentTime();
    mFpsFrameCount = 0;
  }
  renderText(5, 15, QString("fps: ") + QString::number(mFps, 'f', 2) );
  ++mFpsFrameCount;
  glEnable(GL_DEPTH_TEST);
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
    double animationTime = min( mAnimationDuration, mAnimationTimer.elapsed() );
    double t = animationTime / (double)mAnimationDuration;
    t = inversePower(t, 3);
    
    //animation du système de coordonnées de la caméra
    Matrix4d iterationMatrix = 
      math::interpolate(mOldCam.getTransformationToGlobal(),
        mNewCam.getTransformationToGlobal(), t);

    mCam.setTransformationToGlobal(iterationMatrix);
        
    /*animation de la position de la camera à l'intérieur
      du systeme de coordonnées*/
    
    /*Ici, on bircole une matrice (main droite) orthonormale qui réprésente
      l'orientation de la caméra (pos, lat, up, look). On interpolera
      la vielle orientation avec la nouvelle afin d'obtenir les positions,
      ainsi que les vecteurs lat, up er look intermédiaires.*/
    Vector3d look;
    Matrix4d m1;    
    m1.setRow1(mOldCam.getLat().getX(), mOldCam.getLat().getY(),mOldCam.getLat().getZ(), 0);
    m1.setRow2(mOldCam.getUp().getX(), mOldCam.getUp().getY(),mOldCam.getUp().getZ(), 0);    
		//La matrice doit être une matrice main droite (voir quaternion.h)
    look.set(mOldCam.getLook(), mOldCam.getPos());
    look.normalise();
    m1.setRow3(look.getX(), look.getY(),look.getZ(), 0);
    m1.setTranslation(mOldCam.getPos());
    
    Matrix4d m2;
    m2.setRow1(mNewCam.getLat().getX(), mNewCam.getLat().getY(),mNewCam.getLat().getZ(), 0);
    m2.setRow2(mNewCam.getUp().getX(), mNewCam.getUp().getY(),mNewCam.getUp().getZ(), 0);
    //La matrice doit être une matrice main droite (voir quaternion.h)
    look.set(mNewCam.getLook(), mNewCam.getPos());
    look.normalise();
    m2.setRow3(look.getX(), look.getY(),look.getZ(), 0);
    m2.setTranslation(mNewCam.getPos());

		iterationMatrix = math::interpolate(m1, m2, t);
    Vector3d interpolatedLook = toVector(mOldCam.getLook()) * (1 - t) +
      toVector(mNewCam.getLook()) * t;
    mCam.set(iterationMatrix.getTranslation(),
    	toPoint(interpolatedLook),
      iterationMatrix.getBaseY(),
      iterationMatrix.getBaseX());

    if ( animationTime >= mAnimationDuration )
    {
      killTimer( mAnimationTimerId );
      mAnimationTimerId = 0;
    }

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
    {
      mCam.setZoom(finalZoom);
      mCam.applyProjectionTransformation();
    }
  }
  else
  {
    int wheelDir = ipE->delta() > 0 ? 1.0 : -1.0;
    Camera c = getCamera();
    Vector3d lookDirection(c.getPos(), c.getLook());
    
    double logLook = log(lookDirection.fastNorm());
    Point3d p = getCamera().getPos() +
      lookDirection.normalise() * logLook * 0.8 * wheelDir;
      
    if( Vector3d(p, c.getLook()).norm() >= 1.0 )
    {
      c.setPos(p);
      setCamera(c, false);
//      printf("\n\nLook: %3.2f, %3.2f, %3.2f", c.getLook().getX(), c.getLook().getY(), c.getLook().getZ());
//      printf("\nPos: %3.2f, %3.2f, %3.2f", p.getX(), p.getY(), p.getZ());
    }
  }

  update();
}
