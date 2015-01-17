/* Camera.cpp */

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
mPos(),
mLat(),
mLook(),
mLookVector(),
mUp(),
mProjectionInfo(),
mViewport(),
mProjectionMatrix(),
mViewMatrix()
{
	set( Point3d( 0, 0, 100 ),
           Point3d( 0, 0, 0 ),
           Vector3d( 0, 1, 0 ) );
  setPerspectiveProjection(60, 1, 0.5, 10000.0, true);
}

//-----------------------------------------------------------------------------
Camera::~Camera()
{}

//-----------------------------------------------------------------------------
void Camera::applyModelViewTransformation() const
{ 
	//glLoadMatrixd( mViewMatrix.getPtr() );
  glLoadMatrixd( mViewMatrix.getDataPointer() );
}

//-----------------------------------------------------------------------------
void Camera::applyProjectionTransformation() const
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
    
  if( getViewport().mOrientation == Viewport::oHorizontal )
  { glViewport(0, 0, getViewport().getWidth(), getViewport().getHeight() ); }
  else //vertical
  { glViewport(0, 0, getViewport().getWidth(), getViewport().getHeight() ); }

	glLoadMatrixd(mProjectionMatrix.getDataPointer()); 
    
  glMatrixMode(GL_MODELVIEW);
}
//-----------------------------------------------------------------------------
Point3d Camera::cameraToWorld( const Point3d& iP ) const
{ return getViewMatrix().inverse() * iP; }
//-----------------------------------------------------------------------------
Vector3d Camera::cameraToWorld( const Vector3d& iV ) const
{ return getViewMatrix().inverse() * iV; }
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
  	double w = getViewport().getWidth() / p.getWidth();
    double h = getViewport().getHeight() / w / 2.0;
    double cy = p.mBottom + p.getHeight() / 2.0; 
    mProjectionInfo.mTop = cy + h;
    mProjectionInfo.mBottom = cy - h;
  }

	/*Afin d'appliquer correctement le zoom, qui est donné par getVisibleWidth
    et getVisibleHeight, on doit trouver le centre de la projection
    originale (sans zoom). Ensuite on détermine la taille du rectangle
    de la projection avec le zoom a partir de ce centre.*/
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
    case Projection::tOrthogonal:
    {
    	double m[4][4] = {
      	{ 2.0/(r-l), 0.0, 0.0, -(r+l)/(r-l) },
        { 0.0, 2.0/(t-b), 0.0, -(t+b)/(t-b) },
        { 0.0, 0.0, -2.0/(p.mFar - p.mNear), -(p.mFar+p.mNear)/(p.mFar-p.mNear) },
        { 0, 0, 0, 1.0 },
      };
      mProjectionMatrix = myMatrix4( m[0] );
    }	break;
    case Projection::tPerspective: 
    {
    	double m[4][4] = {
      	{ (2.0*p.mNear)/(r-l), 0.0, (r+l)/(r-l), 0.0 },
        { 0.0, (2.0*p.mNear)/(t-b), (t+b)/(t-b), 0.0 },
        { 0, 0, -(p.mFar+p.mNear)/(p.mFar-p.mNear), (-2 * p.mFar * p.mNear)/(p.mFar - p.mNear) },
        { 0.0, 0.0, -1, 0.0 },
      };
      mProjectionMatrix = myMatrix4( m[0] );
    } break;
    default: break;
  }
}

//-----------------------------------------------------------------------------
void Camera::computeViewMatrix()
{
  myMatrix4 viewMatrix( mLat, mUp, -mLookVector );
  myMatrix4 t( toVector( -getPos() ) );
  mViewMatrix = viewMatrix.transpose()*t;
}

//-----------------------------------------------------------------------------
const Vector3d& Camera::getLookVector() const
{return mLookVector;}

//-----------------------------------------------------------------------------
const myMatrix4& Camera::getViewMatrix() const
{ return mViewMatrix; }

//-----------------------------------------------------------------------------
const myMatrix4& Camera::getProjectionMatrix() const
{ return mProjectionMatrix; }

//-----------------------------------------------------------------------------
/*retourne la largeur visible en unité GL*/
double Camera::getVisibleHeight() const
{ return mProjectionInfo.getHeight() * 1.0 / getZoom(); }

//-----------------------------------------------------------------------------
double Camera::getVisibleWidth() const
{ return mProjectionInfo.getWidth() * 1.0 / getZoom(); }

//-----------------------------------------------------------------------------
void Camera::popMatrices() const
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
  cout<<"\n\nPos: "<<mPos.x()<<" "<<mPos.y()<<" "<<mPos.z();
  cout<<"\nLook: "<<mLook.x()<<" "<<mLook.y()<<" "<<mLook.z();
  cout<<"\nLat: "<<mLat.x()<<" "<<mLat.y()<<" "<<mLat.z();
  cout<<"\nUp: "<<mUp.x()<<" "<<mUp.y()<<" "<<mUp.z();
}

//-----------------------------------------------------------------------------
void Camera::pushAndApplyMatrices() const
{
  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  applyProjectionTransformation();
  applyModelViewTransformation();
}

