//!-----------------------------------------------------------------------------
//! \file
//! \brief Classe \c Matrix permet de reprÈsenter une matrice 4x4 ou 3x3
//!
//! AUTHOR:  Pierre-Olivier Beaudoin & David Pinson
//!-----------------------------------------------------------------------------
#ifndef MATRIX_4_H
#define MATRIX_4_H

#include <cassert>
#include "Point.h"
#include "Vect.h"
#include <vector>
#include <iostream>

//!-----------------------------------------------------------------------------
//! \brief Classe gÈrant les fonctionalitÈs associÈes a une matrice 4x4 ou 3x3
//!-----------------------------------------------------------------------------
namespace realisim
{
namespace math
{
  template<class T>
  class Matrix4
  {
  public:

    // --------------- constructeurs -------------------------------------------
    inline Matrix4();
    inline Matrix4(const T &a11, const T &a12, const T &a13, const T &a14,
                  const T &a21, const T &a22, const T &a23, const T &a24,
                  const T &a31, const T &a32, const T &a33, const T &a34,
                  const T &a41, const T &a42, const T &a43, const T &a44);
    inline Matrix4(const Matrix4 &matrix);

    // --------------- destructeurs --------------------------------------------
    ~Matrix4();

    // --------------- fonction set --------------------------------------------
    inline void setRow1(const T &a11, const T &a12, const T &a13, const T &a14);
    inline void setRow2(const T &a21, const T &a22, const T &a23, const T &a24);
    inline void setRow3(const T &a31, const T &a32, const T &a33, const T &a34);
    inline void setRow4(const T &a41, const T &a42, const T &a43, const T &a44);
    
    inline void setTranslation( const Point<T>& iTrans );

    // --------------- fonction get --------------------------------------------
    inline Vect<T> getBaseX() const;
    inline Vect<T> getBaseY() const;
    inline Vect<T> getBaseZ() const;
    inline Point<T> getTranslation() const;

    // --------------- fonction utiles -----------------------------------------
    inline void multEquMat3(const Matrix4 &matrix);
    inline void loadIdentity();
    inline void print() const;
    inline const T* getPtr() const;

    // --------------- Overload: operateurs unitaires --------------------------
    inline Matrix4<T>& operator=  (const Matrix4 &matrix);

    inline Matrix4<T>  operator+  (const Matrix4 &matrix) const;
    inline Matrix4<T>& operator+= (const Matrix4 &matrix);

    inline Matrix4<T>  operator-  (const Matrix4 &matrix) const;
    inline Matrix4<T>& operator-= (const Matrix4 &matrix);

    inline Matrix4<T>  operator*  (const Matrix4 &matrix) const;
    inline void operator*= (const Matrix4 &matrix);
    
    inline const std::vector<T> operator[] ( unsigned int iIndex ) const;

  protected:
  private:

    T mat_[16];
  };

  //! constructeur par dÈfaut.
  template<class T>
  inline Matrix4<T>::Matrix4()
  {
    loadIdentity();
  }

  //! constructeur avec parametre
  template<class T>
  inline Matrix4<T>::Matrix4(
    const T &a11, const T &a12, const T &a13, const T &a14,
    const T &a21, const T &a22, const T &a23, const T &a24,
    const T &a31, const T &a32, const T &a33, const T &a34,
    const T &a41, const T &a42, const T &a43, const T &a44)
  {
    mat_[0]  = a11; mat_[1]  = a12; mat_[2]  = a13; mat_[3]  = a14;
    mat_[4]  = a21; mat_[5]  = a22; mat_[6]  = a23; mat_[7]  = a24;
    mat_[8]  = a31; mat_[9]  = a32; mat_[10] = a33; mat_[11] = a34;
    mat_[12] = a41; mat_[13] = a42; mat_[14] = a43; mat_[15] = a44;
  }

  //! constructeur copie
  template<class T>
  inline Matrix4<T>::Matrix4(const Matrix4 &matrix)
  {
    memcpy( (void*)mat_, (void*)matrix.mat_, 16*sizeof(T) );
  }

  //! destructeur
  template<class T>
  Matrix4<T>::~Matrix4()
  {
  }

  //! permet de setter la 1er ligne
  template<class T>
  inline void Matrix4<T>::setRow1(const T &a11, const T &a12, const T &a13,
    const T &a14)
  {
    mat_[0] = a11; mat_[1] = a12; mat_[2] = a13; mat_[3] = a14;
  }

  //! permet de setter la 2e ligne
  template<class T>
  inline void Matrix4<T>::setRow2(const T &a21, const T &a22, const T &a23,
    const T &a24)
  {
    mat_[4] = a21; mat_[5] = a22; mat_[6] = a23; mat_[7] = a24;
  }

  //! permet de setter la 3e ligne
  template<class T>
  inline void Matrix4<T>::setRow3(const T &a31, const T &a32, const T &a33,
    const T &a34)
  {
    mat_[8] = a31; mat_[9] = a32; mat_[10] = a33; mat_[11] = a34;
  }

