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
  class RealEdit3d;
  class EditionData;
  class ObjectNode;
}

class RealEdit::RealEdit3d : public Realisim::Widget3d
{
public:
  RealEdit3d( QWidget* ipParent, 
              const QGLWidget* iSharedWidget,
              const EditionData& iEditionData );
  ~RealEdit3d();
  
  void currentNodeChanged();
  virtual void paintGL();
  void initDisplayList();

private:
  void drawCube() const;
  void drawScene( const RealEdit::ObjectNode* iObjectNode ) const;

  
  
  //index de display list
  static GLuint mCube;
  
  //reference sur le EditionData de RealEditController
  const EditionData& mEditionData;
  
};

#endif