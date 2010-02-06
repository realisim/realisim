/*
 *  RealEdit3d.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 17/11/08.
 
  mController:
  mDisplayData:
  mEditionData:
  mMouseInfo:
  mMouseState:
    -msBoxSelection: dragging with selection tool
    -msCamera
    -msCameraDrag
    -msDown
    -msDrag
    -msIdle
  mPreviousTool:
 */

#ifndef RealEdit3d_h
#define RealEdit3d_h

#include "Controller.h"
class QKeyEvent;
class QMouseEvent;
#include <QPoint>
class QWheelEvent;
#include <vector>
#include "3d/Widget3d.h"

namespace realEdit 
{
using namespace realisim;
using namespace math;
  
class RealEdit3d : public realisim::treeD::Widget3d
{
public:
  RealEdit3d (QWidget* ipParent, 
              const QGLWidget* iSharedWidget,
              Controller& iC);
//RealEdit3d(const RealEdit3d&);
  virtual ~RealEdit3d ();
  virtual void changeCurrentNode();
  virtual void paintGL ();
  
protected:
  enum mouseState{msCamera, msCameraDrag,
    msDown, msDrag, msIdle};
  
  struct MouseInfo 
  {
    MouseInfo() : delta(), end(), origin(){;}    
    QPoint delta; QPoint end; QPoint origin;
  };

  virtual void changeCursor();
  virtual void drawAxis() const;
  virtual void drawBoundingBox(const RealEditModel&, bool = false) const;
  virtual void drawEdges(const RealEditModel&) const;
  virtual void drawNormals(const RealEditModel&) const;
  virtual void drawPoints(const RealEditModel&, bool = false) const;
  virtual void drawPolygons(const RealEditModel&, bool = false) const;
  virtual void drawScene(const ObjectNode* iObjectNode) const;
  virtual void drawSceneForPicking(const ObjectNode* iObjectNode) const;
  virtual void drawSelectionBox() const;
  virtual void enableSmoothLines() const;
  virtual bool isSelectionBoxShown() const {return mShowSelectionBox;}
  virtual mouseState getMouseState() const {return mMouseState;}
  virtual void keyPressEvent(QKeyEvent*);
  virtual void mouseDoubleClickEvent(QMouseEvent* e);
  virtual void mouseMoveEvent(QMouseEvent* e);
  virtual void mousePressEvent(QMouseEvent* e);
  virtual void mouseReleaseEvent(QMouseEvent* e);
  virtual std::vector<unsigned int> pick(int, int, int = 1, int = 1);
  virtual void setMouseState(mouseState m) {mMouseState = m;}
  virtual void showSelectionBox(bool iS) {mShowSelectionBox = iS;}
  virtual void wheelEvent(QWheelEvent* e);

private:
  Controller& mController;
  DisplayData& mDisplayData;
  const EditionData& mEditionData;
  MouseInfo mMouseInfo;
  mouseState mMouseState;
  Controller::tool mPreviousTool;
  bool mShowSelectionBox;
  unsigned int mHoverId;
};

} //realEdit

#endif