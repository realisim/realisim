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

using namespace Realisim;

//-----------------------------------------------------------------------------
Camera::Camera( Mode iMode /*= PROJECTION*/ ) : mMode( iMode ),
mPos(),
mLook(),
mUp()
{
}

//-----------------------------------------------------------------------------
Camera::Camera( const Camera& iCam ) : mMode( iCam.getMode() ),
mPos( iCam.getPos() ),
mLook( iCam.getLook() ),
mUp( iCam.getUp() )
{
}

//-----------------------------------------------------------------------------
Camera::~Camera()
{
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
Camera&
Camera::operator=( const Camera& iCam )
{
  mMode = iCam.getMode();
  mPos = iCam.getPos();
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
  
  float projectionShortSide = 10.0;
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
                projectionShortSide, -projectionShortSide, 
                0.0, 100.0);
        break;
        
      case PROJECTION:
        gluPerspective (27.0, (GLfloat) iWidth / (GLfloat) iHeight, 0.5, 10000.0);
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
                projectionLongSide, -projectionLongSide, 
                4.0, 15.0);
        break;

      case PROJECTION:
        gluPerspective (27.0, (GLfloat) iWidth / (GLfloat) iHeight, 0.5, 10000.0);
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
}

//-----------------------------------------------------------------------------
void Camera::setLook( const Point3d& iLook )
{
  mLook = iLook;
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
}

//-----------------------------------------------------------------------------
void Camera::setUp( const Vector3d& iUp )
{
  mUp = iUp;
}