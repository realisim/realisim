
#include <iostream>
#include "coreLibrary, glCamera.h"

using namespace resonant;
using namespace coreLibrary;

//------------------------------------------------------------------------------
glCamera::projection::projection() : 
   mProjectionType( ptPerspective ),
   mLeft( -50 ), mRight( 50 ),
   mBottom( -50 ), mTop( 50 ),
   mNear( 0.5 ), mFar( 1000.0 )
{}

//------------------------------------------------------------------------------
double glCamera::projection::getWidth() const
{ return mRight - mLeft; }
//------------------------------------------------------------------------------
double glCamera::projection::getHeight() const
{ return mTop - mBottom; }

//------------------------------------------------------------------------------
//--- glCamera
//------------------------------------------------------------------------------
glCamera::glCamera() : 
   mPosition( 50.0, 0.0, 0.0 ),
   mUp( 0.0, 0.0, 1.0 ),
   mLook( 0.0, 0.0, 0.0 ),
   mProjection(),
   mProjectionMatrix(),
   mViewMatrix(),
   mViewPortWidth(1),
   mViewPortHeight(1),
   mZoomFactor( 1.f )
{}

//------------------------------------------------------------------------------
glCamera::~glCamera()
{}

//------------------------------------------------------------------------------
void glCamera::applyModelViewProjection()
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   
   glViewport(0, 0, getViewportWidth(), getViewportHeight() );
   glLoadMatrixd( (GLdouble*)&mProjectionMatrix );

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glLoadMatrixd( (GLdouble*)&mViewMatrix );
}

//------------------------------------------------------------------------------
/* Conversion d'un point de l'espace camera a l'espace monde (GL). */
point3 glCamera::cameraToWorld( point3 pCam ) const
{
   matrix4 m( getLateral(), getUp(), -getLookVector() );
   matrix4 t( getPosition() - point3::origin );
   return ( t * m ).transform( pCam );
}

//------------------------------------------------------------------------------
/* Conversion d'un vecteur3 de l'espace camera a l'espace monde (GL). Utile
   particulierement pour la conversion d'axe. */
vector3 glCamera::cameraToWorld( vector3 vCam ) const
{
   point3 p = matrix4( getLateral(), getUp(), -getLookVector() ).transform( point3( vCam.dx(), vCam.dy(), vCam.dz() ) );
   return p - point3::origin;
}

//------------------------------------------------------------------------------
void glCamera::computeProjectionMatrix()
{
   const projection& p = getProjection();
   /* Quand on veut une projection proportionnelle, on commence par trouver
   la hauteur (h) proportionnel a la largeur proportionnel (w) courante. 
   Ensuite, on trouve le centre (cy) de la projection et y ajoute la moitié de h
   de part et d'autre. */
   {
      double w = getViewportWidth() / p.getWidth();
      double h = getViewportHeight() / w / 2.0;
      double cy = p.mBottom + p.getHeight() / 2.0; 
      mProjection.mTop = cy + h;
      mProjection.mBottom = cy - h;
   }

  /*Afin d'appliquer correctement le zoom, qui est donné par getZoomFactor,
    on doit trouver le centre de la projection originale (sans zoom).
    Ensuite on détermine la taille du rectangle
    de la projection avec le zoom a partir de ce centre.*/
  double halfVisibleWidth = mProjection.getWidth() * 1.0 / getZoomFactor() / 2.0;
  double halfVisibleHeigh = mProjection.getHeight() * 1.0 / getZoomFactor() / 2.0;
  double cx = 0.0, cy = 0.0, l, r, b, t;
  cx = p.mLeft + p.getWidth() / 2.0;
  cy = p.mBottom + p.getHeight() / 2.0; 
  l = cx - halfVisibleWidth;
  r = cx + halfVisibleWidth;
  b = cy - halfVisibleHeigh;
  t = cy + halfVisibleHeigh;

   switch ( getProjectionType() ) 
   {
   case ptOrthogonal:
      mProjectionMatrix( 0, 0 ) = 2.0/(r-l);
      mProjectionMatrix( 0, 1 ) = 0.0;
      mProjectionMatrix( 0, 2 ) = 0.0;
      mProjectionMatrix( 0, 3 ) = (r+l)/(l-r);
      mProjectionMatrix( 1, 0 ) = 0.0;
      mProjectionMatrix( 1, 1 ) = 2.0/(t-b);
      mProjectionMatrix( 1, 2 ) = 0.0;
      mProjectionMatrix( 1, 3 ) = (t+b)/(b-t);
      mProjectionMatrix( 2, 0 ) = 0.0;
      mProjectionMatrix( 2, 1 ) = 0.0;
      mProjectionMatrix( 2, 2 ) = 2.0/(p.mNear - p.mFar);
      mProjectionMatrix( 2, 3 ) = (p.mFar+p.mNear)/(p.mNear-p.mFar);
      mProjectionMatrix( 3, 0 ) = 0;
      mProjectionMatrix( 3, 1 ) = 0;
      mProjectionMatrix( 3, 2 ) = 0;
      mProjectionMatrix( 3, 3 ) = 1.0;
      break;
   case ptPerspective:
      mProjectionMatrix( 0, 0 ) = (2.0*p.mNear)/(r-l);
      mProjectionMatrix( 0, 1 ) = 0.0;
      mProjectionMatrix( 0, 2 ) = (r+l)/(r-l);
      mProjectionMatrix( 0, 3 ) = 0.0;
      mProjectionMatrix( 1, 0 ) = 0.0;
      mProjectionMatrix( 1, 1 ) = (2.0*p.mNear)/(t-b);
      mProjectionMatrix( 1, 2 ) = (t+b)/(t-b);
      mProjectionMatrix( 1, 3 ) = 0.0;
      mProjectionMatrix( 2, 0 ) = 0;
      mProjectionMatrix( 2, 1 ) = 0;
      mProjectionMatrix( 2, 2 ) = (p.mFar+p.mNear)/(p.mNear-p.mFar);
      mProjectionMatrix( 2, 3 ) = (2 * p.mFar * p.mNear)/(p.mNear - p.mFar);
      mProjectionMatrix( 3, 0 ) = 0.0;
      mProjectionMatrix( 3, 1 ) = 0.0;
      mProjectionMatrix( 3, 2 ) = -1.0;
      mProjectionMatrix( 3, 3 ) = 0.0;
      break;
   default: break;
   }
}

