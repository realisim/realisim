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
#include "Widget3d.h"

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
  
  class Hits
  {
    public:
      enum type{tModel, tPoint, tPolygon, tUnknown};
      Hits() : mMinDepth(0), mMaxDepth(0), mId(0), mType(tUnknown) {;}
      Hits(double a, double b, uint c, type t) :
        mMinDepth(b), mMaxDepth(a), mId(c), mType(t) {;}
      Hits(const Hits& h) : mMinDepth(h.getMinDepth()),
         mMaxDepth(h.getMaxDepth()), mId(h.getId()), mType(h.getType()) {;}
      ~Hits() {;}
      
      double getMaxDepth() const {return mMaxDepth;}
      double getMinDepth() const {return mMinDepth;}
      uint getId() const {return mId;}
      type getType() const {return mType;}
      bool operator< (const Hits& h) const {return getMinDepth() < h.getMinDepth();}
      
    private:
      double mMinDepth;
      double mMaxDepth;
      uint mId;
      type mType;
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
  virtual void drawSelectionBox() const;
  virtual void enableSmoothLines() const;
  virtual bool isSelectionBoxShown() const {return mShowSelectionBox;}
  virtual mouseState getMouseState() const {return mMouseState;}
  virtual void keyPressEvent(QKeyEvent*);
  virtual void mouseDoubleClickEvent(QMouseEvent* e);
  virtual void mouseMoveEvent(QMouseEvent* e);
  virtual void mousePressEvent(QMouseEvent* e);
  virtual void mouseReleaseEvent(QMouseEvent* e);
  virtual std::vector<Hits> pick(int, int, int = 1, int = 1);
  virtual std::vector<Hits> processHits(int, unsigned int[]);
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
};

} //realEdit

#endif