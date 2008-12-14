/*
 *  DefaultInputHandler.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 13/12/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "DefaultInputHandler.h"

using namespace Realisim;

//-----------------------------------------------------------------------------
DefaultInputHandler::DefaultInputHandler( Camera& iCam ) :
mCam( iCam ),
mMousePressed( false ),
mMousePosX( 0 ),
mMousePosY( 0 )
{
}

//-----------------------------------------------------------------------------
//virtual void mouseDoubleClickEvent( QMouseEvent* e ){;}

//-----------------------------------------------------------------------------
void DefaultInputHandler::mouseMoveEvent( QMouseEvent* e )
{
  if( mMousePressed )
  {
    int deltaX = e->x() - mMousePosX;
    int deltaY = e->y() - mMousePosY;
    mCam.move( deltaX, deltaY );
  }
  
  mMousePosX = e->x();
  mMousePosY = e->y();
}

//-----------------------------------------------------------------------------
void DefaultInputHandler::mousePressEvent( QMouseEvent* e )
{
  mMousePressed = true;
  mMousePosX = e->x();
  mMousePosY = e->y();
}

//-----------------------------------------------------------------------------
void DefaultInputHandler::mouseReleaseEvent( QMouseEvent* e )
{
  mMousePressed = false;
}

//-----------------------------------------------------------------------------
//virtual void wheelEvent ( QWheelEvent * event );