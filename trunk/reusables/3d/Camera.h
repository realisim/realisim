
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
  mVisibleGLUnit: Sert a définir le nombre d'unité GL minimale visible  dans le
    viewPort. Par exemple, si on met 20, le viewport sera une fenêtre sur un
    rectangle d'ou moins 20 unités GL sur son coté le plus long. C'est en
    modifiant ce paramètre qu'on peut zoomer ou dezoomer. Plus le chiffre est
    gros plus on voit de la scene donc moins on est zoomé.
  mPixelPerGLUnit: le rapport entre les pixel d'écran et les unité GL.
  mZoomFactor: Facteur multiplicateur sur mVisibleGLUnit.
  mWindowInfo: Information sur la fenetre. Utile afin de calculer la
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
  
  enum Mode{ ORTHOGONAL = 0, PERSPECTIVE };
  enum Orientation{ XY, ZY, XZ, FREE };
  struct WindowInfo
  {
    WindowInfo(){;}
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
  const double getZoom() const { return mZoomFactor; }
  void move( const Vector3d& );
  Camera& operator=( const Camera& );
  Point3d pixelToGL( int, int ) const;
  Vector3d pixelDeltaToGLDelta( int, int, const Point3d& = Point3d(math::MAX_DOUBLE)) const;
  void projectionGL( int, int );
  void set( const Point3d&, const Point3d&, const Vector3d& );
  void set( const Point3d&, const Point3d&, const Vector3d&, const Vector3d& );
  void setMode( Mode );
  void setPos(const Point3d&);
  void setLook(const Point3d&);
  void setOrientation( Orientation );
  void setTransformationToLocal(const Matrix4d&);
  void setTransformationToGlobal(const Matrix4d&);
  void setZoom(double);
//  QString toString() const;
void print() const;
  
protected:
  void computeLatAndUp();
  void computeProjection();
  double getVisibleGLUnit() const;
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
  double mVisibleGLUnit;
  double mPixelPerGLUnit;
  double mZoomFactor;
  WindowInfo mWindowInfo;
};

} //treeD
} //realisim

#endif
