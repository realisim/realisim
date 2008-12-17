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
mVisibleGLUnit( 80 ),
mZoomFactor( 1.0 )
{
}

//-----------------------------------------------------------------------------
Camera::Camera( const Camera& iCam ) : mMode( iCam.getMode() ),
mPos(),
mLat(),
mLook(),
mUp(),
mVisibleGLUnit()
{
  //TODO: regarder si on peut implanter le constructeur copie en fonction de l'operateur �gale.
  operator=( iCam );
}

//-----------------------------------------------------------------------------
Camera::~Camera()
{
}

//-----------------------------------------------------------------------------
void Camera::computeLatAndUp()
{
  //on commence par calculer le vecteur latérale parce que le vecteur up
  //final est dépendant du vecteur latéral
  
  Vector3d lookVect( getLook(), getPos() );
  
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
void Camera::move( int iX, int iY )
{
  switch ( getMode() ) 
  {
    case ORTHOGONAL:
    {
      Point3d delta = pixelToGL( iX, iY );
      //On n'utilise pas les méthode setPos et setLook ici parce
      //qu'on ne veut pas recalculer le vecteur latérale et up, car
      //on sait qu'il sont inchangés puisque la pos et le look
      //se déplace d'une valeur égale.
      //Si on utilisait les méthodes set, les valeurs du vecteur latéral
      // et up serait modifiées ( due aux erreurs d'arrondissement ).
      mPos = Point3d( getPos().getX() + delta.getX(),
                      getPos().getY() + delta.getY(), 
                      getPos().getZ() + delta.getZ() );
      
      mLook = Point3d( getLook().getX() + delta.getX(),
                       getLook().getY() + delta.getY(), 
                       getLook().getZ() + delta.getZ() );
      
    }
    break;
      
    case PERSPECTIVE:
    {      
      Point3d newPos = 
        rotatePoint( (double)iX / 20.0,
                     getPos(),
                     getUp(),
                     getLook() );
      newPos =
        rotatePoint( (double)iY / 20.0,
                     newPos,
                     getLat(),
                     getLook() );
      setPos( newPos );
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
  mZoomFactor = iCam.getZoom();
  
  return *this;
}

//-----------------------------------------------------------------------------
Point3d Camera::pixelToGL( int iX, int iY ) const
{
  int* viewport = new int[4];
  glGetIntegerv( GL_VIEWPORT, viewport );
  
  //le coté le plus long montre la valeur mVisibleGLUnit unité GL
  int windowLongSide = qMax( viewport[2], viewport[3] );

  double onePixelInGLUnit = 1 * mVisibleGLUnit / windowLongSide;
  
  //on va faire un point sur un plan parallele au plan de projection de la
  //camera.
  Vector3d x = getLat() * iX * onePixelInGLUnit;
  Vector3d y = getUp() * iY * onePixelInGLUnit;
  Vector3d result = x + y;
  return Point3d( result.getX(), result.getY(), result.getZ() );
}

//-----------------------------------------------------------------------------
void Camera::projectionGL( int iWidth, int iHeight ) const
{
  bool horiz = iWidth >= iHeight;
  
  //le coté le plus long montre la valeur mVisibleGLUnit unité GL
  int windowShortSide = qMin(iWidth, iHeight);
  int windowLongSide = qMax(iWidth, iHeight);
  
  float projectionLongSide = mVisibleGLUnit * mZoomFactor;
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