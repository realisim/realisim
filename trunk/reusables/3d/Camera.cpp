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

//-----------------------------------------------------------------------------
//--- Camera::Projection
//-----------------------------------------------------------------------------
Camera::Projection::Projection() : mLeft(-10.0),
  mRight(10.0),
  mBottom(0.0),
  mTop(0.0),
  mNear(0.5),
  mFar(2000.0),
  mZoomFactor( 1.0 ),
  mProportionalToWindow(true),
  mType( tPerspective )
{}

double Camera::Projection::getHeight() const
{ return mTop - mBottom; }

double Camera::Projection::getWidth() const
{ return mRight - mLeft; }

//-----------------------------------------------------------------------------
//--- Camera
//-----------------------------------------------------------------------------
Camera::Camera() : 
mOrientation( FREE ),
mToLocal(),
mToGlobal(),
mPos(),
mLat(),
mLook(),
mUp(),
mProjectionInfo(),
mPixelPerGLUnit( 0.0 ),
mWindowInfo()
{
	setOrientation( FREE );
  setPerspectiveProjection(60, 1, 0.5, 1000.0, true);
}

//-----------------------------------------------------------------------------
Camera::Camera( const Camera& iCam ) : 
mOrientation(iCam.getOrientation()),
mToLocal(iCam.getTransformationToLocal()),
mToGlobal(iCam.getTransformationToGlobal()),
mPos(iCam.getPos()),
mLat(iCam.getLat()),
mLook(iCam.getLook()),
mUp(iCam.getUp()),
mProjectionInfo(iCam.getProjection()),
mPixelPerGLUnit(iCam.getPixelPerGLUnit()),
mWindowInfo(iCam.getWindowInfo())
{
}

//-----------------------------------------------------------------------------
Camera::~Camera()
{
}

//-----------------------------------------------------------------------------
void Camera::applyModelViewTransformation() const
{    
  //On place la caméra en coordonnée absolue.
  Point3d absolutePos = getPos() * getTransformationToGlobal();
  Point3d absoluteLook = getLook() * getTransformationToGlobal();
  Vector3d absoluteUp = getUp() * getTransformationToGlobal();
  
    
  gluLookAt( absolutePos.getX(),
             absolutePos.getY(), 
             absolutePos.getZ(),
             absoluteLook.getX(),
             absoluteLook.getY(),
             absoluteLook.getZ(),
             absoluteUp.getX(),
             absoluteUp.getY(),
             absoluteUp.getZ() );
             
    //on stock la matrice de projection
  glGetDoublev(GL_MODELVIEW_MATRIX, mViewMatrix.getNonConstPtr() );
//  glLoadMatrix( mViewMatrix.getPtr() );
}

//-----------------------------------------------------------------------------
void Camera::applyProjectionTransformation() const
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
    
  if( getWindowInfo().mOrientation == WindowInfo::oHorizontal )
  {
    glViewport(0, 0, getWindowInfo().getWidth(), getWindowInfo().getHeight() );
  }
  else //vertical
  {
    glViewport(0, 0, getWindowInfo().getWidth(), getWindowInfo().getHeight() );
  }

	/*Afin d'appliquer correctement le zoom, qui est donné par getVisibleWidth
    et getVisibleHeight, on doit trouver le centre de la projection
    originale (sans zoom). Ensuite on détermine la taille du rectangle
    de la projection avec le zoom a partir de ce centre.*/
  const Projection& p = getProjection();
  double halfVisibleWidth = getVisibleWidth() / 2.0;
  double halfVisibleHeigh = getVisibleHeight() / 2.0;
  double cx = 0.0, cy = 0.0, l, r, b, t;
  cx = p.mLeft + p.getWidth() / 2.0;
  cy = p.mBottom + p.getHeight() / 2.0; 
  l = cx - halfVisibleWidth;
  r = cx + halfVisibleWidth;
  b = cy - halfVisibleHeigh;
  t = cy + halfVisibleHeigh;

  switch ( p.mType ) 
  {
    case Projection::tOrthogonal: glOrtho( l, r, b, t, p.mNear, p.mFar); break;
    case Projection::tPerspective: glFrustum( l, r, b, t, p.mNear, p.mFar); break;
    default: break;
  }
  
  //on stock la matrice de projection
  glGetDoublev(GL_PROJECTION_MATRIX, mProjectionMatrix.getNonConstPtr() );
  
  glMatrixMode(GL_MODELVIEW);
}