//------------------------------------------------------------------------------
void glCamera::computeViewMatrix()
{  
   mViewMatrix = matrix4( mLateral, mUp, -getLookVector() );
   //mViewMatrix = mViewMatrix.transpose();
   mViewMatrix = mViewMatrix.inverse();
   matrix4 translation( -vector3(mPosition.x(), mPosition.y(), mPosition.z()) );
   mViewMatrix *= translation;
}

//------------------------------------------------------------------------------
const vector3& glCamera::getLateral() const
{ return mLateral; }
//------------------------------------------------------------------------------
const point3& glCamera::getLook() const
{ return mLook; }
//------------------------------------------------------------------------------
const vector3& glCamera::getLookVector() const
{ return mLookVector; }
//------------------------------------------------------------------------------
const point3& glCamera::getPosition() const
{ return mPosition; }
//------------------------------------------------------------------------------
const glCamera::projection& glCamera::getProjection() const
{ return mProjection; }
//------------------------------------------------------------------------------
glCamera::projectionType glCamera::getProjectionType() const
{ return mProjection.mProjectionType; }
//------------------------------------------------------------------------------
const vector3& glCamera::getUp() const
{ return mUp; }
//------------------------------------------------------------------------------
const int glCamera::getViewportHeight() const
{ return mViewPortHeight; }
//------------------------------------------------------------------------------
const int glCamera::getViewportWidth() const
{ return mViewPortWidth; }
//------------------------------------------------------------------------------
const double glCamera::getZoomFactor() const
{ return mZoomFactor; }
//------------------------------------------------------------------------------
glCamera glCamera::interpolate( double factor, glCamera previousCamera, glCamera nextCamera )
{
   glCamera c = previousCamera;
   double t = factor * factor * ( 3 - 2 * factor );

   // interpolation de ViewMatrix
   vector3 p( nextCamera.getPosition() - previousCamera.getPosition() );
   vector3 l( nextCamera.getLook() - previousCamera.getLook() );
   vector3 u( nextCamera.getUp() - previousCamera.getUp() );
   p *= t; l *= t; u *= t;
   c.set( previousCamera.getPosition() + p, previousCamera.getLook() + l, 
      previousCamera.getUp() + u );

   /* Nous interpolons la projection de deux manieres, selon les cas.
      - si les 2 cameras ont le meme zoom, alors nous interpolons directement la projectionMatrix;
      - sinon nous interpolons via setZoom() qui recalculera en fonction la projection. */
   if( fabs( nextCamera.getZoomFactor() - previousCamera.getZoomFactor() ) < 1e-6 )
   {
      glCamera::projection projection,
         nextProjection = nextCamera.getProjection(),
         previousProjection = previousCamera.getProjection();
      projection.mProjectionType = nextCamera.getProjectionType();
      projection.mTop = ( nextProjection.mTop - previousProjection.mTop ) * t + previousProjection.mTop;
      projection.mBottom = ( nextProjection.mBottom - previousProjection.mBottom ) * t + previousProjection.mBottom;
      projection.mLeft = ( nextProjection.mLeft - previousProjection.mLeft ) * t + previousProjection.mLeft;
      projection.mRight = ( nextProjection.mRight - previousProjection.mRight ) * t + previousProjection.mRight;
      projection.mNear = ( nextProjection.mNear - previousProjection.mNear ) * t + previousProjection.mNear;
      projection.mFar = ( nextProjection.mFar - previousProjection.mFar ) * t + previousProjection.mFar;
      c.setProjection( projection );
   }
   else
   {
      double z = ( nextCamera.getZoomFactor() - previousCamera.getZoomFactor() ) * t + previousCamera.getZoomFactor();
      c.setZoom( z );
   }
   return c;
}
//------------------------------------------------------------------------------
/* Rotation selon un axe autours d'un point. */
void glCamera::rotate( double angle, vector3 axis, point3 axisPosition )
{
   matrix4 r( axis, angle );
   matrix4 ptoO( point3::origin - axisPosition );
   matrix4 otoP( axisPosition - point3::origin );

   set( ( otoP * r * ptoO ).transform( getPosition() ),
        ( otoP * r * ptoO ).transform( getLook() ),
        r.transform( point3( getUp().dx(), getUp().dy(), getUp().dz() ) ) - point3::origin );
}
//------------------------------------------------------------------------------
/* Conversion d'un index2 de l'espace ecran a l'espace monde (GL). */
point3 glCamera::screenToWorld( index2 pScreen ) const
{
   point3 glPoint;
   GLdouble pX, pY, pZ;
   GLdouble winX, winY, winZ;
   GLint viewport[4] = { 0, 0, mViewPortWidth, mViewPortHeight };
   gluProject( mLook.x(), mLook.y(), mLook.z(),
      (GLdouble*) &mViewMatrix,
      (GLdouble*) &mProjectionMatrix,
      viewport,
      &winX, &winY, &winZ );
   gluUnProject( pScreen.i(), pScreen.j(), winZ,
      (GLdouble*) &mViewMatrix,
      (GLdouble*) &mProjectionMatrix,
      viewport,
      &pX, &pY, &pZ );
   glPoint.set( pX, pY, pZ );
   return glPoint;
}
//------------------------------------------------------------------------------
/* Conversion d'un vecteur2 de l'espace ecran a l'espace monde (GL). */
vector3 glCamera::screenToWorld( vector2 delta ) const
{
   point3 origin3 = screenToWorld( index2( 0, 0 ) );
   point3 delta3 = screenToWorld( index2( delta.dx(), delta.dy() ) );
   return vector3( delta3 - origin3 );
}
//------------------------------------------------------------------------------
void glCamera::set( point3 iPos, point3 iLook, vector3 iUp )
{
   mPosition = iPos;
   mLook = iLook;
   mUp = iUp;

   //on s'assure que les vecteur sont orthogonaux
   mLookVector = getLook() - getPosition();
   mLookVector.normalize();
   mLateral = mLookVector ^ getUp();
   mLateral.normalize();
   mUp = mLateral ^ mLookVector;
   mUp.normalize();

   computeViewMatrix();
}
//------------------------------------------------------------------------------
void glCamera::set( double px, double py, double pz, 
   double lx, double ly, double lz, double ux, double uy, double uz )
{
   set( point3( px, py, pz ),
      point3( lx, ly, lz ),
      vector3( ux, uy, uz ) );
}
//------------------------------------------------------------------------------
void glCamera::setLook( point3 look )
{ set( getPosition(), look, getUp() ); }
//------------------------------------------------------------------------------
void glCamera::setOrthoProjection( double width, double height,
   double iNear /* = 0.5 */,
   double iFar /* = 10000 */)
{
   setProjection( -width/2.0, width/2.0, -height/2.0, height/2.0, iNear, iFar,
     ptOrthogonal );
}
//------------------------------------------------------------------------------
/* iFov est en degree, iRatio est généralement le ratio du viewport sous la
   forme height / width ) */
