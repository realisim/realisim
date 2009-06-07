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

namespace realEdit {class RealEditController;}
#include "Widget3d.h"

namespace realEdit 
{

class RealEdit3d : public realisim::treeD::Widget3d
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
  void drawNormals (const RealEditModel iM) const;
  void drawPolygons (const RealEditModel iM) const;
  void drawScene (const ObjectNode* iObjectNode) const;
  void enableSmoothLines () const;

private:
  RealEditController& mController;
  DisplayData& mDisplayData;
  const EditionData& mEditionData;
};

} //realEdit

#endif