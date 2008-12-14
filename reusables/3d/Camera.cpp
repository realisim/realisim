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
mUp()
{
}

//-----------------------------------------------------------------------------
Camera::Camera( const Camera& iCam ) : mMode( iCam.getMode() ),
mPos( iCam.getPos() ),
mLat( iCam.getLat() ),
mLook( iCam.getLook() ),
mUp( iCam.getUp() )
{
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
void Camera::move( int iDeltaX, int iDeltaY )
{
  switch ( getMode() ) 
  {
    case ORTHOGONAL:
    {
      Vector3d x = getLat() * iDeltaX;
      Vector3d y = getUp() * iDeltaY;
      Vector3d delta = x + y;
      
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
  
  return *this;
}

//-----------------------------------------------------------------------------
void Camera::projectionGL( int iWidth, int iHeight ) const
{
  bool horiz = iWidth >= iHeight;
  
  int windowShortSide = qMin(iWidth, iHeight);
  int windowLongSide = qMax(iWidth, iHeight);
  
  float projectionShortSide = 10.0; //on veut un carré de 20 unités de coté
  float projectionLongSide = windowLongSide * projectionShortSide / windowShortSide; 
  
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