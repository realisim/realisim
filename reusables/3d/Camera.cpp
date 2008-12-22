/*
 *  Camera.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 06/12/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Camera.h"
#include "MathUtils.h"

#include <qgl.h>

using namespace Realisim;

//-----------------------------------------------------------------------------
Camera::Camera( Mode iMode /*= PERSPECTIVE*/ ) : mMode( iMode ),
mPos(),
mLat(),
mLook(),
mUp(),
mVisibleGLUnit( 20 ),
mPixelPerGLUnit( 0.0 ),
mZoomFactor( 1.0 )
{
}

//-----------------------------------------------------------------------------
Camera::Camera( const Camera& iCam ) : mMode( iCam.getMode() ),
mPos(),
mLat(),
mLook(),
mUp(),
mVisibleGLUnit(),
mPixelPerGLUnit( 0.0 ),
mZoomFactor()
{
  //TODO: regarder si on peut implanter le constructeur copie en fonction de l'operateur égale.
  operator=( iCam );
}

//-----------------------------------------------------------------------------
Camera::~Camera()
{
}

//-----------------------------------------------------------------------------
void Camera::computeLatAndUp()
{
  //on commence par calculer le vecteur lat√©rale parce que le vecteur up
  //final est d√©pendant du vecteur lat√©ral
  
  Vector3d lookVect( getPos(), getLook()  );
  
  mLat = lookVect ^ getUp();
  mLat.normalise();
  
  mUp = getLat() ^lookVect;
  mUp.normalise();
}

//-----------------------------------------------------------------------------
void Camera::lookAt()
{
  gluLookAt( getPos().getX(),
             getPos().getY(), 
             getPos().getZ(),
             getLook().getX(),
             getLook().getY(),
             getLook().getZ(),
             getUp().getX(),
             getUp().getY(),
             getUp().getZ() );
}