  //! permet de setter la 4e ligne
  template<class T>
  inline void Matrix4<T>::setRow4(const T &a41, const T &a42, const T &a43,
    const T &a44)
  {
    mat_[12] = a41; mat_[13] = a42; mat_[14] = a43; mat_[15] = a44;
  }
  
  //! permet de setter la 4e ligne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline void Matrix4<T>::setTranslation(const Point<T>& iTrans)
  {
    mat_[12] = iTrans.getX(); mat_[13] = iTrans.getY(); mat_[14] = iTrans.getZ();
  }

  //! permet d'obtenir la 1er ligne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline Vect<T> Matrix4<T>::getBaseX() const
  {
    return Vect<T>( mat_[0], mat_[1], mat_[2]);
  }

  //! permet d'obtenir la 2e ligne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline Vect<T> Matrix4<T>::getBaseY() const
  {
    return Vect<T>( mat_[4], mat_[5], mat_[6]);
  }

  //! permet d'obtenir la 3e ligne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline Vect<T> Matrix4<T>::getBaseZ() const
  {
   return Vect<T>( mat_[8], mat_[9], mat_[10]);
  }
  
  //! permet d'obtenir la 3e ligne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline Point<T> Matrix4<T>::getTranslation() const
  {
    return Point<T>( mat_[12], mat_[13], mat_[14]);
  }

  template<class T>
  inline void Matrix4<T>::multEquMat3(const Matrix4 &matrix)
  {
    T a11, a12, a13;
    T a21, a22, a23;
    T a31, a32, a33;

    a11 = mat_[0]*matrix.mat_[0] + mat_[1]*matrix.mat_[4] + mat_[2]*matrix.mat_[8];
    a12 = mat_[0]*matrix.mat_[1] + mat_[1]*matrix.mat_[5] + mat_[2]*matrix.mat_[9];
    a13 = mat_[0]*matrix.mat_[2] + mat_[1]*matrix.mat_[6] + mat_[2]*matrix.mat_[10];

    a21 = mat_[4]*matrix.mat_[0] + mat_[5]*matrix.mat_[4] + mat_[6]*matrix.mat_[8];
    a22 = mat_[4]*matrix.mat_[1] + mat_[5]*matrix.mat_[5] + mat_[6]*matrix.mat_[9];
    a23 = mat_[4]*matrix.mat_[2] + mat_[5]*matrix.mat_[6] + mat_[6]*matrix.mat_[10];

    a31 = mat_[8]*matrix.mat_[0] + mat_[9]*matrix.mat_[4] + mat_[10]*matrix.mat_[8];
    a32 = mat_[8]*matrix.mat_[1] + mat_[9]*matrix.mat_[5] + mat_[10]*matrix.mat_[9];
    a33 = mat_[8]*matrix.mat_[2] + mat_[9]*matrix.mat_[6] + mat_[10]*matrix.mat_[10];

    setRow1_3(a11, a12, a13);
    setRow2_3(a21, a22, a23);
    setRow3_3(a31, a32, a33);
  }

  //! fonction permettant de charcher la matrice identitÈ
  template<class T>
  inline void Matrix4<T>::loadIdentity()
  {
    mat_[0]  = (T)1; mat_[1]  = (T)0; mat_[2]  = (T)0; mat_[3]  = (T)0;
    mat_[4]  = (T)0; mat_[5]  = (T)1; mat_[6]  = (T)0; mat_[7]  = (T)0;
    mat_[8]  = (T)0; mat_[9]  = (T)0; mat_[10] = (T)1; mat_[11] = (T)0;
    mat_[12] = (T)0; mat_[13] = (T)0; mat_[14] = (T)0; mat_[15] = (T)1;
  }

  //! fonction qui affiche le contenu de la matrice utile pour le dÈbugage
  template<class T>
  inline void Matrix4<T>::print() const
  {
    std::cout<<mat_[0]<<" "<<mat_[1]<<" "<<mat_[2]<<" "<<mat_[3]<<std::endl;
    std::cout<<mat_[4]<<" "<<mat_[5]<<" "<<mat_[6]<<" "<<mat_[7]<<std::endl;
    std::cout<<mat_[8]<<" "<<mat_[9]<<" "<<mat_[10]<<" "<<mat_[11]<<std::endl;
    std::cout<<mat_[12]<<" "<<mat_[13]<<" "<<mat_[14]<<" "<<mat_[15]<<std::endl;
  }

  //! fct qui retourne un pointeur sur la matrice (pratique pour OGL par ex.)
  template<class T>
  inline const T* Matrix4<T>::getPtr() const
  {
    return &mat_[0];
  }

  //! surcharge operateur =
  template<class T>
  inline Matrix4<T>& Matrix4<T>::operator= (const Matrix4 &matrix)
  {
    memcpy( (void*)mat_, (void*)matrix.mat_, 16*sizeof(T) );
    return *this;
  }

  //! TODO surchage operateur +
  template<class T>
  inline Matrix4<T> Matrix4<T>::operator+ (const Matrix4 &matrix) const
  {
  }

