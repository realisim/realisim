
#ifndef Realisim_E3d_Camera_hh
#define Realisim_E3d_Camera_hh

#include "math/Matrix4x4.h"
#include "math/Point.h"
#include "math/Vect.h"

/*
  --members
  mMode: mode de projection, Orthogonal ou Perspective
  mOrientation: Orientation de la camera
mTransformation: transformation appliqué au systeme d'axe de la caméra.
    cette transformation est: syteme local à system global. Le Widget 3d
    utilisera cette transformation pour transformer la position de la caméra
    en position global (de scene).
  mPos: position de la caméra..
  mLat: vecteur latéral normalisé
  mLook:  point visé.
  mUp: vecteur up normalisé.
  mLeft;
  mRight;
  mBottom;
  mTop;
  mNear;
  mFar;
  mZoomFactor: Facteur multiplicateur sur mVisibleGLUnit.
  mProportional:
  mPixelPerGLUnit: le rapport entre les pixel d'écran et les unité GL.
  mWindowInfo: Information sur la fenetre. Utile afin de calculer la
    matrice de projection.
    -mOrientation: indique quel coté de la fenêtre est le plus long
    -mShortSide: taille en pixel du petit coté
    -mLongSide: taille en pixel du long coté
  mIsActive: Sert à indiquer si la camera est active ou non. Lorsque 
    la caméra est active, la projection est calculée et les appels openGl
    tel que glOrtho ou glFrustum sont faits.
*/
namespace realisim
{
namespace treeD
{
using namespace realisim::math;

class Camera
{
public:
  
  enum Mode{ ORTHOGONAL = 0, PERSPECTIVE };
  enum Orientation{ XY, ZY, XZ, FREE };
  
  //windowInfo devrait etre remplacé par viewport...
  struct WindowInfo
  {
    WindowInfo() : mLongSide(1), mShortSide(1){;}
    enum Orientation { oHorizontal, oVertical };
    int getHeight() const{return mOrientation == oVertical ? mLongSide : mShortSide;}    
    int getWidth() const{return mOrientation == oHorizontal ? mLongSide : mShortSide;}

    Orientation mOrientation;
    int mLongSide;
    int mShortSide;
  };

  Camera( Mode iMode = PERSPECTIVE );
  Camera( const Camera& iCam );
  virtual ~Camera();
  
  void applyModelViewTransformation() const;
  void applyProjectionTransformation() const;
  const Vector3d& getLat() const { return mLat; }
  const Point3d& getLook() const { return mLook; }
  Mode getMode() const { return mMode; }
  Orientation getOrientation() const { return mOrientation; }
  const Point3d& getPos() const { return mPos; }
  const double getPixelPerGLUnit() const { return mPixelPerGLUnit; }
  const Matrix4d& getTransformationToLocal() const { return mToLocal; }
  const Matrix4d& getTransformationToGlobal() const { return mToGlobal; }
  const Vector3d& getUp() const { return mUp; }
  const WindowInfo& getWindowInfo() const {return mWindowInfo;}
  const double getZoom() const { return mProjectionInfo.mZoomFactor; }
  void move( const Vector3d& );
  Camera& operator=( const Camera& );
  Point3d pixelToGL( int, int, const Point3d& = Point3d(0.0)) const;
  Vector3d pixelDeltaToGLDelta( int, int, const Point3d& = Point3d(math::MAX_DOUBLE)) const;
  void set( const Point3d&, const Point3d&, const Vector3d& );
  void set( const Point3d&, const Point3d&, const Vector3d&, const Vector3d& );
  void setOrthoProjection(double, double, double);
  void setOrthoProjection(double, double, double, double);
  void setPerspectiveProjection(double, double, double, double, bool = true);
  void setProjection(double, double, double, double, double, double, Mode, bool = true);
  void setOrientation( Orientation );
  void setTransformationToLocal(const Matrix4d&);
  void setTransformationToGlobal(const Matrix4d&);
  void setWindowSize( int, int );
  void setZoom(double);
//  QString toString() const;
void print() const;
  
protected:
	struct ProjectionInfo
  {
  	ProjectionInfo();
    
    double getHeight() const;
    double getWidth() const;    
    
  	double mLeft;
    double mRight;
    double mBottom;
    double mTop;
    double mNear;
    double mFar;
    double mZoomFactor;
    bool mProportionalToWindow;
  };
  
  void computeLatAndUp(); 
  void computeProjection(); 
  double getVisibleHeight() const;
  double getVisibleWidth() const;
  const ProjectionInfo& getProjectionInfo() const {return mProjectionInfo;}
  void setLat( const Vector3d& iLat );  
  void setUp( const Vector3d& iUp );
  
  Mode mMode;
  Orientation mOrientation;
  Matrix4d mToLocal;
  Matrix4d mToGlobal;
  Point3d mPos;
  Vector3d mLat;
  Point3d mLook;
  Vector3d mUp;
  ProjectionInfo mProjectionInfo;
  double mPixelPerGLUnit;
  WindowInfo mWindowInfo;
};

} //treeD
} //realisim

#endif
