//!-----------------------------------------------------------------------------
//! \file
//! \brief Classe \c Quaternion permet de reprÈsenter un quaternion
//!
//! AUTHOR:  Pierre-Olivier Beaudoin & David Pinson
//!-----------------------------------------------------------------------------
#ifndef QUARTERNION_H
#define QUARTERNION_H

#include <iostream>

#include "MathDef.h"
#include "Matrix4x4.h"
#include "Point.h"
#include "Vect.h"

//!-----------------------------------------------------------------------------
//! \brief Classe gÈrant les fonctionalitÈs associÈes a un quaternion
//!
//! un quaternion est de la forme q = w + xi + yj + zk
//!-----------------------------------------------------------------------------
namespace Realisim
{
  template<class T>
  class Quaternion
  {
  public:

    // --------------- constructeurs -------------------------------------------
    inline Quaternion();
    inline Quaternion(const T &w, const T &x, const T &y, const T &z);
    inline Quaternion(const Quaternion<T> &quat);
    inline Quaternion(const Matrix4<T>& iMat);

    // --------------- destructeurs --------------------------------------------
    inline ~Quaternion();

    // --------------- fonction set --------------------------------------------
    inline void setW(const T &w);
    inline void setX(const T &x);
    inline void setY(const T &y);
    inline void setZ(const T &z);
    inline void setQuat(const T &w, const T &x, const T &y, const T &z);

    // --------------- fonction get --------------------------------------------
    inline T getW() const;
    inline T getX() const;
    inline T getY() const;
    inline T getZ() const;
    inline void getQuat(T &w, T &x, T &y, T &z) const;
    inline Quaternion<T> getConjugate() const;
    inline double getLength() const;
    inline Matrix4<T> getUnitRotationMatrix() const;

    // --------------- fonction utiles -----------------------------------------
    inline void print() const;
    inline void conjugate();
    inline void setRot(const T &angle, const Vect<T>& iAxis);
    inline Point<T> multRotation( const Quaternion<T> &quat ) const;

    // --------------- Overload: operateurs unitaires --------------------------
    inline Quaternion<T>& operator=  (const Quaternion<T> &quat);
    inline Quaternion<T>  operator+  (const Quaternion<T> &quat) const;
    inline Quaternion<T>&  operator+=  (const Quaternion<T> &quat);
    inline Quaternion<T>  operator-  (const Quaternion<T> &quat) const;
    inline Quaternion<T>&  operator-=  (const Quaternion<T> &quat);
    inline Quaternion<T>  operator-  () const;
    inline Quaternion<T>  operator*  (T iValue) const;
    inline Quaternion<T>&  operator*=  (T iValue);
    inline Quaternion<T>  operator*  (const Quaternion<T> &quat) const;
    inline Quaternion<T>&  operator*=  (const Quaternion<T> &quat);

  protected:
  private:

    T w_;
    T x_;
    T y_;
    T z_;
  };

  //! constructeur par defaut
  template<class T>
  inline Quaternion<T>::Quaternion() : w_((T)1.0), x_((T)0), y_((T)0), z_((T)0)
  {
  }

  //! constructeur avec parametre
  template<class T>
  inline Quaternion<T>::Quaternion(const T &w, const T &x, const T &y,
    const T &z) : w_(w), x_(x), y_(y), z_(z)
  {
  }

  //! constructeur copie
  template<class T>
  inline Quaternion<T>::Quaternion(const Quaternion<T> &quat)
  {
    w_ = quat.w_;
    x_ = quat.x_;
    y_ = quat.y_;
    z_ = quat.z_;
  }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline Quaternion<T>::Quaternion(const Matrix4<T>& iMat)
  {
    w_ = 0.5 * ( sqrt( iMat[0][0] + iMat[1][1] + iMat[2][2] + iMat[3][3] ) );
    x_ = ( iMat[1][2] - iMat[2][1] ) / ( 4 * w_ );
    y_ = ( iMat[2][0] - iMat[0][2] ) / ( 4 * w_ );
    z_ = ( iMat[0][1] - iMat[1][0]) / ( 4 * w_ );
  }
  