void glCamera::setPerspectiveProjection(double fov,
   double iNear /* = 0.5 */, double iFar /* = 10000 */ )
{
   const double kDegToRad = 2 * 3.14159265359 / 360.0;
   //tan(iFov) = halfHeight / iNear;
   double halfHeight = iNear * tan(fov * 0.5 * kDegToRad);
   //iRatio = halfWidth / halfHeight;
   double halfWidth = halfHeight;
   setProjection( -halfWidth, halfWidth, -halfHeight, halfHeight, iNear,
      iFar, ptPerspective );
}
//------------------------------------------------------------------------------
void glCamera::setPosition( point3 position )
{ set( position, getLook(), getUp() ); }
//------------------------------------------------------------------------------
void glCamera::setProjection( projection p )
{ 
   mProjection = p;
   computeProjectionMatrix();
}
//------------------------------------------------------------------------------
void glCamera::setProjection( double left, double right, double bottom, 
   double top, double iNear, double iFar, projectionType pt )
{
   projection p;
   p.mLeft = left;
   p.mRight = right;
   p.mBottom = bottom;
   p.mTop = top;
   p.mNear = iNear;
   p.mFar = iFar;
   p.mProjectionType = pt;
   setProjection( p );
}
//------------------------------------------------------------------------------
void glCamera::setUp( vector3 up )
{ set( getPosition(), getLook(), up ); }
//------------------------------------------------------------------------------
void glCamera::setViewportSize( int sx, int sy )
{
   mViewPortWidth = sx;
   mViewPortHeight = sy;
   computeProjectionMatrix();
}
//------------------------------------------------------------------------------
/* Zoom numerique, regle le zoomFactor a x, sur le point central de l'ecran. */
void glCamera::setZoom( double x )
{
   double f = x / mZoomFactor;
   mZoomFactor = x;
   setProjection( mProjection.mLeft / f,
                  mProjection.mRight / f,
                  mProjection.mBottom / f,
                  mProjection.mTop / f,
                  mProjection.mNear, mProjection.mFar, mProjection.mProjectionType );
}
//------------------------------------------------------------------------------
/* Zoom numerique, regle le zoomFactor a x, sur un point donne de l'ecran. */
void glCamera::setZoom( double x, index2 aim )
{
   point3 aW = screenToWorld( aim );
   setZoom( x );
   point3 bW = screenToWorld( aim );
   if( x >= 1.f )
   { translate( aW - bW ); }
}
//------------------------------------------------------------------------------
void glCamera::translate( vector3 movement )
{
   set( getPosition() + movement, getLook() + movement, getUp() );
}

