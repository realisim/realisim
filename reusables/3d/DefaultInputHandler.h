/*
 *  DefaultInputHandler.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 13/12/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef Realisim_DefaultInputHandler_hh
#define Realisim_DefaultInputHandler_hh

#include "InputHandler.h"
#include "Camera.h"

namespace realisim
{
namespace treeD
{

class DefaultInputHandler : public InputHandler
{
public:
  DefaultInputHandler( Camera& iCam );
  virtual ~DefaultInputHandler(){;}
  
  //virtual void mouseDoubleClickEvent( QMouseEvent* e ){;}
  virtual void mouseMoveEvent( QMouseEvent* e );
  virtual void mousePressEvent( QMouseEvent* e );
  virtual void mouseReleaseEvent( QMouseEvent* e );
  virtual void wheelEvent ( QWheelEvent * event );
  
private:
  Camera& mCam;
  
  bool mMousePressed;
  int mMousePosX;
  int mMousePosY;
};

} //treeD
} //realisim
#endif