//-----------------------------------------------------------------------------
void Camera::rotate( double iRad, Vector3d iAxe, 
	Point3d iAxisPos /*= Point3d()*/ )
{
	Point3d eye = getPos(), center = getLook();
  Vector3d up = getUp();
  
	myMatrix4 r( iRad, iAxe );
  myMatrix4 t( toVector( iAxisPos ) );
  r =  t * r * t.inverse();

  eye = r * eye;
  center = r * center;
  up = r * up;
  set( eye, center, up );
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
Point3d Camera::screenToWorld( Point2d iP, const Point3d& iR /*=Point3d(0.0)*/) const
{
	double x = iP.x();
  //l'axe de Qt est inversé par rapport a openGL
  double y = getViewport().getHeight() - iP.y();
  
  pushAndApplyMatrices();
  
  double modelView[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
  double projection[16];
  glGetDoublev(GL_PROJECTION_MATRIX, projection);
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  double winx, winy, winz, p0x, p0y, p0z;

  /*A partir du Point3d qui represente le point sur le plan de travail
    perpendiculaire a la camera, on le projete a l'écran. Ce qui nous
    intéresse c'est la coordonnée en z normalisé qui nous donne la profondeur
    de ce point3d dans le zBuffer qui sera utilisé pour trouver le point 
    en coordonnée GL*/
  gluProject( iR.x(), iR.y(), iR.z(),
    modelView, projection, viewport,
    &winx, &winy, &winz);
    
  /*On projete le point d'écran iX, iY, winz qui nous donnera un point (p0)
    qui correspond au point d'écran (iX, iY) a la profondeur du plan de
    travail. */
  gluUnProject(x, y, winz,
    modelView, projection, viewport,
    &p0x, &p0y, &p0z);

	popMatrices();
  return Point3d(p0x, p0y, p0z);
}

//-----------------------------------------------------------------------------
Vector3d Camera::screenToWorld( Vector2d iV, const Point3d& iP/*=Point3d(0.0)*/) const
{
	Point3d p0 = screenToWorld( Point2d(0.0), iP);
  Point3d p1 = screenToWorld( toPoint(iV), iP);
  return p1 - p0;
}

//-----------------------------------------------------------------------------
/*Définie la position de la caméra, le point visé et le vecteur up. Le 
  vecteur Latéral sera calculé à partir du vecteur up et ensuite le up
  sera recalculé afin d'assurer une base normale.*/
void Camera::set( const Point3d& iEye,
         const Point3d& iCenter,
         const Vector3d& iUp )
{
  mPos = iEye;
  mLook = iCenter;
  mUp = iUp;
  
  //on commence par calculer le vecteur laterale parce que le vecteur up
  //final est dependant du vecteur lateral
  mLookVector.set( getPos(), getLook() );
  mLookVector.normalise();
  
  mLat = mLookVector ^ mUp;
  mLat.normalise();
  
  mUp = mLat ^ mLookVector;
  mUp.normalise();
  
  computeViewMatrix();
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
void Camera::setViewportSize( int iWidth, int iHeight )
{
  if(iWidth >= iHeight)
    mViewport.mOrientation = Viewport::oHorizontal;
  else
    mViewport.mOrientation = Viewport::oVertical;
  
  //le coté le plus long montre la valeur mVisibleGLUnit unité GL
  /*la valeur minimale pour les cote est de 1, on evite que la taille soit 0*/
  mViewport.mShortSide = qMax( qMin(iWidth, iHeight), 1 );
  mViewport.mLongSide = qMax( qMax(iWidth, iHeight), 1 );
  
  computeProjection();
}
//-----------------------------------------------------------------------------
void Camera::setViewportSize( Vector2i iS )
{ setViewportSize( iS.x(), iS.y() ); }
//-----------------------------------------------------------------------------
void Camera::setZoom(double iZoom)
{
  mProjectionInfo.mZoomFactor = iZoom;
  computeProjection();
}
//-----------------------------------------------------------------------------
void Camera::translate( const Point3d& iV )
{ Vector3d d = iV - getPos(); translate( d ); }
//-----------------------------------------------------------------------------
void Camera::translate( const Vector3d& iV )
{ set( mPos + iV, mLook + iV, getUp() ); }
//-----------------------------------------------------------------------------
Point3d Camera::worldToCamera( const Point3d& iP ) const
{ return getViewMatrix() * iP; }
//-----------------------------------------------------------------------------
Vector3d Camera::worldToCamera( const Vector3d& iV ) const
{ return getViewMatrix() * iV; }
//-----------------------------------------------------------------------------
Point2d Camera::worldToSreen( const Point3d& iP ) const // comme glToPixel
{
  pushAndApplyMatrices();
  
  double modelView[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
  double projection[16];
  glGetDoublev(GL_PROJECTION_MATRIX, projection);
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  double x, y, z;
	bool sucess = gluProject(iP.x(), iP.y(), iP.z(), 
    modelView, 
    projection, 
    viewport, 
    &x, &y, &z);
  if ( !sucess ) { x = 0; y = 0; z = 0; }

  popMatrices();

	//dépendant de l'orientation de la camera?
	return Point2d( x, y );
}
//-----------------------------------------------------------------------------
Vector2d Camera::worldToSreen( const Vector3d& iV ) const
{
	Point2d v0 = worldToSreen( Point3d(0.0) );
  Point2d v1 = worldToSreen( toPoint( iV ) );
  return v1-v0;
}