//-----------------------------------------------------------------------------
void Camera::computeProjection()
{
	const Projection& p = getProjection();
  if(p.mProportionalToWindow)
  {
    /*Quand on veut une projection proportionnelle, on commence par trouver
    la hauteur (h) proportionnel a la largeur (w) courante. Ensuite, on
    trouve le centre (cy) de la projection et y ajoute la moitié de h
    de part et d'autre.*/
  	double w = getWindowInfo().getWidth() / p.getWidth();
    double h = getWindowInfo().getHeight() / w / 2.0;
    double cy = p.mBottom + p.getHeight() / 2.0; 
    mProjectionInfo.mTop = cy + h;
    mProjectionInfo.mBottom = cy - h;
  }
    
  if( getWindowInfo().mOrientation == WindowInfo::oHorizontal )
  {
    mPixelPerGLUnit = getWindowInfo().getWidth() /  getVisibleWidth();
  }
  else //vertical
  {
    mPixelPerGLUnit = getWindowInfo().getHeight() /  getVisibleHeight();
  }

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
const Matrix4d& Camera::getViewMatrix() const
{ return mViewMatrix; }

//-----------------------------------------------------------------------------
const Matrix4d& Camera::getProjectionMatrix() const
{ return mProjectionMatrix; }

//-----------------------------------------------------------------------------
/*retourne la largeur visible en unité GL*/
double Camera::getVisibleHeight() const
{ return mProjectionInfo.getHeight() * 1.0 / getZoom(); }

//-----------------------------------------------------------------------------
double Camera::getVisibleWidth() const
{ return mProjectionInfo.getWidth() * 1.0 / getZoom(); }

//-----------------------------------------------------------------------------
Point2d Camera::glToPixel( const Point3d& iP ) const
{
  pushProjections();
  
  double modelView[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
  double projection[16];
  glGetDoublev(GL_PROJECTION_MATRIX, projection);
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  double x, y, z;
	bool sucess = gluProject(iP.getX(), iP.getY(), iP.getZ(), 
    modelView, 
    projection, 
    viewport, 
    &x, &y, &z);
  if ( !sucess ) { x = 0; y = 0; z = 0; }

  popProjections();

	//dépendant de l'orientation de la camera?
	return Point2d( x, y );
}

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
    	double visibleGlUnit = 0.0;
      getWindowInfo().mOrientation == WindowInfo::oHorizontal ? 
        visibleGlUnit = getVisibleWidth() : visibleGlUnit = getVisibleHeight();
      //un delta de mVisiblGLUnit represente 360 degrée de rotation...
      //donc 2*Pi Rad
      
      //Le vecteur iDelta est un vecteur qui est dans le plan Lat-Up
      //On projecte le vecteur iDelta sur chacune des composentes du plan
      //afin de déterminer l'angle de ratation pour chaque axe
      
      //La projection sur l'axe laterale reprentera la rotation par rapport
      //a l'axe vertical (y) de la base
      Vector3d projDeltaOnLat = getLat() * ( getLat() * iDelta );
      
      //ROTATION PAR RAPPORT A LA COMPOSANTE Y DE LA TRANSFO DE LA CAMERA
      double angle1 = projDeltaOnLat.norm() * DEUX_PI / visibleGlUnit;
            
      //on test pour voir si le vecteur iDelta projeté sur le vecteur latérale
      //est dans le même sens que ce dernier ou non... Si ce n'est pas le
      //cas, on change la direction de l'angle.
      if( math::isEqual( projDeltaOnLat + getLat() * projDeltaOnLat.norm(),
            Vector3d(0.0) ) )
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

      Vector3d projLatOnX = x * ( x * newLat );
      Vector3d projLatOnZ = z * ( z * newLat );
      Vector3d projXZ = projLatOnX + projLatOnZ;
      setLat( projXZ );
 
      
      //ROTATION PAR RAPPORT AU VECTEUR LAT
      //La projection de iDelta sur l'axe Up représente la rotation autour
      //de l'axe latérale
      Vector3d projDeltaOnUp = getUp() * ( getUp() * iDelta );

      double angle2 = projDeltaOnUp.norm() * DEUX_PI / visibleGlUnit * -1;
            
      //on test pour voir si le vecteur iDelta projeté sur le vecteur latérale
      //est dans le même sens que ce dernier ou non... Si ce n'est pas le
      //cas, on change la direction de l'angle.
      if( math::isEqual( projDeltaOnUp + getUp() * projDeltaOnUp.norm(),
         Vector3d(0.0) ) )
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
  mOrientation = iCam.getOrientation();
  mToLocal = iCam.getTransformationToLocal();
  mToGlobal = iCam.getTransformationToGlobal();
  mPos = iCam.getPos();
  mLat = iCam.getLat();
  mLook = iCam.getLook();
  mUp = iCam.getUp();
  mProjectionInfo = iCam.getProjection();
  mPixelPerGLUnit = iCam.getPixelPerGLUnit();
  mWindowInfo = iCam.getWindowInfo();
  
  return *this;
}

//-----------------------------------------------------------------------------
/*Convertie une position pixel a l'écran en coordonnée GL.
  Les paramètres sont en pixels. Puisque le systeme de fenetrage est Qt, 
  contrairement a openGL, l'origine (0, 0) est dans le coin supérieur gauche.
  Ainsi la coordonné y est inversée. Le point converti sera en  coordonné
  globale. iPoint doit être en coordonné locale de la camera. Ce point
  représente la position d'un plan, parallele à la camera. Le Point3d retourné
  par cette fonction est donc l'intersection de la projection du point iX, iY
  avec ce plan. La projection de caméra est tenue en compte.
  Note: cette methode applique les projections de projection et de modelview
    afin de faire les calculs.*/
Point3d Camera::pixelToGL( int iX, int iY,
  const Point3d& iPoint /*= Point3d(0.0)*/ ) const
{
  //l'axe de Qt est inversé par rapport a openGL
  iY = getWindowInfo().getHeight() - iY;
  
  pushProjections();
  
  double modelView[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
  double projection[16];
  glGetDoublev(GL_PROJECTION_MATRIX, projection);
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  double winx, winy, winz, p0x, p0y, p0z;

  /*on transforme le point en coordonné global afin de faire les calculs de
    projection*/
  Point3d p = iPoint * getTransformationToGlobal();

  /*A partir du Point3d qui represente le point sur le plan de travail
    perpendiculaire a la camera, on le projete a l'écran. Ce qui nous
    intéresse c'est la coordonnée en z normalisé qui nous donne la profondeur
    de ce point3d dans le zBuffer qui sera utilisé pour trouver le point 
    en coordonnée GL*/
  gluProject(p.getX(), p.getY(), p.getZ(),
    modelView, projection, viewport,
    &winx, &winy, &winz);
    
  /*On projete le point d'écran iX, iY, winz qui nous donnera un point (p0)
    qui correspond au point d'écran (iX, iY) a la profondeur du plan de
    travail. */
  gluUnProject(iX, iY, winz,
    modelView, projection, viewport,
    &p0x, &p0y, &p0z);

	popProjections();
  return Point3d(p0x, p0y, p0z);
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
  if(getProjection().mType == Projection::tOrthogonal ||
     (iPoint == Point3d(MAX_DOUBLE)))
  {
    result = getLat() * iDeltaX / mPixelPerGLUnit +
      getUp() * iDeltaY / mPixelPerGLUnit;
  }
  else //Projection::tPerspective
  {
  
    pushProjections();

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
    gluUnProject(winx, winy, winz,
      modelView, projection, viewport,
      &p0x, &p0y, &p0z);
      
    /*On projete le point d'écran iDeltax, iDeltaY, winz qui nous donnera un
      point (p1) qui correspond au point d'écran (iDeltaX, iDeltaY) a la
      profondeur du plan de travail. Ainsi, avec ces deux points 3d qui
      représente le delta pixel en coordonnée GL a la profondeur du plan de
      travail*/
    gluUnProject(winx + iDeltaX, winy + iDeltaY, winz,
      modelView, projection, viewport,
      &p1x, &p1y, &p1z);
      
    popProjections();
    
    /*on met les points dans le systeme de coordonné local. La transformation
      de la camera est du systeme local vers le system global. On a donc besoin
      de la transformation inverse pour mettre les points globaux dans le 
      systeme local.*/
    Matrix4d sceneToLocal = getTransformationToLocal();
    Vector3d p0 = Vector3d(p0x, p0y, p0z) * sceneToLocal;
    Vector3d p1 = Vector3d(p1x, p1y, p1z) * sceneToLocal;
    Vector3d glDelta = p1 - p0;
    
    //projection de glDelta sur lateral
    Vector3d projGlDeltaOnLat = getLat() * ( getLat() * glDelta );
    //projection de glDelta sur up
    Vector3d projGlDeltaOnUp = getUp() * ( getUp() * glDelta );
    result = getLat() * iDeltaX/(abs(iDeltaX)) * projGlDeltaOnLat.norm() + 
      getUp() * iDeltaY/(abs(iDeltaY)) * projGlDeltaOnUp.norm();
  }

  
  return result;
}

//-----------------------------------------------------------------------------
void Camera::popProjections() const
{
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();
}

//-----------------------------------------------------------------------------
void Camera::print() const
{
  using namespace std;
  cout<<"\n\nPos: "<<mPos.getX()<<" "<<mPos.getY()<<" "<<mPos.getZ();
  cout<<"\nLook: "<<mLook.getX()<<" "<<mLook.getY()<<" "<<mLook.getZ();
  cout<<"\nLat: "<<mLat.getX()<<" "<<mLat.getY()<<" "<<mLat.getZ();
  cout<<"\nUp: "<<mUp.getX()<<" "<<mUp.getY()<<" "<<mUp.getZ();
}

//-----------------------------------------------------------------------------
void Camera::pushProjections() const
{
  glMatrixMode( GL_PROJECTION );
  glPushMatrix(); glLoadIdentity();
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix(); glLoadIdentity();
  applyProjectionTransformation();
  applyModelViewTransformation();
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
  
  mUp = getLat() ^ lookVect;
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
void Camera::setOrthoProjection(double iVisibleGlUnit, double iNear, double iFar)
{
  setProjection(-iVisibleGlUnit / 2.0, iVisibleGlUnit / 2.0,
   -1.0, 1.0, iNear, iFar, Projection::tOrthogonal, true);
}

//-----------------------------------------------------------------------------
void Camera::setOrthoProjection(double iVisibleGlUnitX, double iVisibleGlUnitY,
	double iNear, double iFar)
{
  setProjection(-iVisibleGlUnitX / 2.0, iVisibleGlUnitX / 2.0,
   -iVisibleGlUnitY / 2.0, iVisibleGlUnitY / 2.0, iNear, iFar,
   Projection::tOrthogonal, true);
}

//-----------------------------------------------------------------------------
/*iFov est en degree, iRatio est généralement le ratio du viewport sous la
  forme height / width ) */
void Camera::setPerspectiveProjection(double iFov, double iRatio,
                           double iNear, double iFar,
                           bool iProportional /*=true*/)
{
	//tan(iFov) = halfHeight / iNear;
  double halfHeight = iNear * tan(iFov * 0.5 * kDegreeToRadian);
  //iRatio = halfWidth / halfHeight;
  double halfWidth = iRatio * halfHeight;
  setProjection(-halfWidth, halfWidth, -halfHeight, halfHeight,
    iNear, iFar, Projection::tPerspective, iProportional);
}

//-----------------------------------------------------------------------------
void Camera::setProjection( const Projection& iP )
{ mProjectionInfo = iP; computeProjection();}

//-----------------------------------------------------------------------------
void Camera::setProjection(double iLeft, double iRight,
                           double iBottom, double iTop,
                           double iNear, double iFar,
                           Projection::type iType,
                           bool iProportional /*=true*/)
{
	Projection p;
	p.mLeft = iLeft;
  p.mRight = iRight;
  p.mBottom = iBottom;
  p.mTop = iTop;
	p.mNear = iNear;
  p.mFar = iFar;  
  p.mZoomFactor = 1.0;
  p.mType = iType;
	p.mProportionalToWindow = iProportional;
  setProjection(p);
}

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
//void Camera::setUp( const Vector3d& iUp )
//{
//  mUp = iUp;
//
//  Vector3d currentLookVect( getPos(), getLook()  );
//  
//  mLat = currentLookVect ^ getUp();
//  mLat.normalise();
//  
//  Vector3d updatedLookVect = getUp() ^ getLat();
//  mPos = getLook() - ( updatedLookVect.normalise() * currentLookVect.norm() );
//}

//-----------------------------------------------------------------------------
void Camera::setWindowSize( int iWidth, int iHeight )
{
  if(iWidth >= iHeight)
    mWindowInfo.mOrientation = WindowInfo::oHorizontal;
  else
    mWindowInfo.mOrientation = WindowInfo::oVertical;
  
  //le coté le plus long montre la valeur mVisibleGLUnit unité GL
  /*la valeur minimale pour les cote est de 1, on evite que la taille soit 0*/
  mWindowInfo.mShortSide = qMax( qMin(iWidth, iHeight), 1 );
  mWindowInfo.mLongSide = qMax( qMax(iWidth, iHeight), 1 );
  
  computeProjection();
}

//-----------------------------------------------------------------------------
void Camera::setWindowSize( Vector2i iS )
{ setWindowSize( iS.x(), iS.y() ); }

//-----------------------------------------------------------------------------
void Camera::setZoom(double iZoom)
{
  mProjectionInfo.mZoomFactor = iZoom;
  computeProjection();
}
