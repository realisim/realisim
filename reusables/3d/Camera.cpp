/*
 *  Camera.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 06/12/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Camera.h"
#include <qgl.h>
#include "MathUtils.h"

using namespace Realisim;

static const double kNear = 50.0;
static const double kFar = 2000.0;
//-----------------------------------------------------------------------------
Camera::Camera( Mode iMode /*= PERSPECTIVE*/ ) : 
mMode( iMode ),
mOrientation( FREE ),
mTransformation(),
mPos(),
mLat(),
mLook(),
mUp(),
mVisibleGLUnit( 20 ),
mPixelPerGLUnit( 0.0 ),
mZoomFactor( 1.0 ),
mWindowInfo()
{
}

//-----------------------------------------------------------------------------
Camera::Camera( const Camera& iCam ) : 
mMode(),
mOrientation(),
mTransformation(),
mPos(),
mLat(),
mLook(),
mUp(),
mVisibleGLUnit(),
mPixelPerGLUnit(),
mZoomFactor(),
mWindowInfo()
{
  operator=( iCam );
}

//-----------------------------------------------------------------------------
Camera::~Camera()
{
}

//-----------------------------------------------------------------------------
//void Camera::computeLatAndUp()
//{
//  //on commence par calculer le vecteur lat√©rale parce que le vecteur up
//  //final est d√©pendant du vecteur lat√©ral
//  
//  Vector3d lookVect( getPos(), getLook()  );
//  
//  mLat = lookVect ^ getUp();
//  mLat.normalise();
//  
//  mUp = getLat() ^lookVect;
//  mUp.normalise();
//}

//-----------------------------------------------------------------------------
//déplace la caméra
//le delta est en coord GL
void Camera::move( Vector3d iDelta )
{
  switch ( getOrientation() ) 
  {
    case XY:
    case ZY:
    case XZ:
    {
      Point3d trans = mTransformation.getTranslation() + iDelta * mTransformation;
      mTransformation.setTranslation(trans);
      
    }
    break;
      
    case FREE:
    {
      //un delta de mVisiblGLUnit represente 360 degrée de rotation...
      //donc 2*Pi Rad
      
      //Le vecteur iDelta est un vecteur qui est dans le plan Lat-Up
      //On projecte le vecteur iDelta sur chacune des composentes du plan
      //afin de déterminer l'angle de ratation pour chaque axe
      
      //La projection sur l'axe laterale reprentera la rotation par rapport
      //a l'axe vertical (y) de la base
      Vector3d projDeltaOnLat = getLat() * ( getLat() & iDelta );
      
      //ROTATION PAR RAPPORT A LA COMPOSANTE Y DE LA TRANSFO DE LA CAMERA
      double angle1 = projDeltaOnLat.norm() * DEUX_PI / getVisibleGLUnit();
            
      //on test pour voir si le vecteur iDelta projeté sur le vecteur latérale
      //est dans le même sens que ce dernier ou non... Si ce n'est pas le
      //cas, on change la direction de l'angle.
      if( ( projDeltaOnLat + ( getLat() * projDeltaOnLat.norm() ) ) ==
            Vector3d(0.0, 0.0, 0.0) )
      {
        angle1 *= -1;
      }
      
      //y de la transfo de la camera
      Vector3d upRotAxis = Vector3d( 0.0, 1.0, 0.0 );//mTransformation.getBaseY();
      
      //on effectur la rotation autour de l'axe Up
      Point3d newPos = 
        rotatePoint( angle1,
                  getPos(),
                  upRotAxis,
                  getLook() );
      
      //on calcul le nouveau vecteur lateral. Ceci permet de controler un des
      //axes de la camera. Ceci empêche la caméra de basculer sur les cotés
      //ce qui donne comme effet que la caméra garde la tête droite. En controlant
      //le vecteur latéral, on controle aussi le vecteur up.
      
      //un point a l'extrémité du vecteur latéral
      Point3d latPos = getPos() + getLat();
      //on le fait tourner
      latPos = 
        rotatePoint( angle1,
                     latPos,
                     upRotAxis,
                     getLook() );
      
      //on crée le nouveau vecteur latérale
      Vector3d newLat( newPos, latPos );
      
      //on projete le nouveau vecteur latérale sur la composante x et z de
      //la transfo, ce qui a pour effet de garder la caméra stable. On ne peut
      //pas directement seter le vecteur up parce que ce dernier varie dans 2
      //plan distincts
      
      //x et z de la transfo
      Vector3d x = Vector3d(1.0, 0.0, 0.0);//mTransformation.getBaseX();
      Vector3d z = Vector3d(0.0, 0.0, 1.0);//mTransformation.getBaseZ();

      Vector3d projLatOnX = x * ( x & newLat );
      Vector3d projLatOnZ = z * ( z & newLat );
      Vector3d projXZ = projLatOnX + projLatOnZ;
      setLat( projXZ );
 
      
      //ROTATION PAR RAPPORT AU VECTEUR LAT
      //La projection de iDelta sur l'axe Up représente la rotation autour
      //de l'axe latérale
      Vector3d projDeltaOnUp = getUp() * ( getUp() & iDelta );

      double angle2 = projDeltaOnUp.norm() * DEUX_PI / getVisibleGLUnit() * -1;
            
      //on test pour voir si le vecteur iDelta projeté sur le vecteur latérale
      //est dans le même sens que ce dernier ou non... Si ce n'est pas le
      //cas, on change la direction de l'angle.
      if( ( projDeltaOnUp + ( getUp() * projDeltaOnUp.norm() ) ) ==
         Vector3d(0.0, 0.0, 0.0) )
      {
        angle2 *= -1;
      }

      //On effectue la rotation autour de l'axe latérale
      newPos =
        rotatePoint( angle2,
                     newPos,
                     getLat(),
                     getLook() );
      
      //on applique la nouvelle position.
      mPos = newPos;
    }
      break;
      
    default:
      break;
  }
}

