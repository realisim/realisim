//!-----------------------------------------------------------------------------
//! \file
//! \brief Classe \c Matrix permet de reprÈsenter une matrice 4x4 ou 3x3
//!
//! AUTHOR:  Pierre-Olivier Beaudoin & David Pinson
//!-----------------------------------------------------------------------------
#ifndef MATRIX_4_H
#define MATRIX_4_H

#include <iostream>

//!-----------------------------------------------------------------------------
//! \brief Classe gÈrant les fonctionalitÈs associÈes a une matrice 4x4 ou 3x3
//!-----------------------------------------------------------------------------
namespace Realisim
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

    inline void setColumn1(const T &a11, const T &a21,
                           const T &a31, const T &a41);
    inline void setColumn2(const T &a12, const T &a22,
                           const T &a32, const T &a42);
    inline void setColumn3(const T &a13, const T &a23,
                           const T &a33, const T &a43);
    inline void setColumn4(const T &a14, const T &a24,
                           const T &a34, const T &a44);

    inline void setRow1_3(const T &a11, const T &a12, const T &a13);
    inline void setRow2_3(const T &a21, const T &a22, const T &a23);
    inline void setRow3_3(const T &a31, const T &a32, const T &a33);

    inline void setColumn1_3(const T &a11, const T &a21, const T &a31);
    inline void setColumn2_3(const T &a12, const T &a22, const T &a32);
    inline void setColumn3_3(const T &a13, const T &a23, const T &a33);

    // --------------- fonction get --------------------------------------------
    inline void getRow1(T &a11, T &a12, T &a13, T &a14) const;
    inline void getRow2(T &a21, T &a22, T &a23, T &a24) const;
    inline void getRow3(T &a31, T &a32, T &a33, T &a34) const;
    inline void getRow4(T &a41, T &a42, T &a43, T &a44) const;

    inline void getColumn1(T &a11, T &a21, T &a31, T &a41) const;
    inline void getColumn2(T &a12, T &a22, T &a32, T &a42) const;
    inline void getColumn3(T &a13, T &a23, T &a33, T &a43) const;
    inline void getColumn4(T &a14, T &a24, T &a34, T &a44) const;

    inline void getRow1_3(T &a11, T &a12, T &a13) const;
    inline void getRow2_3(T &a21, T &a22, T &a23) const;
    inline void getRow3_3(T &a31, T &a32, T &a33) const;

    inline void getColumn1_3(T &a11, T &a21, T &a31) const;
    inline void getColumn2_3(T &a12, T &a22, T &a32) const;
    inline void getColumn3_3(T &a13, T &a23, T &a33) const;

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
    inline Matrix4<T>& operator*= (const Matrix4 &matrix);

  protected:
  private:

    T mat_[16];
  };

  //! constructeur par dÈfaut.
  template<class T>
  inline Matrix4<T>::Matrix4()
  {
    // TODO utile? memset
    memset( (void*)mat_, 0, 16*sizeof(T) );
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

  //! permet de setter la 1er colonne
  template<class T>
  inline void Matrix4<T>::setColumn1(const T &a11, const T &a21, const T &a31,
    const T &a41)
  {
    mat_[0] = a11; mat_[4] = a21; mat_[8] = a31; mat_[12] = a41;
  }

  //! permet de setter la 2e colonne
  template<class T>
  inline void Matrix4<T>::setColumn2(const T &a12, const T &a22, const T &a32,
    const T &a42)
  {
    mat_[1] = a12; mat_[5] = a22; mat_[9] = a32; mat_[13] = a42;
  }

  //! permet de setter la 3e colonne
  template<class T>
  inline void Matrix4<T>::setColumn3(const T &a13, const T &a23, const T &a33,
    const T &a43)
  {
    mat_[2] = a13; mat_[6] = a23; mat_[10] = a33; mat_[14] = a43;
  }

  //! permet de setter la 4e colonne
  template<class T>
  inline void Matrix4<T>::setColumn4(const T &a14, const T &a24, const T &a34,
    const T &a44)
  {
    mat_[3] = a14; mat_[7] = a24; mat_[11] = a34; mat_[15] = a44;
  }

  //! permet de setter la 1er ligne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline void Matrix4<T>::setRow1_3(const T &a11, const T &a12, const T &a13)
  {
    mat_[0] = a11; mat_[1] = a12; mat_[2] = a13;
  }

  //! permet de setter la 2e ligne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline void Matrix4<T>::setRow2_3(const T &a21, const T &a22, const T &a23)
  {
    mat_[4] = a21; mat_[5] = a22; mat_[6] = a23;
  }

  //! permet de setter la 3e ligne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline void Matrix4<T>::setRow3_3(const T &a31, const T &a32, const T &a33)
  {
    mat_[8] = a31; mat_[9] = a32; mat_[10] = a33;
  }

  //! permet de setter la 1er colonne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline void Matrix4<T>::setColumn1_3(const T &a11, const T &a21, const T &a31)
  {
    mat_[0] = a11; mat_[4] = a21; mat_[8] = a31;
  }

  //! permet de setter la 2e colonne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline void Matrix4<T>::setColumn2_3(const T &a12, const T &a22, const T &a32)
  {
    mat_[1] = a12; mat_[5] = a22; mat_[9] = a32;
  }

  //! permet de setter la 3e colonne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline void Matrix4<T>::setColumn3_3(const T &a13, const T &a23, const T &a33)
  {
    mat_[2] = a13; mat_[6] = a23; mat_[10] = a33;
  }

  //! permet d'obtenir la 1er ligne
  template<class T>
  inline void Matrix4<T>::getRow1(T &a11, T &a12, T &a13, T &a14) const
  {
    a11 = mat_[0]; a12 = mat_[1]; a13 = mat_[2]; a14 = mat_[3];
  }

  //! permet d'obtenir la 2e ligne
  template<class T>
  inline void Matrix4<T>::getRow2(T &a21, T &a22, T &a23, T &a24) const
  {
    a21 = mat_[4]; a22 = mat_[5]; a23 = mat_[6]; a24 = mat_[7];
  }

  //! permet d'obtenir la 3e ligne
  template<class T>
  inline void Matrix4<T>::getRow3(T &a31, T &a32, T &a33, T &a34) const
  {
    a31 = mat_[8]; a32 = mat_[9]; a33 = mat_[10]; a34 = mat_[11];
  }

  //! permet d'obtenir la 4e ligne
  template<class T>
  inline void Matrix4<T>::getRow4(T &a41, T &a42, T &a43, T &a44) const
  {
    a41 = mat_[12]; a42 = mat_[13]; a43 = mat_[14]; a44 = mat_[15];
  }

  //! permet d'obtenir la 1er colonne
  template<class T>
  inline void Matrix4<T>::getColumn1(T &a11, T &a21, T &a31, T &a41) const
  {
    a11 = mat_[0]; a21 = mat_[4]; a31 = mat_[8]; a41 = mat_[12];
  }

  //! permet d'obtenir la 2e colonne
  template<class T>
  inline void Matrix4<T>::getColumn2(T &a12, T &a22, T &a32, T &a42) const
  {
    a12 = mat_[1]; a22 = mat_[5]; a32 = mat_[9]; a42 = mat_[13];
  }

  //! permet d'obtenir la 3e colonne
  template<class T>
  inline void Matrix4<T>::getColumn3(T &a13, T &a23, T &a33, T &a43) const
  {
    a13 = mat_[2]; a23 = mat_[6]; a33 = mat_[10]; a43 = mat_[14];
  }

  //! permet d'obtenir la 4e colonne
  template<class T>
  inline void Matrix4<T>::getColumn4(T &a14, T &a24, T &a34, T &a44) const
  {
    a14 = mat_[3]; a24 = mat_[7]; a34 = mat_[11]; a44 = mat_[15];
  }

  //! permet d'obtenir la 1er ligne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline void Matrix4<T>::getRow1_3(T &a11, T &a12, T &a13) const
  {
    a11 = mat_[0]; a12 = mat_[1]; a13 = mat_[2];
  }

  //! permet d'obtenir la 2e ligne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline void Matrix4<T>::getRow2_3(T &a21, T &a22, T &a23) const
  {
    a21 = mat_[4]; a22 = mat_[5]; a23 = mat_[6];
  }

  //! permet d'obtenir la 3e ligne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline void Matrix4<T>::getRow3_3(T &a31, T &a32, T &a33) const
  {
    a31 = mat_[8]; a32 = mat_[9]; a33 = mat_[10];
  }

  //! permet d'obtenir la 1er colonne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline void Matrix4<T>::getColumn1_3(T &a11, T &a21, T &a31) const
  {
    a11 = mat_[0]; a21 = mat_[4]; a31 = mat_[8];
  }

  //! permet d'obtenir la 2e colonne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline void Matrix4<T>::getColumn2_3(T &a12, T &a22, T &a32) const
  {
    a12 = mat_[1]; a22 = mat_[5]; a32 = mat_[9];
  }

  //! permet d'obtenir la 3e colonne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline void Matrix4<T>::getColumn3_3(T &a13, T &a23, T &a33) const
  {
    a13 = mat_[2]; a23 = mat_[6]; a33 = mat_[10];
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
  inline Matrix4<T>& Matrix4<T>::operator*= (const Matrix4 &matrix)
  {
  }

  typedef Matrix4<int>    Matrix4i;
  typedef Matrix4<float>  Matrix4f;
  typedef Matrix4<double> Matrix4d;

} // fin du namespace realisim

#endif // MATRIX_H
