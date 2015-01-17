
#ifndef Realisim_E3d_Camera_hh
#define Realisim_E3d_Camera_hh

#include "math/Matrix4.h"
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
  mPos: position de la caméra..
  mLat: vecteur latéral normalisé
  mLook:  point visé.
  mUp: vecteur up normalisé.
  mViewport: Information sur la fenetre. Utile afin de calculer la
    matrice de projection.
    -mOrientation: indique quel coté de la fenêtre est le plus long
    -mShortSide: taille en pixel du petit coté
    -mLongSide: taille en pixel du long coté
*/
namespace realisim
{
namespace treeD
{
using namespace realisim::math;

class Camera
{
public:  
  struct Viewport
  {
    Viewport() : mLongSide(1), mShortSide(1){;}
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
//Je garde intentionnellement le constructeur copie et l'operateur par defaut
//pour des raisons d'optimisation du compilateur.
//  Camera( const Camera& iCam );
//  Camera& operator=( const Camera& );
  virtual ~Camera();
  
  void applyModelViewTransformation() const;
  void applyProjectionTransformation() const;
  Point3d cameraToWorld( const Point3d& ) const;
  Vector3d cameraToWorld( const Vector3d& ) const;
  const Vector3d& getLat() const { return mLat; }
  const Point3d& getLook() const { return mLook; }
  const Point3d& getPos() const { return mPos; } 
  const Projection& getProjection() const {return mProjectionInfo;}
  const myMatrix4& getProjectionMatrix() const;
  const Vector3d& getUp() const { return mUp; }
  const myMatrix4& getViewMatrix() const;
  double getVisibleHeight() const;
  double getVisibleWidth() const;
  const Viewport& getViewport() const {return mViewport;}  
  const double getZoom() const { return mProjectionInfo.mZoomFactor; }
  void popMatrices() const;
  void pushAndApplyMatrices() const;  
  void rotate( double, Vector3d, Point3d = Point3d() );
	Point3d screenToWorld( Point2d, const Point3d& = Point3d(0.0)) const;
	Vector3d screenToWorld( Vector2d, const Point3d& = Point3d(0.0)) const;
  void set( const Point3d&, const Point3d&, const Vector3d& );
  void setOrthoProjection(double, double, double);
  void setOrthoProjection(double, double, double, double);
  void setPerspectiveProjection(double, double, double, double, bool = true);
  void setProjection( const Projection& );
  void setProjection(double, double, double, double, double, double, Projection::type, bool = true);
  void setViewportSize( int, int );
  void setViewportSize( Vector2i );
  void setZoom(double);
  void translate( const Point3d& );
  void translate( const Vector3d& );
  Point3d worldToCamera( const Point3d& ) const;
  Vector3d worldToCamera( const Vector3d& ) const;
  Point2d worldToSreen( const Point3d& ) const;
  Vector2d worldToSreen( const Vector3d& ) const;

//  QString toString() const;
void print() const;
  
protected:  
  void computeProjection(); 
  void computeViewMatrix();
  const Vector3d& getLookVector() const;
  
  Point3d mPos;
  Vector3d mLat;
  Point3d mLook;
  Vector3d mLookVector;
  Vector3d mUp;
  Projection mProjectionInfo;
  Viewport mViewport;
  mutable myMatrix4 mProjectionMatrix;
  mutable myMatrix4 mViewMatrix;
};

} //treeD
} //realisim

#endif
