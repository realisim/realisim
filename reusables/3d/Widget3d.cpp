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
mControlType( ctRotateAround ),
mAnimationTimer(),
mAnimationTimerId( 0 ),
mCameraControlTimerId( 0 ),
mAnimationDuration(0),
mFps(0.0),
mFpsFrameCount(0),
mFpsTimer(),
mMousePressed( false ),
mMousePosX( 0 ),
mMousePosY( 0 ),
mCameraSpeed( 1.0 ),
mFrameBuffers(),
mShaders(),
mAbsoluteUpVector(auvY)
{ setFocusPolicy( Qt::StrongFocus ); }

//-----------------------------------------------------------------------------
Widget3d::~Widget3d()
{}

//-----------------------------------------------------------------------------
void Widget3d::beginFrame()
{
    makeCurrent();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    mCam.applyModelViewTransformation();
}
//-----------------------------------------------------------------------------
const Camera& Widget3d::getCamera() const
{
    const Camera* r = &mCam;
    //if( isAnimatingCamera() ){ r = &mNewCam; }
    return *r;
}
//-----------------------------------------------------------------------------
Widget3d::controlType Widget3d::getControlType() const
{ return mControlType; }
//-----------------------------------------------------------------------------
void Widget3d::initializeGL()
{
    QGLWidget::initializeGL();
    
#ifdef WIN32
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
#endif
    
    //print openGL info
    OpenGLInfo i;
    i.print();
    
    //useful for lights
    GLfloat shininess[] = {80.0};
    GLfloat position[]  = {50.0, 50.0, 50.0, 1.0};
    GLfloat ambiant[]   = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat diffuse[]   = {0.7, 0.7, 0.7, 1.0};
    GLfloat specular[]  = {1.0, 1.0, 1.0, 1.0};
    
    GLfloat mat_ambiant[] = {0.25f, 0.25f, 0.25f, 1.0f};
    GLfloat mat_diffuse[] = {0.6f, 0.6f, 0.6f, 1.0f};
    GLfloat mat_specular[]  = {1.0f, 1.0f, 1.0f, 1.0f};
    
    // Let OpenGL clear background to Grey
    //glClearColor(125/255.0f, 125/255.0f, 125/255.0f, 0.0);
    glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
    
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
bool Widget3d::isAnimatingCamera() const
{ return mAnimationTimerId != 0; }

//-----------------------------------------------------------------------------
bool Widget3d::isKeyPressed( int iKey ) const
{
    bool r = false;
    map< int, bool >::const_iterator it = mKeys.find( iKey );
    if( it != mKeys.end() ) { r = it->second; }
    return r;
}

//-----------------------------------------------------------------------------
void Widget3d::keyPressEvent(QKeyEvent* ipE)
{
    if( getControlType() != ctNone && !ipE->isAutoRepeat() )
    {
        mKeys[(int)ipE->key()] = true;
        if( mCameraControlTimerId == 0 )
        { mCameraControlTimerId = startTimer( 15 ); }
    }
}
//-----------------------------------------------------------------------------
void Widget3d::keyReleaseEvent( QKeyEvent* ipE )
{
    if( !ipE->isAutoRepeat() )
    {
        map< int, bool >::iterator it = mKeys.find(ipE->key());
        if (it != mKeys.end())
        {
            mKeys.erase(it);
        }
        
        if (mKeys.empty() && mCameraControlTimerId != 0)
        {
            killTimer(mCameraControlTimerId); mCameraControlTimerId = 0;
        }
    }
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
    
    if( mMousePressed && !isAnimatingCamera() ) //deplacement de la camera
    {
        int deltaX = e->x() - mMousePosX;
        int deltaY = e->y() - mMousePosY;
        
        Vector3d delta = getCamera().screenToWorld( Vector2d(deltaX, deltaY),
                                                   mCam.getLook() );
        switch ( getControlType() )
        {
            case ctPan: mCam.translate( -delta ); break;
            case ctRotateAround:
            {
                //arbitrairement, la taille du viewport correspond a une rotation de 360
                double radX = deltaX * 2 * PI / (double)mCam.getViewport().getWidth();
                double radY = deltaY * 2 * PI / (double)mCam.getViewport().getHeight();
                //rotation relative a x;
                mCam.rotate( -radX, Vector3d( 0.0, 1.0, 0.0 ), mCam.getLook() );
                //rotation relative a y
                mCam.rotate( -radY, mCam.cameraToWorld( Vector3d(1,0,0) ), mCam.getLook() );
            } break;
            case ctFree:
            {
                //arbitrairement, la taille du viewport correspond a une rotation de 180
                double radX = deltaX * PI / (double)mCam.getViewport().getWidth();
                double radY = deltaY * PI / (double)mCam.getViewport().getHeight();
                
                Vector3d up(0.0, 1.0, 0.0);
                if(mAbsoluteUpVector == auvZ)
                { up.setXYZ(0.0, 0.0, 1.0); }
                
                mCam.rotate( -radX, up, mCam.getPos() );
                mCam.rotate( -radY, mCam.getLat(),
                            mCam.getPos() );
            }break;
            default: break;
        }
    }
    
    mMousePosX = e->x();
    mMousePosY = e->y();
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
    beginFrame();
    
    //replacer les lumieres
    Vector4d pos(50.0, 30.0, 5.0, 0.0);
    if( mAbsoluteUpVector == auvZ )
    { pos.set(400, 280, 1000, 0.0); }
    GLfloat position[]  = {(float)pos.x(), (float)pos.y(), (float)pos.z(), (float)pos.w() };
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    
    draw();
    
#ifndef NDEBUG
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
    makeCurrent();
    GLint viewport[4]; //x, y, width, height
    glGetIntegerv(GL_VIEWPORT,viewport);
    
    /*glReadpixel prend le coin inférieur gauche et la taille, donc
     on va s'assurer de lui passer le coin inférieur gauche de la boite.*/
    int x1,y1,x2,y2;
    //on definit les 4 coins de la fenetre de selection
    //x1 = iX; y1 = iY; x2 = x1 + iWidth; y2 = y1 + iHeight;
    //on trouve le coin inférieur gauche de la boite de selection
    x1 = min(iX - iWidth / 2.0, iX + iWidth / 2.0 );
    y1 = max(iY - iHeight / 2.0, iY + iHeight / 2.0 );
    //le coin superieur droit
    x2 = max(iX - iWidth / 2.0, iX + iWidth / 2.0);
    y2 = min(iY - iHeight / 2.0, iY + iHeight / 2.0 );
    
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
    GLubyte *pixels = new GLubyte[absWidth * absHeight * 4];
    
    glPushAttrib(GL_COLOR_BUFFER_BIT | GL_POLYGON_BIT);
    //draw the scene in picking mode...
    /*On s'assure que le clear color est completement blanc parce qu'il
     représentera le id 2^32 et n'interferera donc pas dans la sélection.*/
    glClearColor(1, 1, 1, 1);
    beginFrame();
    drawSceneForPicking();
    
    glReadPixels(x1, viewport[3] - y1, absWidth, absHeight,
                 GL_RGBA,GL_UNSIGNED_BYTE,(void *)pixels);
    for(int i = 0; i < absHeight; ++i)
        for(int j = 0; j < absWidth; ++j)
            if(pixels[i*absWidth*4 + j*4] != 255 || pixels[i*absWidth*4 + j*4 + 1] != 255 || pixels[i*absWidth*4 + j*4 + 2] != 255 || pixels[i*absWidth*4 + j*4 + 3] != 255)
                hits.push_back(colorToId(QColor(pixels[i*absWidth*4 + j*4],pixels[i*absWidth*4 + j*4 + 1],pixels[i*absWidth*4 + j*4 + 2],pixels[i*absWidth*4 + j*4 + 3])));
    
    delete[] pixels;
    /*Quand la boite fait plus de 1x1, on fais une selection sur les back facing
     polygones aussi, ainsi permettant une selection qui passe au travers
     de la surface.*/
    //  if(absWidth > 1 || absHeight > 1 )
    //  {
    //    glEnable(GL_CULL_FACE);
    //    glCullFace(GL_FRONT);
    //    beginFrame();
    //    drawSceneForPicking();
    //
    //    glReadPixels(x1, viewport[3] - y1, absWidth, absHeight,
    //      GL_RGBA,GL_UNSIGNED_BYTE,(void *)pixels);
    //    for(int i = 0; i < absHeight; ++i)
    //      for(int j = 0; j < absWidth; ++j)
    //        if(pixels[i*absWidth*4 + j*4] != 255 || pixels[i*absWidth*4 + j*4 + 1] != 255 || pixels[i*absWidth*4 + j*4 + 2] != 255 || pixels[i*absWidth*4 + j*4 + 3] != 255)
    //          hits.push_back(colorToId(QColor(pixels[i*absWidth*4 + j*4],pixels[i*absWidth*4 + j*4 + 1],pixels[i*absWidth*4 + j*4 + 2],pixels[i*absWidth*4 + j*4 + 3])));
    //
    //  }
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
void Widget3d::resizeGL(int iWidth, int iHeight)
{
    QGLWidget::resizeGL(iWidth, iHeight);
    mCam.setViewportSize(iWidth, iHeight);
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
         matrice interpolé sera appliqué a la camera courante (mCam). La projection
         aussi sera interpolée.*/
        
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
void Widget3d::setControlType( controlType iT )
{ mControlType = iT; }

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
    glColor3ub(255, 255, 255);
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
        
        /*Ici, on bircole une matrice (main droite) orthonormale qui réprésente
         l'orientation de la caméra (pos, lat, up, look). On interpolera
         la vielle orientation avec la nouvelle afin d'obtenir les positions,
         ainsi que les vecteurs lat, up et look intermédiaires.*/
        Vector3d lookVector( mOldCam.getLook(), mOldCam.getPos() );
        lookVector.normalise();
        Matrix4 m1( mOldCam.getLat(), mOldCam.getUp(), lookVector );
        m1 = Matrix4( toVector(mOldCam.getPos()) ) * m1;
        lookVector.set( mNewCam.getLook(), mNewCam.getPos() );
        lookVector.normalise();
        Matrix4 m2( mNewCam.getLat(), mNewCam.getUp(), lookVector );
        m2 = Matrix4( toVector(mNewCam.getPos()) ) * m2;
        
        Matrix4 iterationMatrix =
        math::interpolate( m1, m2, t );
        Vector3d interpolatedLook = toVector(mOldCam.getLook()) * (1 - t) +
        toVector(mNewCam.getLook()) * t;
        mCam.set( toPoint(iterationMatrix.getTranslationAsVector()),
                 toPoint(interpolatedLook),
                 Vector3d( iterationMatrix(1, 0), iterationMatrix(1, 1), iterationMatrix(1, 2) ) );
        
        //--- animation de la projection
        Camera::Projection iProj = mNewCam.getProjection();
        Camera::Projection oldProj = mOldCam.getProjection();
        Camera::Projection newProj = mNewCam.getProjection();
        iProj.mRight = oldProj.mRight * (1-t) + newProj.mRight * t;
        iProj.mLeft = oldProj.mLeft * (1-t) + newProj.mLeft * t;
        iProj.mBottom = oldProj.mBottom * (1-t) + newProj.mBottom * t;
        iProj.mTop = oldProj.mTop * (1-t) + newProj.mTop * t;
        iProj.mNear = oldProj.mNear * (1-t) + newProj.mNear * t;
        iProj.mFar = oldProj.mFar * (1-t) + newProj.mFar * t;
        iProj.mZoomFactor =  oldProj.mZoomFactor * (1-t) + newProj.mZoomFactor * t;
        iProj.mType = newProj.mType;
        iProj.mProportionalToWindow = newProj.mProportionalToWindow;
        mCam.setProjection( iProj );
        mCam.applyProjectionTransformation();
        
        if ( animationTime >= mAnimationDuration )
        {
            killTimer( mAnimationTimerId );
            mAnimationTimerId = 0;
        }
        
        update();
    }
    else if( ipE->timerId() == mCameraControlTimerId && !isAnimatingCamera() )
    {
        switch ( getControlType() )
        {
            case ctRotateAround:
            {
                Vector3d v; double a = 1.0 * PI / 180;
                if( isKeyPressed( Qt::Key_W ) )
                { v += mCam.getLat(); a *= -1; }
                if( isKeyPressed( Qt::Key_S ) )
                { v += mCam.getLat(); }
                if( isKeyPressed( Qt::Key_A ) )
                { v += Vector3d(0, 1, 0); a *= -1; }
                if( isKeyPressed( Qt::Key_D ) )
                { v += Vector3d(0, 1, 0); }
                mCam.rotate( a, v, mCam.getLook() );
            }break;
            case ctFree:
            {
                Vector3d v;
                if( isKeyPressed( Qt::Key_W ) )
                { v += Vector3d( mCam.getPos(), mCam.getLook() ); }
                if( isKeyPressed( Qt::Key_S ) )
                { v += -Vector3d( mCam.getPos(), mCam.getLook() ); }
                if( isKeyPressed( Qt::Key_A ) )
                { v += mCam.getLat() * -1; }
                if( isKeyPressed( Qt::Key_D ) )
                { v += mCam.getLat(); }
                if( isKeyPressed( Qt::Key_Q ) )
                { v += mCam.getUp(); }
                if( isKeyPressed( Qt::Key_E ) )
                { v += mCam.getUp() * -1; }
                v.normalise();
                mCam.translate( v * getCameraSpeed() );
            } break;
            default: break;
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
    
    if( isAnimatingCamera() ) { return; }
    
    if(getCamera().getProjection().mType == Camera::Projection::tOrthogonal)
    {
        Point2d mousePos( ipE->x(), ipE->y() );
        double zoom = 1 / 1.15;
        if(ipE->delta() < 0)
            zoom = 1.15;
        double finalZoom = getCamera().getZoom() * zoom;
        if(finalZoom >= kMaxZoom && finalZoom <= kMinZoom)
        {                  
            Point3d workingPlane( 0.0, 0.0, mCam.getProjection().mNear );
            Point3d preZoom = mCam.screenToWorld( mousePos, workingPlane );
            mCam.setZoom(finalZoom);
            Point3d postZoom = mCam.screenToWorld( mousePos, workingPlane );
            
            //on trouve le delta en coordonnée oeil.
            Matrix4 viewMatrix = mCam.getViewMatrix();
            Vector3d ecDelta = ( viewMatrix * postZoom ) - ( viewMatrix * preZoom );
            
            Camera::Projection p = mCam.getProjection();
            p.mLeft -= ecDelta.x();
            p.mRight -= ecDelta.x();
            p.mTop -= ecDelta.y();
            p.mBottom -= ecDelta.y();
            mCam.setProjection( p );      
            mCam.applyProjectionTransformation();
        }
    }
    else
    {
        double wheelDir = ipE->delta() > 0 ? 2.0 : 0.5;
        setCameraSpeed( max(getCameraSpeed() * wheelDir, 0.0078125) );
    }
}
