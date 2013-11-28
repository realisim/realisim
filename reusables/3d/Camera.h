
#ifndef Realisim_E3d_Camera_hh
#define Realisim_E3d_Camera_hh

#include "math/Matrix4x4.h"
#include "math/Point.h"
#include "math/Vect.h"

/*
	-- struct
  Projection
    mLeft;
    mRight;
    mBottom;
    mTop;
    mNear;
    mFar;
    mZoomFactor: Facteur multiplicateur sur mVisibleGLUnit.
    mProportional:

  -- membres
  mOrientation: Orientation de la camera
mTransformation: transformation appliqué au systeme d'axe de la caméra.
    cette transformation est: syteme local à system global. Le Widget 3d
    utilisera cette transformation pour transformer la position de la caméra
    en position global (de scene).
  mPos: position de la caméra..
  mLat: vecteur latéral normalisé
  mLook:  point visé.
  mUp: vecteur up normalisé.
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
  enum Orientation{ XY, ZY, XZ, FREE };
  
  //windowInfo devrait etre remplacé par viewport...
  struct WindowInfo
  {
    WindowInfo() : mLongSide(1), mShortSide(1){;}
    enum Orientation { oHorizontal, oVertical };
    int getHeight() const{return mOrientation == oVertical ? mLongSide : mShortSide;}    
    int getWidth() const{return mOrientation == oHorizontal ? mLongSide : mShortSide;}
    Vector2i getSize() const { return Vector2i( getWidth(), getHeight() ); }

    Orientation mOrientation;
    int mLongSide;
    int mShortSide;
  };
  
  struct Projection
  {
  	Projection();
    enum type{ tOrthogonal = 0, tPerspective };
    
    double getHeight() const;
    double getWidth() const;  
    Vector2d getSize() const { return Vector2d( getWidth(), getHeight() ); }  
    
  	double mLeft;
    double mRight;
    double mBottom;
    double mTop;
    double mNear;
    double mFar;
    double mZoomFactor;
    bool mProportionalToWindow;
    type mType;
  };


  Camera();
  Camera( const Camera& iCam );
  virtual ~Camera();
  
  void applyModelViewTransformation() const;
  void applyProjectionTransformation() const;
  const Vector3d& getLat() const { return mLat; }
  const Point3d& getLook() const { return mLook; }
  Orientation getOrientation() const { return mOrientation; }
  const Point3d& getPos() const { return mPos; }
  const double getPixelPerGLUnit() const { return mPixelPerGLUnit; }
  const Projection& getProjection() const {return mProjectionInfo;}
  const Matrix4d& getProjectionMatrix() const;
  const Matrix4d& getTransformationToLocal() const { return mToLocal; }
  const Matrix4d& getTransformationToGlobal() const { return mToGlobal; }
  const Vector3d& getUp() const { return mUp; }
  const Matrix4d& getViewMatrix() const;
  double getVisibleHeight() const;
  double getVisibleWidth() const;
  const WindowInfo& getWindowInfo() const {return mWindowInfo;}
  Point2d glToPixel( const Point3d& ) const;
  const double getZoom() const { return mProjectionInfo.mZoomFactor; }
  void move( const Vector3d& );
  Camera& operator=( const Camera& );
  Point3d pixelToGL( int, int, const Point3d& = Point3d(0.0)) const;
  Vector3d pixelDeltaToGLDelta( int, int, const Point3d& = Point3d(math::MAX_DOUBLE)) const;
  void popMatrices() const;
  void pushAndApplyMatrices() const;  
  void set( const Point3d&, const Point3d&, const Vector3d& );
  void setOrthoProjection(double, double, double);
  void setOrthoProjection(double, double, double, double);
  void setPerspectiveProjection(double, double, double, double, bool = true);
  void setProjection( const Projection& );
  void setProjection(double, double, double, double, double, double, Projection::type, bool = true);
  void setOrientation( Orientation );
  void setTransformationToLocal(const Matrix4d&);
  void setTransformationToGlobal(const Matrix4d&);
  void setWindowSize( int, int );
  void setWindowSize( Vector2i );
  void setZoom(double);
//  QString toString() const;
void print() const;
  
protected:  
  void computeLatAndUp(); 
  void computeProjection(); 
  void computeViewMatrix();
  virtual const Vector3d& getLookVector() const;
  void setLat( const Vector3d& iLat );  
  void setPos( const Point3d& );
  void setUp( const Vector3d& iUp );
  
  Orientation mOrientation;
  Matrix4d mToLocal;
  Matrix4d mToGlobal;
  Point3d mPos;
  Vector3d mLat;
  Point3d mLook;
  Vector3d mLookVector;
  Vector3d mUp;
  Projection mProjectionInfo;
  double mPixelPerGLUnit;
  WindowInfo mWindowInfo;
  mutable Matrix4d mProjectionMatrix;
  mutable Matrix4d mViewMatrix;
};

} //treeD
} //realisim

#endif
