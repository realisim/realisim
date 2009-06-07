/*
 *  DefaultInputHandler.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 13/12/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "DefaultInputHandler.h"

using namespace realisim;
using namespace realisim::treeD;

static const double kMaxZoom = 1/128.0;
static const double kMinZoom = 128;
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
    
    //on met un - devant le deltaY parce que le systeme de fenetrage QT
    //a laxe Y vers le bas et notre systeme de fenetrage GL a l'axe y vers le
    //haut.
    Vector3d delta = mCam.pixelDeltaToGLDelta( deltaX, -deltaY );
    
    //On met un - devant le delta pour donner l'impression qu'on ne 
    //déplace pas la camera, mais le model. Si on ne mettait pas de -,
    //la caméra se déplacerait en suivant la souris et ce qu'on voit a l'écran
    //s'en irait dans le sens contraire de la souris. En mettant le - on
    //donne l'impression de déplacer le contenu de l'écran.
    mCam.move( -delta );
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
void DefaultInputHandler::wheelEvent ( QWheelEvent* e )
{
  double zoom = 1 / 1.15;
  if(e->delta() < 0)
    zoom = 1.15;
  double finalZoom = mCam.getZoom() * zoom;
  if(finalZoom >= kMaxZoom && finalZoom <= kMinZoom)
    mCam.setZoom(finalZoom);
}