//------------------------------------------------------------------------------
/* Conversion d'un point de l'espace monde (GL) a l'espace camera. */
point3 glCamera::worldToCamera( point3 pWorld ) const
{
   matrix4 m( getLateral(), getUp(), -getLookVector() );
   matrix4 t( getPosition() - point3::origin );
   return ( t * m ).inverse().transform( pWorld );
}

//------------------------------------------------------------------------------
/* Conversion d'un vecteur3 de l'espace monde (GL) a l'espace camera. Utile
   particulierement pour la conversion d'axe. */
vector3 glCamera::worldToCamera( vector3 vWorld ) const
{
   point3 p = matrix4( getLateral(), getUp(), -getLookVector() ).inverse().transform( point3( vWorld.dx(), vWorld.dy(), vWorld.dz() ) );
   return p - point3::origin;
}

//------------------------------------------------------------------------------
/* Conversion d'un point3 de l'espace monde (GL) a l'espace ecran. */
index2 glCamera::worldToScreen( point3 pWorld ) const
{
   index2 pScreen;
   GLdouble winX, winY, winZ;
   GLint viewport[4] = { 0, 0, mViewPortWidth, mViewPortHeight };
   gluProject( pWorld.x(), pWorld.y(), pWorld.z(),
      (GLdouble*) &mViewMatrix,
      (GLdouble*) &mProjectionMatrix,
      viewport,
      &winX, &winY, &winZ );
   pScreen.set( winX, winY );
   return pScreen;
}

//------------------------------------------------------------------------------
/* Conversion d'un vector3 de l'espace monde (GL) a l'espace ecran. */
vector2 glCamera::worldToScreen( vector3 delta ) const
{
   index2 origin2 = worldToScreen( point3( 0, 0, 0 ) );
   index2 delta2 = worldToScreen( point3( delta.dx(), delta.dy(), delta.dz() ) );
   return vector2( point2( delta2.i(), delta2.j() )
                   - point2( origin2.i(), origin2.j() ) );
}

//------------------------------------------------------------------------------
/* Zoom numerique, incremente le zoomFactor de x, sur le point central de l'ecran. */
void glCamera::zoom( double x )
{
   setZoom( mZoomFactor * x );
}

//------------------------------------------------------------------------------
/* Zoom numerique, incremente le zoomFactor de x, sur un point donne de l'ecran. */
void glCamera::zoom( double x, index2 aim )
{
   setZoom( mZoomFactor * x, aim );
}