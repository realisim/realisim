/*
 *  Camera.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 06/12/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Camera.h"
#include <iostream>
#include <qgl.h>
#include "math/MathUtils.h"

using namespace realisim;
using namespace realisim::treeD;

static const double kNear = 50.0;
static const double kFar = 2000.0;

//-----------------------------------------------------------------------------
Camera::Camera( Mode iMode /*= PERSPECTIVE*/ ) : 
mMode( iMode ),
mOrientation( FREE ),
mToLocal(),
mToGlobal(),
mPos(),
mLat(),
mLook(),
mUp(),
mVisibleGLUnit( 20.0 ),
mPixelPerGLUnit( 0.0 ),
mZoomFactor( 1.0 ),
mWindowInfo()
{
}

//-----------------------------------------------------------------------------
Camera::Camera( const Camera& iCam ) : 
mMode(iCam.getMode()),
mOrientation(iCam.getOrientation()),
mToLocal(iCam.getTransformationToLocal()),
mToGlobal(iCam.getTransformationToGlobal()),
mPos(iCam.getPos()),
mLat(iCam.getLat()),
mLook(iCam.getLook()),
mUp(iCam.getUp()),
mVisibleGLUnit(iCam.mVisibleGLUnit), /*ici, on ne peut pas utiliser la méthode
  getVisibleGLUnit parce que celle ci retourne le visibleGLUnit * zoomFactor,
  ce qui donne le nombre de gl unit visible une fois le zoom appliqué.*/
mPixelPerGLUnit(iCam.getPixelPerGLUnit()),
mZoomFactor(iCam.getZoom()),
mWindowInfo(iCam.getWindowInfo())
{
}

//-----------------------------------------------------------------------------
Camera::~Camera()
{
}

//-----------------------------------------------------------------------------
//void Camera::computeLatAndUp()
//{
//  //on commence par calculer le vecteur latérale parce que le vecteur up
//  //final est dependant du vecteur lateral
//  
//  Vector3d lookVect( getPos(), getLook()  );
//  
//  mLat = lookVect ^ getUp();
//  mLat.normalise();
//  
//  mUp = getLat() ^lookVect;
//  mUp.normalise();
//}

//-----------------------------------------------------------------------------
/*déplace la caméra. Le delta est en coordonnée GL et locale à la caméra.*/
void Camera::move( const Vector3d& iDelta )
{
  switch ( getOrientation() ) 
  {
    case XY:
    case ZY:
    case XZ:
    {
      mPos += toPoint(iDelta);
      mLook += toPoint(iDelta);
    }
    break;
      
    case FREE:
    {
      //un delta de mVisiblGLUnit represente 360 degrée de rotation...
      //donc 2*Pi Rad
      
      //Le vecteur iDelta est un vecteur qui est dans le plan Lat-Up
      //On projecte le vecteur iDelta sur chacune des composentes du plan
      //afin de déterminer l'angle de ratation pour chaque axe
      
      //La projection sur l'axe laterale reprentera la rotation par rapport
      //a l'axe vertical (y) de la base
      Vector3d projDeltaOnLat = getLat() * ( getLat() & iDelta );
      
      //ROTATION PAR RAPPORT A LA COMPOSANTE Y DE LA TRANSFO DE LA CAMERA
      double angle1 = projDeltaOnLat.norm() * DEUX_PI / getVisibleGLUnit();
            
      //on test pour voir si le vecteur iDelta projeté sur le vecteur latérale
      //est dans le même sens que ce dernier ou non... Si ce n'est pas le
      //cas, on change la direction de l'angle.
      if( ( projDeltaOnLat + (getLat() * projDeltaOnLat.norm())) ==
            Vector3d(0.0) )
      {
        angle1 *= -1;
      }
      
      //y de la transfo de la camera
      Vector3d upRotAxis = Vector3d( 0.0, 1.0, 0.0 );//mTransformation.getBaseY();
      
      //on effectur la rotation autour de l'axe Up
      Point3d newPos = 
        rotatePoint( angle1,
                  getPos(),
                  upRotAxis,
                  getLook() );
      
      //on calcul le nouveau vecteur lateral. Ceci permet de controler un des
      //axes de la camera. Ceci empêche la caméra de basculer sur les cotés
      //ce qui donne comme effet que la caméra garde la tête droite. En controlant
      //le vecteur latéral, on controle aussi le vecteur up.
      
      //un point a l'extrémité du vecteur latéral
      Point3d latPos = getPos() + getLat();
      //on le fait tourner
      latPos = 
        rotatePoint( angle1,
                     latPos,
                     upRotAxis,
                     getLook() );
      
      //on crée le nouveau vecteur latérale
      Vector3d newLat( newPos, latPos );
      
      //on projete le nouveau vecteur latérale sur la composante x et z de
      //la transfo, ce qui a pour effet de garder la caméra stable. On ne peut
      //pas directement seter le vecteur up parce que ce dernier varie dans 2
      //plan distincts
      
      //x et z de la transfo
      Vector3d x = Vector3d(1.0, 0.0, 0.0);//mTransformation.getBaseX();
      Vector3d z = Vector3d(0.0, 0.0, 1.0);//mTransformation.getBaseZ();

      Vector3d projLatOnX = x * ( x & newLat );
      Vector3d projLatOnZ = z * ( z & newLat );
      Vector3d projXZ = projLatOnX + projLatOnZ;
      setLat( projXZ );
 
      
      //ROTATION PAR RAPPORT AU VECTEUR LAT
      //La projection de iDelta sur l'axe Up représente la rotation autour
      //de l'axe latérale
      Vector3d projDeltaOnUp = getUp() * ( getUp() & iDelta );

      double angle2 = projDeltaOnUp.norm() * DEUX_PI / getVisibleGLUnit() * -1;
            
      //on test pour voir si le vecteur iDelta projeté sur le vecteur latérale
      //est dans le même sens que ce dernier ou non... Si ce n'est pas le
      //cas, on change la direction de l'angle.
      if( ( projDeltaOnUp + ( getUp() * projDeltaOnUp.norm() ) ) ==
         Vector3d(0.0) )
      {
        angle2 *= -1;
      }

      //On effectue la rotation autour de l'axe latérale
      newPos =
        rotatePoint( angle2,
                     newPos,
                     getLat(),
                     getLook() );
      
      //on applique la nouvelle position.
      mPos = newPos;
    }
      break;
      
    default:
      break;
  }
}