  //! TODO surchage operateur +=
  template<class T>
  inline Matrix4<T>& Matrix4<T>::operator+= (const Matrix4 &matrix)
  {
  }

  //! TODO surchage operateur -
  template<class T>
  inline Matrix4<T> Matrix4<T>::operator- (const Matrix4 &matrix) const
  {
  }

  //! TODO surchage operateur -=
  template<class T>
  inline Matrix4<T>& Matrix4<T>::operator-= (const Matrix4 &matrix)
  {
  }

  //! surchage operateur *
  template<class T>
  inline Matrix4<T> Matrix4<T>::operator* (const Matrix4 &matrix) const
  {
    Matrix4 tmp;
    T a, b, c, d;

    a = mat_[0]*matrix.mat_[0] + mat_[1]*matrix.mat_[4] + mat_[2]*matrix.mat_[8] + mat_[3]*matrix.mat_[12];
    b = mat_[0]*matrix.mat_[1] + mat_[1]*matrix.mat_[5] + mat_[2]*matrix.mat_[9] + mat_[3]*matrix.mat_[13];
    c = mat_[0]*matrix.mat_[2] + mat_[1]*matrix.mat_[6] + mat_[2]*matrix.mat_[10] + mat_[3]*matrix.mat_[14];
    d = mat_[0]*matrix.mat_[3] + mat_[1]*matrix.mat_[7] + mat_[2]*matrix.mat_[11] + mat_[3]*matrix.mat_[15];

    tmp.setRow1(a, b, c, d);

    a = mat_[4]*matrix.mat_[0] + mat_[5]*matrix.mat_[4] + mat_[6]*matrix.mat_[8] + mat_[7]*matrix.mat_[12];
    b = mat_[4]*matrix.mat_[1] + mat_[5]*matrix.mat_[5] + mat_[6]*matrix.mat_[9] + mat_[7]*matrix.mat_[13];
    c = mat_[4]*matrix.mat_[2] + mat_[5]*matrix.mat_[6] + mat_[6]*matrix.mat_[10] + mat_[7]*matrix.mat_[14];
    d = mat_[4]*matrix.mat_[3] + mat_[5]*matrix.mat_[7] + mat_[6]*matrix.mat_[11] + mat_[7]*matrix.mat_[15];

    tmp.setRow2(a, b, c, d);

    a = mat_[8]*matrix.mat_[0] + mat_[9]*matrix.mat_[4] + mat_[10]*matrix.mat_[8] + mat_[11]*matrix.mat_[12];
    b = mat_[8]*matrix.mat_[1] + mat_[9]*matrix.mat_[5] + mat_[10]*matrix.mat_[9] + mat_[11]*matrix.mat_[13];
    c = mat_[8]*matrix.mat_[2] + mat_[9]*matrix.mat_[6] + mat_[10]*matrix.mat_[10] + mat_[11]*matrix.mat_[14];
    d = mat_[8]*matrix.mat_[3] + mat_[9]*matrix.mat_[7] + mat_[10]*matrix.mat_[11] + mat_[11]*matrix.mat_[15];

    tmp.setRow3(a, b, c, d);

    a = mat_[12]*matrix.mat_[0] + mat_[13]*matrix.mat_[4] + mat_[14]*matrix.mat_[8] + mat_[15]*matrix.mat_[12];
    b = mat_[12]*matrix.mat_[1] + mat_[13]*matrix.mat_[5] + mat_[14]*matrix.mat_[9] + mat_[15]*matrix.mat_[13];
    c = mat_[12]*matrix.mat_[2] + mat_[13]*matrix.mat_[6] + mat_[14]*matrix.mat_[10] + mat_[15]*matrix.mat_[14];
    d = mat_[12]*matrix.mat_[3] + mat_[13]*matrix.mat_[7] + mat_[14]*matrix.mat_[11] + mat_[15]*matrix.mat_[15];

    tmp.setRow4(a, b, c, d);

    return tmp;
  }

  //! TODO surchage operateur *=
  template<class T>
  inline void Matrix4<T>::operator*= (const Matrix4 &matrix)
  {
    *this = *this * matrix;
  }
  
  //----------------------------------------------------------------------------
  //Retourne une rangé de la matrice. EX:
  // matrice[2] => retourne la rangée 3 de la matrice.
  // matrice[2][2] => retourne l'item 3,3 de la matrice ( ou mat_[10] ).
  template<class T>
  inline const std::vector<T>
  Matrix4<T>::operator[] ( unsigned int iIndex ) const
  {
    assert( iIndex < 4 );
    std::vector<T> subVector;
    
    for( unsigned int i = 0; i < 4; ++i )
    {
      subVector.push_back( mat_[ iIndex*4 + i ] );
    }
    
    return subVector;
  }

  typedef Matrix4<int>    Matrix4i;
  typedef Matrix4<float>  Matrix4f;
  typedef Matrix4<double> Matrix4d;

} //math
} // fin du namespace realisim

#endif // MATRIX_H
