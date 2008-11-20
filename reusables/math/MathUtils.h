//!-----------------------------------------------------------------------------
//! \file
//! \brief Fichier contenant des fonctions mathématique utile.
//!
//! AUTHOR:  Pierre-Olivier Beaudoin & David Pinson
//!-----------------------------------------------------------------------------
#ifndef MATH_UTILE_H
#define MATH_UTILE_H

#include "Quaternion.h"
#include "Point.h"

namespace Realisim
{
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

  template<class V>
  inline Point<V> operator+ (const Point<V> &point, const Vect<V> &vect)
  {
    Point<V> result;

    result.setX(point.getX() + vect.getX());
    result.setY(point.getY() + vect.getY());
    result.setZ(point.getZ() + vect.getZ());

    return result;
  }

  template<class V>
  inline Point<V> operator- (const Point<V> &point, const Vect<V> &vect)
  {
    Point<V> result;

    result.setX(point.getX() - vect.getX());
    result.setY(point.getY() - vect.getY());
    result.setZ(point.getZ() - vect.getZ());

    return result;
  }

  template<class T>
  inline Point<T> rotatePoint(const T &angle, const Point<T> &point,
                              const Vect<T> axis)
  {
    Quat4f quatRot;
    Quat4f quatResult;

    quatRot.setRot(angle, axis.getX(), axis.getY(), axis.getZ());

    //! TODO mettre une explication sur les quaternions...
    //quatResult = (quatRot*point)*quatRot.getConjugate();
    //point.setXYZ(quatResult.getX(), quatResult.getY(), quatResult.getZ());

    return (quatRot*point).multRotation(quatRot.getConjugate());
  }

  template<class T>
  inline Point<T> rotatePoint(const Quaternion<T> &quat, const Point<T> &point)
  {
    return (quat*point).multRotation(quat.getConjugate());
  }

  template<class T>
  inline Point<T> rotatePoint(const T &angle, const Point<T> &point,
                              const Vect<T> axis, const Point<T> &axisPos)
  {
    //On trouve la position relative du Point a tourner par rapport a l'axe
    Point3f relPos, rotatedPoint;

    relPos = point - axisPos;
    rotatedPoint = RotatePoint(angle, relPos, axis);

    //On retranslate le point rotater
    rotatedPoint += axisPos;

    return rotatedPoint;
  }
} // fin du namespace realisim

#endif // MATH_UTILE_H