//-----------------------------------------------------------------------------
Camera&
Camera::operator=( const Camera& iCam )
{
  mMode = iCam.getMode();
  mOrientation = iCam.getOrientation();
  mToLocal = iCam.getTransformationToLocal();
  mToGlobal = iCam.getTransformationToGlobal();
  mPos = iCam.getPos();
  mLat = iCam.getLat();
  mLook = iCam.getLook();
  mUp = iCam.getUp();
  mVisibleGLUnit = iCam.mVisibleGLUnit; /*ici, on ne peut pas utiliser la méthode
  getVisibleGLUnit parce que celle ci retourne le visibleGLUnit * zoomFactor,
  ce qui donne le nombre de gl unit visible une fois le zoom appliqué.*/
  mPixelPerGLUnit = iCam.getPixelPerGLUnit();
  mZoomFactor = iCam.getZoom();
  mWindowInfo = iCam.getWindowInfo();
  
  return *this;
}

//-----------------------------------------------------------------------------
//Convertie une position pixel a l'écran en coordonnée GL
//Les paramètres sont en pixels
Point3d Camera::pixelToGL( int iX, int iY ) const
{
  assert(0);
  return(Point3d());
  //la fonction était incorrecte et n'est pas en utilisation pour l'instant...
  //il faudrait aussi prendre la transformation en compte.
  
//  int width = getWindowInfo().getWidth();
//  int heigth = getWindowInfo().getHeight();
//  
//  Point3d result = getPos() - ( getLat() * (width/2.0) / mPixelPerGLUnit ) + 
//    ( getUp() * (heigth/2.0) / mPixelPerGLUnit ) +
//    getLat() * iX / mPixelPerGLUnit +
//    getUp() * iY / mPixelPerGLUnit;
//    
//  /*TODO: Pour la camera perspective, voire la méthode pixelDeltaToGLDelta et
//    s'en inspirer pour faire celle-ci*/
//  
//  return result;
}

//-----------------------------------------------------------------------------
/*Convertie une delta en pixel d'écran en delta GL. Le deltaGL sera en
  coordonnée locale.
  Les paramètres sont en pixels. Le Point3d répresente un point sur le
  plan de travail qui est perdiculaire a la caméra. On a besoin de ce point
  afin de déterminer, dans la vue de perspective, la profodeur à laquelle on
  veut le deltaGL.
  NOTE: le Point3d doit être en coordonnée locale et non globale.*/
