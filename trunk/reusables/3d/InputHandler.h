/*
 *  InputHandler.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 09/12/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef Realisim_InputHandler_hh
#define Realisim_InputHandler_hh

#include <QWidget>
#include <QMouseEvent>
#include <QWheelEvent>

namespace realisim
{
namespace treeD
{

class InputHandler : public QWidget
{
public:
  InputHandler(){;}
  virtual ~InputHandler(){;}
  
  virtual void mouseDoubleClickEvent( QMouseEvent* e ){;}
  virtual void mouseMoveEvent( QMouseEvent* e ){;}
  virtual void mousePressEvent( QMouseEvent* e ){;}
  virtual void mouseReleaseEvent( QMouseEvent* e ){;}
  
  virtual void wheelEvent ( QWheelEvent * event ){;}
};

} //treeD
} //realisim
#endif