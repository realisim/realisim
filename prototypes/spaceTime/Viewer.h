#ifndef SpaceTime_Viewer_h
#define SpaceTime_Viewer_h

#include "3d/Widget3d.h"
#include "Engine.h"
#include "math/BoundingBox.h"
class QKeyEvent;
class QMouseEvent;
namespace SpaceTime { class Viewer; }

using namespace realisim;
	using namespace treeD;

class SpaceTime::Viewer : public realisim::treeD::Widget3d,
													public SpaceTime::Engine::Client
{
public:
  Viewer(QWidget*, SpaceTime::Engine&);
  ~Viewer();

  virtual void call(Client::message);
	virtual void initialize();
	virtual bool isDebugging() const {return mIsDebugging;}
	virtual void setAsDebugging(bool iD);
  virtual double getInnerRadius() const {return mInnerRadius;}
  virtual double getOuterRadius() const {return mOuterRadius;}
  virtual void update();
  
protected:
  enum cubeMapSide{cmsPosX, cmsNegX, cmsPosY, cmsNegY, cmsPosZ, cmsNegZ};
	enum levelOfDetail{lodHigh, lodMed, lodLow};
  
  virtual void centerMouse();
  virtual void draw();
  virtual void drawArrow();
  virtual void drawBodies(double, double, levelOfDetail = lodHigh);
  virtual void drawCubeMapFace(cubeMapSide, const Camera&, double, double);
  virtual void drawDebuggingBox(const BB3d&);
  virtual void drawDistantBodies(double, double);
  virtual void drawSceneForPicking() const;
  virtual Camera getCubeMapCamera(cubeMapSide, const BB3d&);
  virtual void initializeDisplayLists();
  virtual void initializeGL();
  virtual bool isCameraFollowingShip() const {return mCameraFollowsShip;}
  virtual void keyPressEvent(QKeyEvent*);
  virtual void keyReleaseEvent(QKeyEvent*);
  virtual void mouseMoveEvent(QMouseEvent*);
//  virtual void mousePressEvent(QMouseEvent*);
//  virtual void mouseReleaseEvent(QMouseEvent*);
  virtual void paintGL();
  virtual void resizeGL(int, int);
  
  Camera mShipCamera;
  GLint mSphere;
  GLint mCube;
  double mOuterRadius;
  double mInnerRadius;
  bool mCameraFollowsShip;
  bool mIsDebugging;
  bool mIsThirdPersonView;
  FrameBufferObject mCubeMapFbo;
  double mThresholdToRenderCubeMap;
  Point3d mLastCubeMapRenderPosition;
};


#endif

