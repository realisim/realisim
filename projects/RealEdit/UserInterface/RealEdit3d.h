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
/*

  mDisplayListCube
*/
namespace RealEdit 
{
  class DisplayData;
  class EditionData;
  class ObjectNode;
  class RealEdit3d;
  class RealEditController;
  class RealEditModel;
}

class RealEdit::RealEdit3d : public Realisim::Widget3d
{
public:
  RealEdit3d (QWidget* ipParent, 
              const QGLWidget* iSharedWidget,
              RealEditController& iC);
  virtual ~RealEdit3d ();
  void currentNodeChanged ();
  virtual void paintGL ();
  
protected:
  void drawAxis () const;
  void drawNormals (const RealEdit::RealEditModel iM) const;
  void drawPolygons (const RealEdit::RealEditModel iM) const;
  void drawScene (const RealEdit::ObjectNode* iObjectNode) const;
  void enableSmoothLines () const;

private:
  RealEditController& mController;
  DisplayData& mDisplayData;
  const EditionData& mEditionData;
  
};

#endif