/*
 *  MainWindow.h
 */

#ifndef MainDialog_hh
#define MainDialog_hh

#include "math/BezierCurve.h"
class QKeyEvent;
#include <QMainWindow>
class QMouseEvent;
#include "3d/Widget3d.h"
#include "math/PlatonicSolid.h"

using namespace realisim;
  using namespace math;

class Viewer : public realisim::treeD::Widget3d
{
public:
  Viewer(QWidget*);
  ~Viewer();

private:
  enum mouseState{msIdle, msDown, msDrag};
  enum state{sCreation, sEdition, sSelection};
  
  virtual void drawBezier(bool) const;
  virtual void drawSceneForPicking() const;
  virtual mouseState getMouseState() const {return mMouseState;}
  virtual state getState() const {return mState;}
  virtual void initializeGL();
  virtual void keyPressEvent(QKeyEvent*);
  virtual void mouseMoveEvent(QMouseEvent*);
  virtual void mousePressEvent(QMouseEvent*);
  virtual void mouseReleaseEvent(QMouseEvent*);
  virtual void paintGL();
  virtual void setMouseState(mouseState iS) {mMouseState = iS;}
  virtual void setState(state iS) {mState = iS;}
  
  mouseState mMouseState;
  unsigned int mMouseX;
  unsigned int mMouseY;
  std::vector<math::BezierCurve> mCurves;
  int mCurrentCurve;
  int mCurrentPoint;
  int mHover;
  state mState;
};


class MainDialog : public QMainWindow
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog(){};
  
public slots:

                
protected:
  Viewer* mpViewer;
};

#endif
