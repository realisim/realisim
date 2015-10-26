
#ifndef CORELIBRARY_GLCAMERA_HH
#define CORELIBRARY_GLCAMERA_HH

#include "coreLibrary, types.h"
//--- glew.h doit être inclus avant qgl.h (a cause de gl.h)
#ifdef WIN32
   #ifndef __glew_h__
      #include <GL/glew.h> //nécessaire pour l'utilisation de GL_BGRA
   #endif   
#endif
#include "qgl.h"

/*

Le systeme de coordonné de la caméra.

        Y
        ^
        |
        |
        |
        |
        -----------> X
       /
      /
     /
    Z
*/

namespace resonant
{
namespace coreLibrary
{

class glCamera
{
public:
   glCamera();
   virtual ~glCamera();

   enum projectionType{ ptOrthogonal, ptPerspective };
   struct projection
   {
      projection();
      double getWidth() const;
      double getHeight() const;

      projectionType mProjectionType;
      double mLeft;
      double mRight;
      double mBottom;
      double mTop;
      double mNear;
      double mFar;
   }; 

   virtual void applyModelViewProjection();
   virtual point3 cameraToWorld( point3 ) const;
   virtual vector3 cameraToWorld( vector3 ) const;
   virtual const point3& getLook() const;
   virtual const point3& getPosition() const;
   virtual const projection& getProjection() const;
   virtual projectionType getProjectionType() const;
   virtual const vector3& getUp() const;
   virtual const int getViewportHeight() const;
   virtual const int getViewportWidth() const;
   virtual const double getZoomFactor() const;
   static glCamera interpolate( double, glCamera, glCamera );
   virtual void rotate( double, vector3, point3 );
   virtual vector3 screenToWorld( vector2 ) const;
   virtual point3 screenToWorld( index2 ) const;
   virtual void set( point3, point3, vector3 );
   virtual void set( double, double, double,
      double, double, double,
      double, double, double );
   virtual void setLook( point3 );
   virtual void setOrthoProjection( double width, double height, 
      double iNear = 0.5, double iFar = 10000.0 );
   virtual void setPerspectiveProjection( double fov,
      double iNear = 0.5, double iFar = 10000.0 );
   virtual void setPosition( point3 );
   virtual void setProjection( projection );
   virtual void setProjection( double left, double right, double bottom,
      double top, double iNear, double iFar, projectionType );
   virtual void setUp( vector3 );
   virtual void setViewportSize( int, int );
   virtual void setZoom( double );
   virtual void setZoom( double, index2 );
   virtual void translate( vector3 );
   virtual point3 worldToCamera( point3 ) const;
   virtual vector3 worldToCamera( vector3 ) const;
   virtual index2 worldToScreen( point3 ) const;
   virtual vector2 worldToScreen( vector3 ) const;
   virtual void zoom( double );
   virtual void zoom( double, index2 );

protected:
   virtual void computeProjectionMatrix();
   virtual void computeViewMatrix();
   virtual const vector3& getLateral() const;
   virtual const vector3& getLookVector() const;

   point3 mPosition;
   vector3 mUp;
   point3 mLook;
   vector3 mLookVector;
   vector3 mLateral;
   projection mProjection;
   matrix4 mProjectionMatrix;
   matrix4 mViewMatrix;
   int mViewPortWidth;
   int mViewPortHeight;
   double mZoomFactor;
};

}
}

#endif