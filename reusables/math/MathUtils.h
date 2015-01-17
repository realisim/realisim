//!-----------------------------------------------------------------------------
//! \file
//! \brief Fichier contenant des fonctions mathÈmatique utile.
//!
//! AUTHOR:  Pierre-Olivier Beaudoin & David Pinson
//!-----------------------------------------------------------------------------
#ifndef MATH_UTILE_H
#define MATH_UTILE_H

#include "Matrix4.h"
#include "Point.h"
#include "Vect.h"
#include <vector>
#include "Quaternion.h"

namespace realisim
{
namespace math
{
	using namespace std;
  
	//---------------------------------------------------------------------------
  template< class T >
  Vector3<T> absolute( const Vector3<T>& iP )
  { return Vector3<T>( abs( iP.x() ), abs( iP.y() ), abs( iP.z() ) ); }
  //---------------------------------------------------------------------------
	template<class T>
	bool isEqual( T a, T b, T iEpsilon = std::numeric_limits<T>::epsilon() )
  {
    T d = a - b;
    if(d<(T)0.0) d = -d;
    return ( d <= iEpsilon );
  }
	//---------------------------------------------------------------------------
	template<class T>
	bool isEqual( const Vector3<T>& iV0, const Vector3<T>& iV1, T iEpsilon = 
  	std::numeric_limits<T>::epsilon() )
  {
    T dx = iV0.x() - iV1.x();
    T dy = iV0.y() - iV1.y();
    T dz = iV0.z() - iV1.z();
    if(dx<(T)0.0) dx = -dx;
    if(dy<(T)0.0) dy = -dy;
    if(dz<(T)0.0) dz = -dz;
    return (dx <= iEpsilon && dy <= iEpsilon && dz <= iEpsilon);
  }
	//---------------------------------------------------------------------------
  template<class T>
	bool isEqual( const Point3<T>& iP0, const Point3<T>& iP1, T iEpsilon = 
  	std::numeric_limits<T>::epsilon() )
  {
    T dx = iP0.x() - iP1.x();
    T dy = iP0.y() - iP1.y();
    T dz = iP0.z() - iP1.z();
    if(dx<(T)0.0) dx = -dx;
    if(dy<(T)0.0) dy = -dy;
    if(dz<(T)0.0) dz = -dz;
    return (dx <= iEpsilon && dy <= iEpsilon && dz <= iEpsilon);
  }
  //---------------------------------------------------------------------------
  //convertie un vecteur en point
  template<class T>
  inline Point3<T> toPoint (const Vector3<T>& iV)
  { return Point3<T>(iV.x(), iV.y(), iV.z()); }
  
  //---------------------------------------------------------------------------
  //convertie un point en vecteur
  template<class T>
  inline Vector3<T> toVector (const Point3<T>& iP)
  { return Vector3<T>(iP.x(), iP.y(), iP.z()); }
  
  //---------------------------------------------------------------------------
  //convertie un vecteur en point
  template<class T>
  inline Point2<T> toPoint (const Vector2<T>& iV)
  { return Point2<T>( iV.x(), iV.y() ); }
  
  //---------------------------------------------------------------------------
  //convertie un point en vecteur
  template<class T>
  inline Vector2<T> toVector (const Point2<T>& iP)
  { return Vector2<T>(iP.x(), iP.y()); }

  //---------------------------------------------------------------------------
  template<class V>
  inline Quaternion<V> operator* (const Quaternion<V> &quat,
                                  const Point3<V> &point)
  {
    Quaternion<V> result;
    
    result.setW(- (quat.x() * point.x()) - (quat.y() * point.y()) -
                (quat.z() * point.z()) );
    
    result.setX( (quat.w() * point.x()) + (quat.y() * point.z()) -
                (quat.z() * point.y()) );
    
    result.setY( (quat.w() * point.y()) - (quat.x() * point.z()) +
                (quat.z() * point.x()) );
    
    result.setZ( (quat.w() * point.z()) + (quat.x() * point.y()) -
                (quat.y() * point.x()) );
    
    return result;
  }
  
