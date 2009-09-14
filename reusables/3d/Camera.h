/*
 *  Camera.h
 */

#ifndef Realisim_E3d_Camera_hh
#define Realisim_E3d_Camera_hh

#include "Matrix4x4.h"
#include "Point.h"
#include "Vect.h"

/*
  --members
  mMode: mode de projection, Orthogonal ou Perspective
  mOrientation: Orientation de la camera
  mTransformation: transformation appliqué au systeme d'axe de la caméra.
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
  const Matrix4d& getTransformation() const { return mTransformation; }
  const Vector3d& getUp() const { return mUp; }
  const WindowInfo& getWindowInfo() const {return mWindowInfo;}
  const double getZoom() const { return mZoomFactor; }
  void move( Vector3d iDelta );
  Camera& operator=( const Camera& iCam );
  Point3d pixelToGL( int iX, int iY ) const;
  Vector3d pixelDeltaToGLDelta( int iDeltaX, int iDeltaY ) const;
  void projectionGL( int iWidth, int iHeight );
  void set( const Point3d& iPos,
            const Point3d& iLook,
            const Vector3d& iUp );
  void setMode( Mode iMode );
  void setOrientation( Orientation iO );
  void setTransformation(const Matrix4d&);
  void setZoom(double iZoom);
  
protected:
  void computeLatAndUp();
  void computeProjection();
  const double getVisibleGLUnit() const { return mVisibleGLUnit * mZoomFactor; }
  void setLat( const Vector3d& iLat );
  void setUp( const Vector3d& iUp );
  
  Mode mMode;
  Orientation mOrientation;
  Matrix4d mTransformation;
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
