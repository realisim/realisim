//!-----------------------------------------------------------------------------
//! \file
//! \brief Fichier contenant des fonctions mathÈmatique utile.
//!
//! AUTHOR:  Pierre-Olivier Beaudoin & David Pinson
//!-----------------------------------------------------------------------------
#ifndef MATH_UTILE_H
#define MATH_UTILE_H

#include "Matrix4.h"
#include "Matrix4x4.h"
#include "Point.h"
#include "Vect.h"
#include <vector>
#include "Quaternion.h"

namespace realisim
{
namespace math
{
	using namespace std;
  Matrix4d fromMyMatrix( const myMatrix4& iM );  
  myMatrix4 toMyMatrix( const Matrix4d& iM );
  
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
  /*Le vecteur multiplié par la matrice ne fait que modifié sont orientation,
    on n'ajoute pas la translation.*/
  template<class T>
  inline Vector3<T> operator* ( const Vector3<T>& iVect, const Matrix4<T>& iMat)
  {
    Vector3<T> vect;
    vect.setX( iVect.x() * iMat(0, 0) + iVect.y() * iMat(1, 0) + iVect.z() * iMat(2, 0) );
    vect.setY( iVect.x() * iMat(0, 1) + iVect.y() * iMat(1, 1) + iVect.z() * iMat(2, 1) );
    vect.setZ( iVect.x() * iMat(0, 2) + iVect.y() * iMat(1, 2) + iVect.z() * iMat(2, 2) );
    return vect;
  }
  
  //---------------------------------------------------------------------------
  //---
  //-- Operateur pour Point3d
  //---
  //---------------------------------------------------------------------------
  /*On multiplie le point par la partie de rotation et on ajoute la translation
    au point.*/
  template<class T>
  inline Point3<T> operator* ( const Point3<T>& iPoint, const Matrix4<T>& iMat)
  {
    Point3<T> result;
    result.setX( iPoint.x() * iMat(0, 0) + iPoint.y() * iMat(1, 0) + iPoint.z() * iMat(2, 0) );
    result.setY( iPoint.x() * iMat(0, 1) + iPoint.y() * iMat(1, 1) + iPoint.z() * iMat(2, 1) );
    result.setZ( iPoint.x() * iMat(0, 2) + iPoint.y() * iMat(1, 2) + iPoint.z() * iMat(2, 2) );
    //on ajoute la translation au point
    result += Point3<T>(iMat(3, 0), iMat(3, 1), iMat(3, 2));
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
  //retourne la matrice de rotation correpondant a la rotation de iAngle
  //radian autour de l'axe iAxis
  template<class T>
  inline Matrix4d getRotationMatrix( double iAngle,
                                      Vector3<T> iAxis )
  {
    iAxis.normalise();
    Quaternion<T> quat;
    quat.setRot(iAngle, iAxis);
    
    return quat.getUnitRotationMatrix();
  }
  //---------------------------------------------------------------------------
  //retourne la matrice de rotation correpondant a la rotation de la matrice m
  //de iAngle radian autour de l'axe iAxis
  template<class T>
  inline Matrix4<T> rotate( const Matrix4<T> &m, const T &angle,
                              Vector3<T> axis, const Point3<T> &axisPos )
  {
		Matrix4<T> r = m;
    axis.normalise();
    Quaternion<T> quat;
    quat.setRot(angle, axis);
    r.translate(toVector(axisPos * -1));
    r = r * quat.getUnitRotationMatrix(); 
    r.translate(toVector(axisPos));
    return r;
  }
  //---------------------------------------------------------------------------
  inline myMatrix4 interpolate(const myMatrix4& iM1, const myMatrix4& iM2, 
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
    myMatrix4 iterationMatrix( q2 );
    
    //trouver la translation totale a effectuer
  	Vector3d t = iM1.getTranslationAsVector()*( 1 - iT ) + 
      iM2.getTranslationAsVector()*( iT );
    myMatrix4 translation(t);
    iterationMatrix = translation * iterationMatrix;
    return iterationMatrix;
  }
  
//  //---------------------------------------------------------------------------
//  //l'angle est en radian
//  template<class T>
//  inline Point3<T> rotatePoint(const T &angle, const Point3<T> &point,
//                              Vector3<T> axis)
//  {
//    Quaternion<T> quatRot;
//    Quaternion<T> quatResult;
//    
//    axis.normalise();
//    quatRot.setRot( angle, axis );
//    
//    //! TODO mettre une explication sur les quaternions...
//    //quatResult = (quatRot*point)*quatRot.getConjugate();
//    //point.setXYZ(quatResult.x(), quatResult.y(), quatResult.z());
//    
//    return ( quatRot*point ).multRotation( quatRot.getConjugate() );
//  }
//  
//  //---------------------------------------------------------------------------
//  template<class T>
//  inline Point3<T> rotatePoint(const Quaternion<T> &quat, const Point3<T> &point)
//  {
//    return (quat*point).multRotation(quat.getConjugate());
//  }
//  
//  //---------------------------------------------------------------------------
//  //l'angle est en radian
//  template<class T>
//  inline Point3<T> rotatePoint(const T &angle, const Point3<T> &point,
//                              Vector3<T> axis, const Point3<T> &axisPos)
//  {
//    axis.normalise();
//    
//    //On trouve la position relative du Point a tourner par rapport a l'axe
//    Point3<T> relPos, rotatedPoint;
//    
//    relPos = point - toVector(axisPos);
//    rotatedPoint = rotatePoint(angle, relPos, axis);
//    
//    //On retranslate le point rotater
//    rotatedPoint = rotatedPoint + toVector(axisPos);
//    
//    return rotatedPoint;
//  }
  
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