//-----------------------------------------------------------------------------
void Camera::move( Vector3d iDelta )
{
  switch ( getMode() ) 
  {
    case ORTHOGONAL:
    {
      //On n'utilise pas les m√©thode setPos et setLook ici parce
      //qu'on ne veut pas recalculer le vecteur lat√©rale et up, car
      //on sait qu'il sont inchang√©s puisque la pos et le look
      //se d√©place d'une valeur √©gale.
      //Si on utilisait les m√©thodes set, les valeurs du vecteur lat√©ral
      // et up serait modifi√©es ( due aux erreurs d'arrondissement ).
      mPos = Point3d( getPos().getX() + iDelta.getX(),
                      getPos().getY() + iDelta.getY(), 
                      getPos().getZ() + iDelta.getZ() );
      
      mLook = Point3d( getLook().getX() + iDelta.getX(),
                       getLook().getY() + iDelta.getY(), 
                       getLook().getZ() + iDelta.getZ() );
      
    }
    break;
      
    case PERSPECTIVE:
    {
      //un delta de mVisiblGLUnit represente 360 degrée de rotation...
      //donc 2*Pi Rad
      
      //Le vecteur iDelta est un vecteur qui est dans le plan Lat-Up
      //On projecte le vecteur iDelta sur chacune des composentes du plan
      //afin de déterminer l'angle de ratation pour chaque axe
      Vector3d projDeltaOnLat = getLat() * ( getLat() & iDelta );
      Vector3d projDeltaOnUp = getUp() * ( getUp() & iDelta );
      
      //ROTATION PAR RAPPORT AU VECTEUR UP
      double angle1 = projDeltaOnLat.norm() * DEUX_PI / getVisibleGLUnit();
            
      //on test pour voir si le vecteur iDelta projeté sur le vecteur latérale
      //est dans le même sens que ce dernier ou non... Si ce n'est pas le
      //cas, on change la direction de l'angle.
      if( ( projDeltaOnLat + ( getLat() * projDeltaOnLat.norm() ) ) ==
            Vector3d(0.0, 0.0, 0.0) )
      {
        angle1 *= -1;
      }
      
      //on effectur la rotation autour de l'axe Up
      Point3d newPos = 
        rotatePoint( angle1,
                     getPos(),
                     getUp(),
                     getLook() );
      
      
      //ROTATION PAR RAPPORT AU VECTEUR LAT
      double angle2 = projDeltaOnUp.norm() * DEUX_PI / getVisibleGLUnit() * -1;
            
      if( ( projDeltaOnUp + ( getUp() * projDeltaOnUp.norm() ) ) ==
         Vector3d(0.0, 0.0, 0.0) )
      {
        angle2 *= -1;
      }
      
      //on effectue la rotation par rapport a l'axe Latérale
      newPos =
        rotatePoint( angle2,
                     newPos,
                     getLat(),
                     getLook() );
      
      //On force le vecteur UP a etre parallele a l'axe Y. Cette technique
      //facilite beaucoup l'usage de la caméra      
      setUp( Vector3d( 0.0, getUp().getY() ? getUp().getY():1.0, 0.0 ) );
      
      setPos( newPos );
      
//      Vector3d up( 0.0, getUp().getY(), 0.0 );
//      if ( getUp().getY() >= 0.0 && getUp().getY() <= 0.009 )
//      {
//        up.set( getUp() );
//      }
//      setUp( up );
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
  mPos = iCam.getPos();
  mLat = iCam.getLat();
  mLook = iCam.getLook();
  mUp = iCam.getUp();
  mVisibleGLUnit = iCam.getVisibleGLUnit();
  mPixelPerGLUnit = iCam.getPixelPerGLUnit();
  mZoomFactor = iCam.getZoom();
  
  return *this;
}

//-----------------------------------------------------------------------------
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
Vector3d Camera::pixelDeltaToGLDelta( int iDeltaX, int iDeltaY ) const
{
  Vector3d result =
    getLat() * iDeltaX / mPixelPerGLUnit +
    getUp() * iDeltaY / mPixelPerGLUnit;
  
  return result;
}

//-----------------------------------------------------------------------------
void Camera::projectionGL( int iWidth, int iHeight ) const
{
  bool horiz = iWidth >= iHeight;
  
  //le cot√© le plus long montre la valeur mVisibleGLUnit unit√© GL
  int windowShortSide = qMin(iWidth, iHeight);
  int windowLongSide = qMax(iWidth, iHeight);
  
  float projectionLongSide = getVisibleGLUnit();
  mPixelPerGLUnit = windowLongSide / projectionLongSide;
  float projectionShortSide = windowShortSide * projectionLongSide / windowLongSide; 
  
  //puisque le viewport est de -projectionLongSide a projectionLongSide et
  //que projectionLongSide represente mVisibleUnitGL, on doit diviser en 2 afin de 
  //conserver les proportions.
  projectionShortSide /= 2.0;
  projectionLongSide /= 2.0;
  
  if( horiz )
  {
    glViewport(0, 0, windowLongSide, windowShortSide );
    
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    
    switch ( getMode() ) 
    {
      case ORTHOGONAL:
        glOrtho(-projectionLongSide, projectionLongSide,
                -projectionShortSide, projectionShortSide, 
                0.0, 100.0);
        break;
        
      case PERSPECTIVE:
        gluPerspective (27.0, (GLfloat) windowLongSide / (GLfloat) windowShortSide, 0.5, 10000.0);
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
                0.0, 100.0);
        break;

      case PERSPECTIVE:
        gluPerspective (27.0, (GLfloat) windowShortSide / (GLfloat) windowLongSide, 0.5, 10000.0);
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
  
  computeLatAndUp();
}

//-----------------------------------------------------------------------------
void Camera::setLat( const Vector3d& iLat )
{
  //TODO: cette fonction n'est pas vraiment testée...
  mLat = iLat;
  mLat.normalise();
  
  mUp = ( mLat ^ Vector3d( getPos(), getLook() ) ).normalise();
  //on ne doit pas utiliser computeLatAndUp parce que cette
  //derniere recalcule le vecteur Lat basé sur le Up... étant donné
  //que c'est le vecteur Lat qu'on désire setter on ne veut pas l'écraser
  //avec les valeures calculées par computeLatAndUp
}

//-----------------------------------------------------------------------------
void Camera::setLook( const Point3d& iLook )
{
  mLook = iLook;
  
  computeLatAndUp();
}

//-----------------------------------------------------------------------------
void Camera::setMode( Mode iMode )
{
  mMode = iMode;
}

//-----------------------------------------------------------------------------
void Camera::setPos( const Point3d& iPos )
{
  mPos = iPos;
  
  computeLatAndUp();
}

//-----------------------------------------------------------------------------
void Camera::setUp( const Vector3d& iUp )
{
  mUp = iUp;
  
  computeLatAndUp();
}