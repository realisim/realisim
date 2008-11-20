/*
 *  RealEdit3d.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 17/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef RealEdit3d_h
#define RealEdit3d_h

#include "Widget3d.h"

namespace RealEdit 
{
  class ObjectNode;
}

class RealEdit3d : public Realisim::Widget3d
{
public:
  RealEdit3d( QWidget* ipParent, 
              const QGLWidget* iSharedWidget,
             const RealEdit::ObjectNode& iObjectNode );
  
  virtual void paintGL();
  
private:
  const RealEdit::ObjectNode& mObjectNode;
  
};

#endif