  //---------------------------------------------------------------------------
  template<class T>
  inline Point3<T> operator* (const T& iVal, const Point3<T>& iPoint)
  {
  	return Point3<T>( iVal * iPoint.x(),
    	iVal * iPoint.y(),
      iVal * iPoint.z() );
  }
  //---------------------------------------------------------------------------
  template<class T>
  inline Point3<T> operator+ (const Point3<T> &point, const Vector3<T> &vect)
  {
  	return Point3<T>( point.x() + vect.x(),
    	point.y() + vect.y(),
    	point.z() + vect.z() );
  }
  //---------------------------------------------------------------------------
  template<class T>
  inline Point3<T> operator- (const Point3<T> &point, const Vector3<T> &vect)
  {
    return Point3<T>( point.x() - vect.x(),
    	point.y() - vect.y(),
    	point.z() - vect.z() );
  }
  //---------------------------------------------------------------------------
  template<class T>
  inline Vector3<T> operator+ (const Point3<T> &p1, const Point3<T> &p2)
  {
    return Vector3<T>( p1.x() + p2.x(),
    	p1.y() + p2.y(),
      p1.z() + p2.z() );
  }
  //---------------------------------------------------------------------------
  template<class T>
  inline Vector3<T> operator- (const Point3<T> &p1, const Point3<T> &p2)
  {
    return Vector3<T>( p1.x() - p2.x(),
    	p1.y() - p2.y(),
      p1.z() - p2.z() );
  }
  
  //---------------------------------------------------------------------------
  //---
  //-- Operateur Point2
  //---
  //---------------------------------------------------------------------------
  template<class V>
  inline Point2<V> operator+ (const Point2<V> &p, const Vector2<V> &v)
  { return Point2<V>( p.x() + v.x(), p.y() + v.y() ); }
  
  template<class V>
  inline Point2<V> operator- (const Point2<V> &p, const Vector2<V> &v)
  { return Point2<V>( p.x() - v.x(), p.y() - v.y() ); }
  
  template<class V>
  inline Vector2<V> operator+ (const Point2<V> &p1, const Point2<V> &p2)
  { return Vector2<V>( p1.x() + p2.x(), p1.y() + p2.y() ); }
  
  template<class V>
  inline Vector2<V> operator- (const Point2<V> &p1, const Point2<V> &p2)
  { return Vector2<V>( p1.x() - p2.x(), p1.y() - p2.y() ); }
  
