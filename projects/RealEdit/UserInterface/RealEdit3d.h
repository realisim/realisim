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
#include <Point.h>
class QKeyEvent;
class QMouseEvent;
class QWheelEvent;
#include <vector>
#include "Widget3d.h"

namespace realEdit 
{
using namespace realisim;
using namespace math;
  
class RealEdit3d : public realisim::treeD::Widget3d
{
public:
  enum tool{tCamera, tSelection};


  RealEdit3d (QWidget* ipParent, 
              const QGLWidget* iSharedWidget,
              RealEditController& iC);
//RealEdit3d(const RealEdit3d&);
  virtual ~RealEdit3d ();
  virtual void currentNodeChanged ();
  virtual void paintGL ();
  virtual void setTool(tool t) {mTool = t;}
  
protected:
  enum mouseState{msDown, msDrag, msIdle};
  
  struct MouseInfo 
  {
    MouseInfo() : delta(), end(), origin(){;}    
    Point3i delta; Point3i end; Point3i origin;
  };
  
  class Hits
  {
    public:
      Hits() : mMinDepth(0), mMaxDepth(0), mName(0) {;}
      Hits(double a, double b, uint c) : mMinDepth(b), mMaxDepth(a), mName(c) {;}
      Hits(const Hits& h) : mMinDepth(h.getMinDepth()),
         mMaxDepth(h.getMaxDepth()), mName(h.getName()) {;}
      ~Hits() {;}
      
      double getMaxDepth() const {return mMaxDepth;}
      double getMinDepth() const {return mMinDepth;}
      uint getName() const {return mName;}
      bool operator< (const Hits& h) const {return getMinDepth() < h.getMinDepth();}
      
    private:
      double mMinDepth;
      double mMaxDepth;
      uint mName;
  };

  virtual void changeCursor();
  virtual void drawAxis() const;
  virtual void drawBoundingBox(const RealEditModel&, bool = false) const;
  virtual void drawLines(const RealEditModel&) const;
  virtual void drawNormals(const RealEditModel&) const;
  virtual void drawPoints(const RealEditModel&, bool = false) const;
  virtual void drawPolygons(const RealEditModel&, bool = false) const;
  virtual void drawScene(const ObjectNode* iObjectNode) const;
  virtual void drawSceneForPicking(const ObjectNode* iObjectNode) const;
  virtual void enableSmoothLines() const;
  virtual mouseState getMouseState() const {return mMouseState;}
  virtual tool getTool() const {return mTool;}
  virtual void keyPressEvent(QKeyEvent*);
  virtual void mouseDoubleClickEvent(QMouseEvent* e);
  virtual void mouseMoveEvent(QMouseEvent* e);
  virtual void mousePressEvent(QMouseEvent* e);
  virtual void mouseReleaseEvent(QMouseEvent* e);
  virtual std::vector<Hits> pick(int, int);
  virtual std::vector<Hits> processHits(int, unsigned int[]);
  virtual void setMouseState(mouseState m) {mMouseState = m;}
  virtual void wheelEvent(QWheelEvent* e);

private:
  RealEditController& mController;
  DisplayData& mDisplayData;
  const EditionData& mEditionData;
  MouseInfo mMouseInfo;
  mouseState mMouseState;
  tool mTool;
  tool mPreviousTool;
};

} //realEdit

#endif