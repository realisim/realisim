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
    
    //On met un - devant le delta pour donner l'impression qu'on ne 
    //déplace pas la camera, mais le model. Si on ne mettait pas de -,
    //la caméra se déplacerait en suivant la souris et ce qu'on voit a l'écran
    //s'en irait dans le sens contraire de la souris. En mettant le - on
    //donne l'impression de déplacer le contenu de l'écran.
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