  template<class V>
  inline Vector2<V> operator* (double a, const Vector2<V> &v)
  { return Vector2<V>( v.x() * a, v.y() * a ); }

  
  //---------------------------------------------------------------------------
  //---
  //-- Operateur Vector3
  //---
  //---------------------------------------------------------------------------
  template<class V>
  inline Vector3<V> operator* (const V& iVal, const Vector3<V>& vect)
  {
    Vector3<V> result;
    
    result.setX(iVal * vect.x());
    result.setY(iVal * vect.y());
    result.setZ(iVal * vect.z());
    
    return result;
  }
  //---------------------------------------------------------------------------
  template<class T>
  Vector3<T> getPerpendicularVector( const Vector3<T>& iV )
  {
  	/*afin d'obtenir un vecteur perpendiculaire, le produit scalaire doit donner
    0. donc
    	1- (ax, by, cz) * (dx, ey, fz) = 0 
      2- ( a*d + b*e + c*z ) = 0 
      si d = b et que e = -a et que z = 0,
      3- a*b + b*(-a) + 0 = 0
      Bref, en permuttant deux valeurs et en inversant une des deux et remplacant
      la troisieme par 0, on obtient toujours un vecteur perpendiculaire.*/
    Vector3<T> r(1.0, 0.0, 0.0);
    if( !isEqual( iV.x(), 0.0 ) )
    	r = Vector3<T>( iV.y(), -iV.x(), (T)0.0 );
    else if( !isEqual( iV.y(), 0.0 ) ) 
    	r = Vector3<T>( -iV.y(), iV.x(), (T)0.0 );
    else if( !isEqual( iV.z(), 0.0 ) )
			r = Vector3<T>( (T)0.0, iV.z(), -iV.y() );
  	return r;
  }
  //---------------------------------------------------------------------------
  inline Matrix4 interpolate(const Matrix4& iM1, const Matrix4& iM2, 
    double iT)
  {
  	Quaterniond q1 = iM1.getRotationAsQuaternion();
    Quaterniond q2 = iM2.getRotationAsQuaternion();
    //on compare avec les longueurs pour prendre le plus petit angle
    if( (-q2-q1).getLength() < (q2-q1).getLength() )
    {
      q2 = -q2;
    }
    
    q2 = q1*( 1 - iT ) + q2*iT;
    Matrix4 iterationMatrix( q2 );
    
    //trouver la translation totale a effectuer
  	Vector3d t = iM1.getTranslationAsVector()*( 1 - iT ) + 
      iM2.getTranslationAsVector()*( iT );
    Matrix4 translation(t);
    iterationMatrix = translation * iterationMatrix;
    return iterationMatrix;
  }
    
  //----------------------------------------------------------------------------
  template< class T >
  bool isCoplanar( const std::vector< Point3<T>* >& iP, double iEpsilon = 
  	std::numeric_limits<double>::epsilon() )
  {
  	Vector3d n0, n1;
    int iMinus1, i0, i1;
    bool r = true;
    if( iP.size() > 3 )
    {
    	n0 = Vector3d( *iP[0], *iP[1] ) ^ Vector3d( *iP[1], *iP[2] );
      n0.normalise();
//printf("n0; %.16f, %.16f, %.16f\n", n0.x(), n0.y(), n0.z() );
      for( int i = 2; i < (int)iP.size(); ++i )
      {
      	iMinus1 = i -1; i0 = i; i1 = (i + 1) % iP.size();
        n1 = Vector3d( *iP[iMinus1], *iP[i0] ) ^ Vector3d( *iP[i0], *iP[i1] );
        n1.normalise();
//printf("n1; %.16f, %.16f, %.16f\n", n1.x(), n1.y(), n1.z() );
        if( !math::isEqual( absolute(n0), absolute(n1), iEpsilon ) )
        { r = false; break; }
      }
    }
    return r;
  }
  
  //----------------------------------------------------------------------------
  //Interpolation
  //iValue must be between 0-1
  //http://sol.gfxile.net/interpolation/
  template<class T>
  T smoothStep( T iValue )
  { return (iValue) * (iValue) * (3 - 2 * (iValue) ); }
  
  //----------------------------------------------------------------------------
  //Interpolation
  //iValue must be between 0-1
  //http://sol.gfxile.net/interpolation/
  template<class T>
  T smoothStep2( T iValue )
  { return smoothStep( smoothStep( iValue ) ); }
  
  //----------------------------------------------------------------------------
  //Interpolation
  //iValue must be between 0-1
  //http://sol.gfxile.net/interpolation/
  template<class T>
  T inversePower( T iValue, unsigned int iPower = 1 )
  {
    T result = 1;
    for( unsigned int i = 0; i < iPower; ++i  )
    {
       result *= (1 - iValue) ;
    }
    return 1 - result; 
  }
  
  double normalDistribution( double x, double s, double u = 0.0 );
  vector<double> gaussianKernel1D( int iKernelSize, double s );
  vector< double > gaussianKernel2D( int iKernelSize, double s );
  vector<double> meanKernel2D( int iKernelSize );
  
} //math
} // fin du namespace realisim


#endif // MATH_UTILE_H
