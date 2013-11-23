//!-----------------------------------------------------------------------------
//! \file
//! \brief Fichier contenant des fonctions mathÈmatique utile.
//!
//! AUTHOR:  Pierre-Olivier Beaudoin & David Pinson
//!-----------------------------------------------------------------------------
#ifndef MATH_UTILE_H
#define MATH_UTILE_H

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
	//---------------------------------------------------------------------------
  template< class T >
  Vector3<T> absolute( const Vector3<T>& iP )
  { return Vector3<T>( abs( iP.getX() ), abs( iP.getY() ), abs( iP.getZ() ) ); }
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
    T dx = iV0.getX() - iV1.getX();
    T dy = iV0.getY() - iV1.getY();
    T dz = iV0.getZ() - iV1.getZ();
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
    T dx = iP0.getX() - iP1.getX();
    T dy = iP0.getY() - iP1.getY();
    T dz = iP0.getZ() - iP1.getZ();
    if(dx<(T)0.0) dx = -dx;
    if(dy<(T)0.0) dy = -dy;
    if(dz<(T)0.0) dz = -dz;
    return (dx <= iEpsilon && dy <= iEpsilon && dz <= iEpsilon);
  }
  //---------------------------------------------------------------------------
  //convertie un vecteur en point
  template<class T>
  inline Point3<T> toPoint (const Vector3<T>& iV)
  { return Point3<T>(iV.getX(), iV.getY(), iV.getZ()); }
  
  //---------------------------------------------------------------------------
  //convertie un point en vecteur
  template<class T>
  inline Vector3<T> toVector (const Point3<T>& iP)
  { return Vector3<T>(iP.getX(), iP.getY(), iP.getZ()); }
  
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
    
    result.setW(- (quat.getX() * point.getX()) - (quat.getY() * point.getY()) -
                (quat.getZ() * point.getZ()) );
    
    result.setX( (quat.getW() * point.getX()) + (quat.getY() * point.getZ()) -
                (quat.getZ() * point.getY()) );
    
    result.setY( (quat.getW() * point.getY()) - (quat.getX() * point.getZ()) +
                (quat.getZ() * point.getX()) );
    
    result.setZ( (quat.getW() * point.getZ()) + (quat.getX() * point.getY()) -
                (quat.getY() * point.getX()) );
    
    return result;
  }
  
    //---------------------------------------------------------------------------
  /*Le vecteur multiplié par la matrice ne fait que modifié sont orientation,
    on n'ajoute pas la translation.*/
  template<class T>
  inline Vector3<T> operator* ( const Vector3<T>& iVect, const Matrix4<T>& iMat)
  {
    Vector3<T> vect;
    vect.setX( iVect.getX() * iMat(0, 0) + iVect.getY() * iMat(1, 0) + iVect.getZ() * iMat(2, 0) );
    vect.setY( iVect.getX() * iMat(0, 1) + iVect.getY() * iMat(1, 1) + iVect.getZ() * iMat(2, 1) );
    vect.setZ( iVect.getX() * iMat(0, 2) + iVect.getY() * iMat(1, 2) + iVect.getZ() * iMat(2, 2) );
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
    result.setX( iPoint.getX() * iMat(0, 0) + iPoint.getY() * iMat(1, 0) + iPoint.getZ() * iMat(2, 0) );
    result.setY( iPoint.getX() * iMat(0, 1) + iPoint.getY() * iMat(1, 1) + iPoint.getZ() * iMat(2, 1) );
    result.setZ( iPoint.getX() * iMat(0, 2) + iPoint.getY() * iMat(1, 2) + iPoint.getZ() * iMat(2, 2) );
    //on ajoute la translation au point
    result += Point3<T>(iMat(3, 0), iMat(3, 1), iMat(3, 2));
    return result;
  }
  //---------------------------------------------------------------------------
  template<class T>
  inline Point3<T> operator* (const T& iVal, const Point3<T>& iPoint)
  {
  	return Point3<T>( iVal * iPoint.getX(),
    	iVal * iPoint.getY(),
      iVal * iPoint.getZ() );
  }
  //---------------------------------------------------------------------------
  template<class T>
  inline Point3<T> operator+ (const Point3<T> &point, const Vector3<T> &vect)
  {
  	return Point3<T>( point.getX() + vect.getX(),
    	point.getY() + vect.getY(),
    	point.getZ() + vect.getZ() );
  }
  //---------------------------------------------------------------------------
  template<class T>
  inline Point3<T> operator- (const Point3<T> &point, const Vector3<T> &vect)
  {
    return Point3<T>( point.getX() - vect.getX(),
    	point.getY() - vect.getY(),
    	point.getZ() - vect.getZ() );
  }
  //---------------------------------------------------------------------------
  template<class T>
  inline Vector3<T> operator+ (const Point3<T> &p1, const Point3<T> &p2)
  {
    return Vector3<T>( p1.getX() + p2.getX(),
    	p1.getY() + p2.getY(),
      p1.getZ() + p2.getZ() );
  }
  //---------------------------------------------------------------------------
  template<class T>
  inline Vector3<T> operator- (const Point3<T> &p1, const Point3<T> &p2)
  {
    return Vector3<T>( p1.getX() - p2.getX(),
    	p1.getY() - p2.getY(),
      p1.getZ() - p2.getZ() );
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
    
    result.setX(iVal * vect.getX());
    result.setY(iVal * vect.getY());
    result.setZ(iVal * vect.getZ());
    
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
    if( !isEqual( iV.getX(), 0.0 ) )
    	r = Vector3<T>( iV.getY(), -iV.getX(), (T)0.0 );
    else if( !isEqual( iV.getY(), 0.0 ) ) 
    	r = Vector3<T>( -iV.getY(), iV.getX(), (T)0.0 );
    else if( !isEqual( iV.getZ(), 0.0 ) )
			r = Vector3<T>( (T)0.0, iV.getZ(), -iV.getY() );
  	return r;
  }
  //---------------------------------------------------------------------------
  //retourne la matrice de rotation correpondant a la rotation de iAngle
  //radian autour de l'axe iAxis
  template<class T>
  inline Matrix4<T> getRotationMatrix( double iAngle,
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
  template<class T>
  inline Matrix4<T> interpolate(const Matrix4<T>& iM1, const Matrix4<T>& iM2, 
    double iT)
  {
  	Quat4d q1( iM1 );
    Quat4d q2( iM2 );
    //on compare avec les longueurs pour prendre le plus petit angle
    if( (-q2-q1).getLength() < (q2-q1).getLength() )
    {
      q2 = -q2;
    }
    
    q2 = q1*( 1 - iT ) + q2*iT;
    Matrix4d iterationMatrix = q2.getUnitRotationMatrix();
    
    //trouver la translation totale a effectuer
    iterationMatrix.setTranslation( toPoint(
    	iM1.getTranslation()*( 1 - iT ) + 
      iM2.getTranslation()*( iT ) ) );
    return iterationMatrix;
  }
  
  //---------------------------------------------------------------------------
  template<class T>
  inline Point3<T> rotatePoint(const T &angle, const Point3<T> &point,
                              Vector3<T> axis)
  {
    Quaternion<T> quatRot;
    Quaternion<T> quatResult;
    
    axis.normalise();
    quatRot.setRot( angle, axis );
    
    //! TODO mettre une explication sur les quaternions...
    //quatResult = (quatRot*point)*quatRot.getConjugate();
    //point.setXYZ(quatResult.getX(), quatResult.getY(), quatResult.getZ());
    
    return ( quatRot*point ).multRotation( quatRot.getConjugate() );
  }
  
  //---------------------------------------------------------------------------
  template<class T>
  inline Point3<T> rotatePoint(const Quaternion<T> &quat, const Point3<T> &point)
  {
    return (quat*point).multRotation(quat.getConjugate());
  }
  
  //---------------------------------------------------------------------------
  template<class T>
  inline Point3<T> rotatePoint(const T &angle, const Point3<T> &point,
                              Vector3<T> axis, const Point3<T> &axisPos)
  {
    axis.normalise();
    
    //On trouve la position relative du Point a tourner par rapport a l'axe
    Point3<T> relPos, rotatedPoint;
    
    relPos = point - toVector(axisPos);
    rotatedPoint = rotatePoint(angle, relPos, axis);
    
    //On retranslate le point rotater
    rotatedPoint = rotatedPoint + toVector(axisPos);
    
    return rotatedPoint;
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
//printf("n0; %.16f, %.16f, %.16f\n", n0.getX(), n0.getY(), n0.getZ() );
      for( int i = 2; i < (int)iP.size(); ++i )
      {
      	iMinus1 = i -1; i0 = i; i1 = (i + 1) % iP.size();
        n1 = Vector3d( *iP[iMinus1], *iP[i0] ) ^ Vector3d( *iP[i0], *iP[i1] );
        n1.normalise();
//printf("n1; %.16f, %.16f, %.16f\n", n1.getX(), n1.getY(), n1.getZ() );
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
  
  //----------------------------------------------------------------------------
  /* applique l'équation d'une distribution normale.
  voir 
  https://en.wikipedia.org/wiki/Normal_distribution#Standard_normal_distribution
  */
  template< typename T >
  T normalDistribution( double x, double s, double u = 0.0 )
  {
    double e = 2.718281828459045235360287471352662497757247093;
    double sigmaSquare = s*s;
    T r = ( 1.0 / sqrt( 2.0 * 3.1415629f * sigmaSquare) ) * 
      pow(e, -( (x * x - u * u) / (2.0 * sigmaSquare) ) );
    return r;
  }

  //----------------------------------------------------------------------------
  template< typename T >
  vector<T> gaussianKernel1D( int iKernelSize, double s )
  {
    vector<T> r; r.resize( iKernelSize );
    int i, j = 0;
    for(i = -iKernelSize / 2; i <= iKernelSize / 2; ++i)
    {
      r[j] = normalDistribution<T>( i, s);
      ++j;
    }
    return r;
  }
  //----------------------------------------------------------------------------
  /*retourne un filtre gaussien 2d avec l'arrangement mémoire suivant (pour
    un kernel size de 3):
    a00, a01, a02,
    a10, a11, a12,
    a20, a21, a22
    */
  template< typename T >
  vector< T > gaussianKernel2D( int iKernelSize, double s )
  {
  	iKernelSize = iKernelSize % 2 == 0 ? iKernelSize - 1 : iKernelSize;
    vector< T > r; r.resize( iKernelSize * iKernelSize );
        
    int i = 0, j = 0, ki, kj;
    for( j = 0, kj = -iKernelSize / 2; kj <= iKernelSize / 2; kj++, j++ )
    {
      for( i = 0, ki = -iKernelSize / 2; ki <= iKernelSize / 2; ki++, i++ )
      {
        r[ j * iKernelSize + i ] = normalDistribution<T>( ki, s ) * normalDistribution<T>( kj, s );
        printf("%f ", r[ j * iKernelSize + i ]);
      }
    printf("\n");
    }
    return r;
  }
  //----------------------------------------------------------------------------
  /*retourne un filtre moyen 2d voir gaussianKernel2D
    */
  template< typename T >
  vector< T > meanKernel2D( int iKernelSize )
  {
    vector< T > r; r.resize( iKernelSize * iKernelSize );
        
    int i, j;
    for( j = 0; j < iKernelSize; j++ )
    {
      for( i = 0; i < iKernelSize ; i++ )
      {
        r[ j * iKernelSize + i ] = 1.0 / (iKernelSize * iKernelSize);
        printf("%f ", r[ j * iKernelSize + i ]);
      }
    printf("\n");
    }
    return r;
  }


    
} //math
} // fin du namespace realisim

#endif // MATH_UTILE_H