Vector3d Camera::pixelDeltaToGLDelta( int iDeltaX, int iDeltaY,
	const Point3d& iPoint /*= Point3d(MAX_DOUBLE)*/ ) const
{
  Vector3d result(0.0);
  if(getMode() == ORTHOGONAL ||
     (iPoint == Point3d(MAX_DOUBLE)))
  {
    result = getLat() * iDeltaX / mPixelPerGLUnit +
      getUp() * iDeltaY / mPixelPerGLUnit;
  }
  else //PERSPECTIVE
  {
    double modelView[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
    double projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    double winx, winy, winz, p0x, p0y, p0z, p1x, p1y, p1z;
    
    /*on transforme le point en coordonné global afin de faire les calculs de
      projection*/
    Point3d p = iPoint * getTransformationToGlobal();

    /*A partir du Point3d qui represente le point sur le plan de travail
      perpendiculaire a la camera, on le projete a l'écran. Ce qui nous
      intéresse c'est la coordonnée en z normalisé qui nous donne la profondeur
      de ce point3d dans le zBuffer qui sera utilisé pour trouver le deltaGL*/
    gluProject(p.getX(), p.getY(), p.getZ(),
      modelView, projection, viewport,
      &winx, &winy, &winz);
      
    /*On projete le point d'écran 0.0, 0.0, winz qui nous donnera un point (p0)
      qui correspond au point d'écran (0.0, 0.0) a la profondeur du plan de
      travail*/
    gluUnProject(0.0, 0.0, winz,
      modelView, projection, viewport,
      &p0x, &p0y, &p0z);
      
    /*On projete le point d'écran iDeltax, iDeltaY, winz qui nous donnera un
      point (p1) qui correspond au point d'écran (iDeltaX, iDeltaY) a la
      profondeur du plan de travail. Ainsi, avec ces deux points 3d qui
      représente le delta pixel en coordonnée GL a la profondeur du plan de
      travail*/
    gluUnProject(iDeltaX, iDeltaY, winz,
      modelView, projection, viewport,
      &p1x, &p1y, &p1z);
    
    /*on met les points dans le systeme de coordonné local. La transformation
      de la camera est du systeme local vers le system global. On a donc besoin
      de la transformation inverse pour mettre les points globaux dans le 
      systeme local.*/
    Matrix4d sceneToLocal = getTransformationToLocal();
    Vector3d p0 = Vector3d(p0x, p0y, p0z) * sceneToLocal;
    Vector3d p1 = Vector3d(p1x, p1y, p1z) * sceneToLocal;
    Vector3d glDelta = p1 - p0;
    
    //projection de glDelta sur lateral
    Vector3d projGlDeltaOnLat = getLat() * ( getLat() & glDelta );
    //projection de glDelta sur up
    Vector3d projGlDeltaOnUp = getUp() * ( getUp() & glDelta );
    result = getLat() * iDeltaX/(abs(iDeltaX)) * projGlDeltaOnLat.fastNorm() + 
      getUp() * iDeltaY/(abs(iDeltaY)) * projGlDeltaOnUp.fastNorm();
  }

  
  return result;
}

//-----------------------------------------------------------------------------
void Camera::projectionGL( int iWidth, int iHeight )
{
  if(iWidth >= iHeight)
    mWindowInfo.mOrientation = WindowInfo::oHorizontal;
  else
    mWindowInfo.mOrientation = WindowInfo::oVertical;
  
  //le coté le plus long montre la valeur mVisibleGLUnit unité GL
  mWindowInfo.mShortSide = qMin(iWidth, iHeight);
  mWindowInfo.mLongSide = qMax(iWidth, iHeight);
  
  computeProjection();
}
//-----------------------------------------------------------------------------
void Camera::computeProjection()
{
  int windowLongSide = getWindowInfo().mLongSide;
  int windowShortSide = getWindowInfo().mShortSide;
  float projectionLongSide = getVisibleGLUnit();
  mPixelPerGLUnit = windowLongSide / projectionLongSide;
  float projectionShortSide = windowShortSide * projectionLongSide / 
    windowLongSide; 
  
  //puisque le viewport est de -projectionLongSide a projectionLongSide et
  //que projectionLongSide represente mVisibleUnitGL, on doit diviser en 2 afin de 
  //conserver les proportions.
  projectionShortSide /= 2.0;
  projectionLongSide /= 2.0;
  
  if( getWindowInfo().mOrientation == WindowInfo::oHorizontal )
  {
    glViewport(0, 0, windowLongSide, windowShortSide );
    
    switch ( getMode() ) 
    {
      case ORTHOGONAL:
        glOrtho(-projectionLongSide, projectionLongSide,
                -projectionShortSide, projectionShortSide, 
                kNear, kFar);
        break;
        
      case PERSPECTIVE:
        gluPerspective (27.0,
          (GLfloat) projectionLongSide / (GLfloat) projectionShortSide,
          0.5,
          1000.0);
        break;
      default:
        break;
    }
  }
  else //vertical
  {
    glViewport(0, 0, windowShortSide, windowLongSide );
    
    switch ( getMode() ) 
    {
      case ORTHOGONAL:
        glOrtho(-projectionShortSide, projectionShortSide,
                -projectionLongSide, projectionLongSide, 
                kNear, kFar);
        break;
        
      case PERSPECTIVE:
        gluPerspective (27.0,
          (GLfloat) projectionShortSide / (GLfloat) projectionLongSide,
          0.5,
          1000.0);
        break;
        
      default:
        break;
    }   
  }
}

//-----------------------------------------------------------------------------
/*Définie la position de la caméra, le point visé et le vecteur up. Le 
  vecteur Latéral sera calculé à partir du vecteur up et ensuite le up
  sera recalculé afin d'assurer une base normale.*/
void Camera::set( const Point3d& iPos,
         const Point3d& iLook,
         const Vector3d& iUp )
{
  mPos = iPos;
  mLook = iLook;
  mUp = iUp;
  
  //on commence par calculer le vecteur laterale parce que le vecteur up
  //final est dependant du vecteur lateral
  Vector3d lookVect( getPos(), getLook()  );
  
  mLat = lookVect ^ getUp();
  mLat.normalise();
  
  mUp = getLat() ^lookVect;
  mUp.normalise();
}

//-----------------------------------------------------------------------------
void Camera::setLat( const Vector3d& iLat )
{
  mLat = iLat;
  mLat.normalise();
  
  mUp = ( mLat ^ Vector3d( getPos(), getLook() ) ).normalise();
}

//-----------------------------------------------------------------------------
void Camera::setMode( Mode iMode )
{ mMode = iMode; }

//-----------------------------------------------------------------------------
void Camera::setPos( const Point3d& iPos)
{ set(iPos, getLook(), getUp()); }

//-----------------------------------------------------------------------------
void Camera::setOrientation( Orientation iO )
{
  mOrientation = iO;

  switch( mOrientation )
  {
    case XY:
      set( Point3d( 0, 0, 100 ),
           Point3d( 0, 0, 0 ),
           Vector3d( 0, 1, 0 ) );
      break;
    case ZY:
      set( Point3d( 100, 0, 0 ),
           Point3d( 0, 0, 0 ),
           Vector3d( 0, 1, 0 ) );
      break;
    case XZ:
      set( Point3d( 0, 100, 0 ),
           Point3d( 0, 0, 0 ),
           Vector3d( 0, 0, -1 ) );
      break;
    case FREE:
      set( Point3d( 60, 60, 60 ),
           Point3d( 0, 0, 0 ),
           Vector3d( 0, 1, 0 ) );
      break;
    default:
      break;
  }
}

//-----------------------------------------------------------------------------
void Camera::setTransformationToLocal(const Matrix4d& iTransfo)
{
  mToLocal = iTransfo;
  mToGlobal = mToLocal;
  mToGlobal.inverse();
}

//-----------------------------------------------------------------------------
void Camera::setTransformationToGlobal(const Matrix4d& iTransfo)
{
  mToGlobal = iTransfo;
  mToLocal = mToGlobal;
  mToLocal.inverse();
}

//-----------------------------------------------------------------------------
void Camera::setUp( const Vector3d& iUp )
{
   mUp = iUp;

  Vector3d currentLookVect( getPos(), getLook()  );
  
  mLat = currentLookVect ^ getUp();
  mLat.normalise();
  
  Vector3d updatedLookVect = getUp() ^ getLat();
  mPos = getLook() - ( updatedLookVect.normalise() * currentLookVect.norm() );
}

//-----------------------------------------------------------------------------
void Camera::setZoom(double iZoom)
{
  mZoomFactor = iZoom;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  computeProjection();
  glMatrixMode(GL_MODELVIEW);
}