  //! destructeur
  template<class T>
  inline Quaternion<T>::~Quaternion()
  {
  }

  template<class T>
  inline void Quaternion<T>::setW(const T &w)
  {
    w_ = w;
  }

  template<class T>
  inline void Quaternion<T>::setX(const T &x)
  {
    x_ = x;
  }

  template<class T>
  inline void Quaternion<T>::setY(const T &y)
  {
    y_ = y;
  }

  template<class T>
  inline void Quaternion<T>::setZ(const T &z)
  {
    z_ = z;
  }

  template<class T>
  inline void Quaternion<T>::setQuat(const T &w, const T &x, const T &y,
    const T &z)
  {
    w_ = w;
    x_ = x;
    y_ = y;
    z_ = z;
  }

  template<class T>
  inline T Quaternion<T>::getW() const
  {
    return w_;
  }

  template<class T>
  inline T Quaternion<T>::getX() const
  {
    return x_;
  }

  template<class T>
  inline T Quaternion<T>::getY() const
  {
    return y_;
  }

  template<class T>
  inline T Quaternion<T>::getZ() const
  {
    return z_;
  }

  template<class T>
  inline void Quaternion<T>::getQuat(T &w, T &x, T &y, T &z) const
  {
    w = w_;
    x = x_;
    y = y_;
    z = z_;
  }

  template<class T>
  inline void Quaternion<T>::print() const
  {
    std::cout<<"q = "<<w_<<" + "<<x_<<"i + "<<y_<<"j + "<<z_<<"k"<<std::endl;
  }

  template<class T>
  inline Quaternion<T>& Quaternion<T>::operator= (const Quaternion<T> &quat)
  {
    w_ = quat.w_;
    x_ = quat.x_;
    y_ = quat.y_;
    z_ = quat.z_;
    return *this;
  }

  template<class T>
  inline Point<T> Quaternion<T>::multRotation(const Quaternion<T> &quat) const
  {
    Point<T> result;

    result.setX(w_*quat.x_ + x_*quat.w_ + y_*quat.z_ - z_*quat.y_);
    result.setY(w_*quat.y_ - x_*quat.z_ + y_*quat.w_ + z_*quat.x_);
    result.setZ(w_*quat.z_ + x_*quat.y_ - y_*quat.x_ + z_*quat.w_);

    return result;
  }

  //----------------------------------------------------------------------------
  //addition de quaternion
  template<class T>
  inline Quaternion<T> Quaternion<T>::operator+  (const Quaternion<T> &quat) const
  {
    
    Quaternion<T> result(*this);
    result += quat;
    return result;
  }

  //----------------------------------------------------------------------------
  //addition de quaternion
  template<class T>
  inline Quaternion<T>& Quaternion<T>::operator+=  (const Quaternion<T> &quat)
  {
    w_ += quat.w_;
    x_ += quat.x_;
    y_ += quat.y_;
    z_ += quat.z_;
    return *this;
  }

  //----------------------------------------------------------------------------
  //soustraction de quaternion
  template<class T>
  inline Quaternion<T> Quaternion<T>::operator-  (const Quaternion<T> &quat) const
  {
    
    Quaternion<T> result(*this);
    result -= quat;
    return result;
  }
  
  //----------------------------------------------------------------------------
  //soustraction de quaternion
  template<class T>
  inline Quaternion<T>& Quaternion<T>::operator-=  (const Quaternion<T> &quat)
  {
    w_ -= quat.w_;
    x_ -= quat.x_;
    y_ -= quat.y_;
    z_ -= quat.z_;
    return *this;
  }
  
  //----------------------------------------------------------------------------
  //inversion du quaternion
  template<class T>
  inline Quaternion<T> Quaternion<T>::operator-  () const
  {
    
    Quaternion<T> result(*this);
    result *= -1;
    return result;
  }
  
  //----------------------------------------------------------------------------
  //multiplication par un scalaire
  template<class T>
  inline Quaternion<T> Quaternion<T>::operator*  (T iValue) const
  {
    Quaternion<T> result(*this);
    return result *= iValue;
  }
  