//-----------------------------------------------------------------------------
Camera&
Camera::operator=( const Camera& iCam )
{
  mMode = iCam.getMode();
  mOrientation = iCam.getOrientation();
  mTransformation = iCam.getTransformation();
  mPos = iCam.getPos();
  mLat = iCam.getLat();
  mLook = iCam.getLook();
  mUp = iCam.getUp();
  mVisibleGLUnit = iCam.getVisibleGLUnit();
  mPixelPerGLUnit = iCam.getPixelPerGLUnit();
  mZoomFactor = iCam.getZoom();
  mWindowInfo = iCam.getWindowInfo();
  
  return *this;
}

//-----------------------------------------------------------------------------
//Convertie une position pixel a l'écran en coordonnée GL
//Les paramètres sont en pixels
Point3d Camera::pixelToGL( int iX, int iY ) const
{
  int* viewport = new int[4];
  glGetIntegerv( GL_VIEWPORT, viewport );
  
  int width = viewport[2];
  int heigth = viewport[3];
  
  Point3d result = getPos() - ( getLat() * (width/2.0) / mPixelPerGLUnit ) + 
    ( getUp() * (heigth/2.0) / mPixelPerGLUnit ) +
    getLat() * iX / mPixelPerGLUnit +
    getUp() * iY / mPixelPerGLUnit;
  
//  std::cout<<"x: "<<iX<<" y: "<<iY<<std::endl;
//  result.print();
  
  return result;
}

//-----------------------------------------------------------------------------
//Convertie une delta en pixel d'écran en delta GL
//Les paramètres sont en pixels
Vector3d Camera::pixelDeltaToGLDelta( int iDeltaX, int iDeltaY ) const
{
  Vector3d result =
    getLat() * iDeltaX / mPixelPerGLUnit +
    getUp() * iDeltaY / mPixelPerGLUnit;
  
  return result;
}

//-----------------------------------------------------------------------------
void Camera::projectionGL( int iWidth, int iHeight )
{
  if(iWidth >= iHeight)
    mWindowInfo.mOrientation = WindowInfo::oHorizontal;
  else
    mWindowInfo.mOrientation = WindowInfo::oVertical;
  
  //le coté le plus long montre la valeur mVisibleGLUnit unité GL
  mWindowInfo.mShortSide = qMin(iWidth, iHeight);
  mWindowInfo.mLongSide = qMax(iWidth, iHeight);
  
  computeProjection();
}
//-----------------------------------------------------------------------------
void Camera::computeProjection()
{
  int windowLongSide = getWindowInfo().mLongSide;
  int windowShortSide = getWindowInfo().mShortSide;
  float projectionLongSide = getVisibleGLUnit();
  mPixelPerGLUnit = windowLongSide / projectionLongSide;
  float projectionShortSide = windowShortSide * projectionLongSide / 
    windowLongSide; 
  
  //puisque le viewport est de -projectionLongSide a projectionLongSide et
  //que projectionLongSide represente mVisibleUnitGL, on doit diviser en 2 afin de 
  //conserver les proportions.
  projectionShortSide /= 2.0;
  projectionLongSide /= 2.0;
  
  if( getWindowInfo().mOrientation == WindowInfo::oHorizontal )
  {
    glViewport(0, 0, windowLongSide, windowShortSide );
    
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    
    switch ( getMode() ) 
    {
      case ORTHOGONAL:
        glOrtho(-projectionLongSide, projectionLongSide,
                -projectionShortSide, projectionShortSide, 
                kNear, kFar);
        break;
        
      case PERSPECTIVE:
        //gluPerspective (27.0, (GLfloat) projectionLongSide / (GLfloat) projectionShortSide, 0.5, 10000.0);
        glFrustum(-projectionLongSide, projectionLongSide,
                  -projectionShortSide, projectionShortSide, 
                  kNear, kFar);
        break;
      default:
        break;
    }
    
    glMatrixMode( GL_MODELVIEW );
    
  }
  else //vertical
  {
    glViewport(0, 0, windowShortSide, windowLongSide );
    
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    
    switch ( getMode() ) 
    {
      case ORTHOGONAL:
        glOrtho(-projectionShortSide, projectionShortSide,
                -projectionLongSide, projectionLongSide, 
                kNear, kFar);
        break;
        
      case PERSPECTIVE:
        //gluPerspective (27.0, (GLfloat) projectionShortSide / (GLfloat) projectionLongSide, 0.5, 10000.0);
        glFrustum(-projectionShortSide, projectionShortSide,
                -projectionLongSide, projectionLongSide, 
                kNear, kFar);
        break;
        
      default:
        break;
    }   
    
    glMatrixMode( GL_MODELVIEW );
  }
  
}

