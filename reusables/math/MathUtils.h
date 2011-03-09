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
#include "Quaternion.h"

namespace realisim
{
namespace math
{

  //---------------------------------------------------------------------------
  //convertie un vecteur en point
  template<class T>
  inline Point<T> toPoint (const Vect<T>& iV)
  {
    return Point<T>(iV.getX(), iV.getY(), iV.getZ());
  }
  
  //---------------------------------------------------------------------------
  //convertie un point en vecteur
  template<class T>
  inline Vect<T> toVector (const Point<T>& iP)
  {
    return Vect<T>(iP.getX(), iP.getY(), iP.getZ());
  }

  //---------------------------------------------------------------------------
  template<class V>
  inline Quaternion<V> operator* (const Quaternion<V> &quat,
                                  const Point<V> &point)
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
  inline Vect<T> operator* ( const Vect<T>& iVect, const Matrix4<T>& iMat)
  {
    Vect<T> vect;
    vect.setX( iVect.getX() * iMat(0, 0) + iVect.getY() * iMat(1, 0) + iVect.getZ() * iMat(2, 0) );
    vect.setY( iVect.getX() * iMat(0, 1) + iVect.getY() * iMat(1, 1) + iVect.getZ() * iMat(2, 1) );
    vect.setZ( iVect.getX() * iMat(0, 2) + iVect.getY() * iMat(1, 2) + iVect.getZ() * iMat(2, 2) );
    return vect;
  }
  
  //---------------------------------------------------------------------------
  /*On multiplie le point par la partie de rotation et on ajoute la translation
    au point.*/
  template<class T>
  inline Point<T> operator* ( const Point<T>& iPoint, const Matrix4<T>& iMat)
  {
    Point<T> result;
    result.setX( iPoint.getX() * iMat(0, 0) + iPoint.getY() * iMat(1, 0) + iPoint.getZ() * iMat(2, 0) );
    result.setY( iPoint.getX() * iMat(0, 1) + iPoint.getY() * iMat(1, 1) + iPoint.getZ() * iMat(2, 1) );
    result.setZ( iPoint.getX() * iMat(0, 2) + iPoint.getY() * iMat(1, 2) + iPoint.getZ() * iMat(2, 2) );
    //on ajoute la translation au point
    result += Point<T>(iMat(3, 0), iMat(3, 1), iMat(3, 2));
    return result;
  }
  
  //---------------------------------------------------------------------------
  template<class V>
  inline Point<V> operator+ (const Point<V> &point, const Vect<V> &vect)
  {
    Point<V> result;
    
    result.setX(point.getX() + vect.getX());
    result.setY(point.getY() + vect.getY());
    result.setZ(point.getZ() + vect.getZ());
    
    return result;
  }
  
  //---------------------------------------------------------------------------
  template<class V>
  inline Point<V> operator- (const Point<V> &point, const Vect<V> &vect)
  {
    Point<V> result;
    
    result.setX(point.getX() - vect.getX());
    result.setY(point.getY() - vect.getY());
    result.setZ(point.getZ() - vect.getZ());
    
    return result;
  }
  
  //---------------------------------------------------------------------------
  template<class V>
  inline Point<V> operator* (const Point<V> &point, const Vect<V> &vect)
  {
    Point<V> result;
    
    result.setX(point.getX() * vect.getX());
    result.setY(point.getY() * vect.getY());
    result.setZ(point.getZ() * vect.getZ());
    
    return result;
  }
  
  //---------------------------------------------------------------------------
  template<class V>
  inline Vect<V> operator* (const V& iVal, const Vect<V>& vect)
  {
    Vect<V> result;
    
    result.setX(iVal * vect.getX());
    result.setY(iVal * vect.getY());
    result.setZ(iVal * vect.getZ());
    
    return result;
  }
  
  //---------------------------------------------------------------------------
  //retourne la matrice de rotation correpondant a la rotation de iAngle
  //radian autour de l'axe iAxis
  template<class T>
  inline Matrix4<T> getRotationMatrix( double iAngle,
                                       Vect<T> iAxis )
  {
    iAxis.normalise();
    Quaternion<T> quat;
    quat.setRot(iAngle, iAxis);
    
    return quat.getUnitRotationMatrix();
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
    iterationMatrix.setTranslation( iM1.getTranslation()*( 1 - iT ) + 
      iM2.getTranslation()*( iT ) );
    return iterationMatrix;
  }
  
  //---------------------------------------------------------------------------
  template<class T>
  inline Point<T> rotatePoint(const T &angle, const Point<T> &point,
                              Vect<T> axis)
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
  inline Point<T> rotatePoint(const Quaternion<T> &quat, const Point<T> &point)
  {
    return (quat*point).multRotation(quat.getConjugate());
  }
  
  //---------------------------------------------------------------------------
  template<class T>
  inline Point<T> rotatePoint(const T &angle, const Point<T> &point,
                              Vect<T> axis, const Point<T> &axisPos)
  {
    axis.normalise();
    
    //On trouve la position relative du Point a tourner par rapport a l'axe
    Point<T> relPos, rotatedPoint;
    
    relPos = point - axisPos;
    rotatedPoint = rotatePoint(angle, relPos, axis);
    
    //On retranslate le point rotater
    rotatedPoint += axisPos;
    
    return rotatedPoint;
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
    
} //math
} // fin du namespace realisim

#endif // MATH_UTILE_H