  //----------------------------------------------------------------------------
  //multiplication par un scalaire
  template<class T>
  inline Quaternion<T>& Quaternion<T>::operator*=  (T iValue)
  {    
    w_ = w_ * iValue;
    x_ = x_ * iValue;
    y_ = y_ * iValue;
    z_ = z_ * iValue;
    return *this;
  }

  //----------------------------------------------------------------------------
  template<class T>
  inline Quaternion<T>  Quaternion<T>::operator*  (const Quaternion<T> &quat) const
  {
    Quaternion<T> result(*this);
    return result *= quat;
  }

  //----------------------------------------------------------------------------
  template<class T>
  inline Quaternion<T>&  Quaternion<T>::operator*=  (const Quaternion<T> &quat)
  {    
    w_ = w_*quat.w_ - x_*quat.x_ - y_*quat.y_ - z_*quat.z_;
    x_ = w_*quat.x_ + x_*quat.w_ + y_*quat.z_ - z_*quat.y_;
    y_ = w_*quat.y_ - x_*quat.z_ + y_*quat.w_ + z_*quat.x_;
    z_ = w_*quat.z_ + x_*quat.y_ - y_*quat.x_ + z_*quat.w_;
    return *this;
  }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline Quaternion<T> Quaternion<T>::getConjugate() const
  {
    return Quaternion<T>(w_, -x_, -y_, -z_);
  }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline double Quaternion<T>::getLength() const
  {
    return (double)sqrt(w_*w_ + x_*x_ + y_*y_ + z_*z_);
  }


  //Cette fonction remplie la matrice unitRotationMatrix avec la matrice de
  //rotation correspondant au quaternion.
  template<class T>
  inline Matrix4<T>
  Quaternion<T>::getUnitRotationMatrix() const
  {
    Matrix4<T> mat; //matrice identité
    mat.setRow1(
          1-(2*y_*y_)-(2*z_*z_),
          (2*x_*y_)+(2*w_*z_),
          (2*x_*z_)-(2*w_*y_),
          0 );
    mat.setRow2(
          (2*x_*y_)-(2*w_*z_),
          1-(2*x_*x_)-(2*z_*z_),
          (2*y_*z_)+(2*w_*x_),
          0 );
    mat.setRow3(
          (2*x_*z_)+(2*w_*y_),
          (2*y_*z_)-(2*w_*x_),
          1-(2*x_*x_)-(2*y_*y_),
          0 );
    mat.setRow4(
          0,
          0,
          0,
          1 );
    return mat;
  }

  //Cette fonction est a utiliser seulement si le quaternion est unitaire
  template<class T>
  inline void Quaternion<T>::conjugate()
  {
    x_ = -x_;
    y_ = -y_;
    z_ = -z_;
  }

  //!---------------------------------------------------------------------------
  //! \brief  Fonction permettant d'initialiser un quaternion pour une rotation.
  //!
  //! ATTENTION POUR FONCTIONNER ADEQUATEMENT L'AXE PASSE EN PARAMETRE SOIT
  //! IMPERATIVEMENT ETRE NORMALISE!!!
  //!
  //! \param angle angle que l'on doit tourner autour de l'axe EN RADIAN!!!
  //! \param axisX
  //! \param axisY le vecteur a tourner autour...
  //! \param axisZ
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Quaternion<T>::setRot(const T &angle, const Vect<T>& iAxis)
  {
    T sinTmp = std::sin(angle/(T)(2.0));

    x_ = iAxis.getX() * sinTmp;
    y_ = iAxis.getY() * sinTmp;
    z_ = iAxis.getZ() * sinTmp;
    w_ = std::cos(angle/(T)(2.0));
  }

  typedef Quaternion<float>   Quat4f;
  typedef Quaternion<double>  Quat4d;
  //NE PAS FAIRE DE QUATERNION DE INT!!

} // fin du namespace realisim

#endif // QUARTERNION_H