//-----------------------------------------------------------------------------
void Camera::set( const Point3d& iPos,
         const Point3d& iLook,
         const Vector3d& iUp )
{
  mPos = iPos;
  mLook = iLook;
  mUp = iUp;
  
  //on commence par calculer le vecteur lat√©rale parce que le vecteur up
  //final est d√©pendant du vecteur lat√©ral
  Vector3d lookVect( getPos(), getLook()  );
  
  mLat = lookVect ^ getUp();
  mLat.normalise();
  
  mUp = getLat() ^lookVect;
  mUp.normalise();
}

//-----------------------------------------------------------------------------
void Camera::setLat( const Vector3d& iLat )
{
  mLat = iLat;
  mLat.normalise();
  
  mUp = ( mLat ^ Vector3d( getPos(), getLook() ) ).normalise();
}

//-----------------------------------------------------------------------------
void Camera::setMode( Mode iMode )
{
  mMode = iMode;
}

//-----------------------------------------------------------------------------
void Camera::setOrientation( Orientation iO )
{
  mOrientation = iO;

  switch( mOrientation )
  {
    case XY:
    case ZY:
    case XZ:
      set( Point3d( 0, 0, 100 ),
           Point3d( 0, 0, 0 ),
           Vector3d( 0, 1, 0 ) );
      break;
    case FREE:
      set( Point3d( 60, 60, 60 ),
           Point3d( 0, 0, 0 ),
           Vector3d( 0, 1, 0 ) );
      break;
    default:
      break;
  }

  //l'orientation a changé, on s'assure de rafraichier la matrice
  //de transformation
  setTransformation( getTransformation() );
}

//-----------------------------------------------------------------------------
void Camera::setTransformation( const Matrix4d& iTransfo,
  bool iApplyOrientation/*=true*/ )
{
  if( iApplyOrientation )
  {
    switch ( getOrientation() ) 
    {
      case Camera::ZY:
      {
        Quat4d rot;
        rot.setRot( PI / 2.0, iTransfo.getBaseY() );
        mTransformation = iTransfo * rot.getUnitRotationMatrix();
        mTransformation.setTranslation( iTransfo.getTranslation() );
      }
        break;
      case Camera::XZ:
      {
        Quat4d rot;
        rot.setRot( -PI / 2.0, iTransfo.getBaseX() );
        mTransformation = iTransfo * rot.getUnitRotationMatrix();
        mTransformation.setTranslation( iTransfo.getTranslation() );
      }
        break;
      case Camera::XY:
      case Camera::FREE:
        mTransformation = iTransfo;
        break;
      default:
        break;
    }
  }
  else
  {
    mTransformation = iTransfo;
  }
}

//-----------------------------------------------------------------------------
void Camera::setUp( const Vector3d& iUp )
{
   mUp = iUp;

  Vector3d currentLookVect( getPos(), getLook()  );
  
  mLat = currentLookVect ^ getUp();
  mLat.normalise();
  
  Vector3d updatedLookVect = getUp() ^ getLat();
  mPos = getLook() - ( updatedLookVect.normalise() * currentLookVect.norm() );
}

//-----------------------------------------------------------------------------
void Camera::setZoom(double iZoom)
{
  mZoomFactor = iZoom;
  computeProjection();
}
