//!-----------------------------------------------------------------------------
//! \file
//! \brief Classe \c Matrix permet de reprÈsenter une matrice 4x4 ou 3x3
//!
//! AUTHOR:  Pierre-Olivier Beaudoin & David Pinson
//!-----------------------------------------------------------------------------
/*Classe qui représente un matrice 4x4 de la forme

   m = a00 a01 a02 a03    m = s r r x  -> r: rotation
       a10 a11 a12 a13        r s r x     s: scale
       a20 a21 a22 a23        r r s x     t: translation
       a30 a31 a32 a33        t t t 1
       
  Par défaut la matrice est initialisée comme une matrice identité.
  il est possible d'obtenir la valeur i, j de la matrice par l'opérateur ().
  
  La matrice est de type row-major et les multiplications sont "right multiply"
  ou post-multiply. La documentation opengl utilise column-major et le left
  multiply. Ce qui peut porter a confusion...
  
  Par exemple (de wiki):
  "pre-multiply (or left multiply) A by B" means BA, while "post-multiply (or 
  right multiply) A by C" means AC.
*/

#ifndef MATRIX_4_H
#define MATRIX_4_H

#include <cassert>
#include "Point.h"
#include "Vect.h"
#include <vector>
#include <iostream>
#include <iomanip>

//!-----------------------------------------------------------------------------
//! \brief Classe gÈrant les fonctionalités associées a une matrice 4x4 ou 3x3
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
    inline Matrix4( const T* );
    inline Matrix4(const T &a11, const T &a12, const T &a13, const T &a14,
                  const T &a21, const T &a22, const T &a23, const T &a24,
                  const T &a31, const T &a32, const T &a33, const T &a34,
                  const T &a41, const T &a42, const T &a43, const T &a44);
    inline Matrix4(const Matrix4 &matrix);
    template< class U > Matrix4( const Matrix4<U>& );

    // --------------- destructeurs --------------------------------------------
    ~Matrix4();

    // --------------- fonction set --------------------------------------------
    inline void setRow1(const T &a11, const T &a12, const T &a13, const T &a14);
    inline void setRow2(const T &a21, const T &a22, const T &a23, const T &a24);
    inline void setRow3(const T &a31, const T &a32, const T &a33, const T &a34);
    inline void setRow4(const T &a41, const T &a42, const T &a43, const T &a44);
    
    inline void setRotation(const Matrix4<T>& iRot);
    inline void setScaling( const Vector3<T>& iScale );
    inline void setTranslation( const Point3<T>& iTrans );

    // --------------- fonction get --------------------------------------------
    inline Vector3<T> getBaseX() const;
    inline Vector3<T> getBaseY() const;
    inline Vector3<T> getBaseZ() const;
    inline Point3<T> getTranslation() const;
    inline Matrix4<T> getRotation() const;

    // --------------- fonction utiles -----------------------------------------
    inline const T* getPtr() const;
    inline T* getNonConstPtr();
    inline Matrix4<T>& inverse();
    inline void loadIdentity();
    inline void multEquMat3(const Matrix4 &matrix);
    inline void print() const;
    inline void translate(const Vector3<T>& iTrans);
    inline void transpose();

    // --------------- Overload: operateurs unitaires --------------------------
    inline T operator()(unsigned int i, unsigned int j) const;
    inline T& operator[] (int);
    inline Matrix4<T>& operator=  (const Matrix4 &matrix);

    inline Matrix4<T>  operator+  (const Matrix4 &matrix) const;
    inline Matrix4<T>& operator+= (const Matrix4 &matrix);

    inline Matrix4<T>  operator-  (const Matrix4 &matrix) const;
    inline Matrix4<T>& operator-= (const Matrix4 &matrix);

    inline Matrix4<T>  operator*  (const Matrix4 &matrix) const;
    inline void operator*= (const Matrix4 &matrix);    

  protected:
  private:

    T mat_[16];
  };

  //! constructeur par défaut.
  template<class T>
  inline Matrix4<T>::Matrix4()
  { loadIdentity();}

	template<class T>
  inline Matrix4<T>::Matrix4( const T* iM )
  { memcpy(&mat_[0], iM, 16 * sizeof(T)); }
  
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
  { memcpy( (void*)mat_, (void*)matrix.mat_, 16*sizeof(T) ); }
  
  //!---------------------------------------------------------------------------
  /*permet la conversion implicite entre les type int, double, float etc...*/
  template<class T>
  template<class U>
  Matrix4<T>::Matrix4( const Matrix4<U>& iV )
  { mat_[0]  = (U)iV(0,0); mat_[1]  = (U)iV(0,1); mat_[2]  = (U)iV(0,2); mat_[3]  = (U)iV(0,3);
    mat_[4]  = (U)iV(1,0); mat_[5]  = (U)iV(1,1); mat_[6]  = (U)iV(1,2); mat_[7]  = (U)iV(1,3);
    mat_[8]  = (U)iV(2,0); mat_[9]  = (U)iV(2,1); mat_[10] = (U)iV(2,2); mat_[11] = (U)iV(2,3);
    mat_[12] = (U)iV(3,0); mat_[13] = (U)iV(3,1); mat_[14] = (U)iV(3,2); mat_[15] = (U)iV(3,3); 
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
  
  
  //permet de setter la sous matrice 3x3 qui représente la rotation
  template<class T>
  inline void Matrix4<T>::setRotation(const Matrix4<T>& iRot)
  { 
    mat_[0] = iRot(0, 0); mat_[1] = iRot(0, 1); mat_[2] = iRot(0, 2);
    mat_[4] = iRot(1, 0); mat_[5] = iRot(1, 1); mat_[6] = iRot(1, 2);
    mat_[8] = iRot(2, 0); mat_[9] = iRot(2, 1); mat_[10] = iRot(2, 2);
  }
  
  //ecrase la diagonale qui représente le scaling
  template<class T>
  inline void Matrix4<T>::setScaling(const Vector3<T>& iV)
  {
    mat_[0] = iV.getX(); mat_[5] = iV.getY(); mat_[10] = iV.getZ();
  }
  
  //! permet de setter la 4e ligne de la matrice
  template<class T>
  inline void Matrix4<T>::setTranslation(const Point3<T>& iTrans)
  {
    mat_[12] = iTrans.getX(); mat_[13] = iTrans.getY(); mat_[14] = iTrans.getZ();
  }
  
  // permet d'ajouter la translation iTrans a la matrice
  template<class T>
  inline void Matrix4<T>::translate(const Vector3<T>& iTrans)
  {
  	Point3d t( getTranslation().getX() + iTrans.getX(),
    	getTranslation().getY() + iTrans.getY(),
      getTranslation().getZ() + iTrans.getZ() );
    setTranslation(t);
  }

  //transpose la matrice
  template<class T>
  inline void Matrix4<T>::transpose()
  {
  	Matrix4<T> m(*this);
  	mat_[0] = m[0]; mat_[1] = m[4]; mat_[2] = m[8]; mat_[3] = m[12];
    mat_[4] = m[1]; mat_[5] = m[5]; mat_[6] = m[9]; mat_[7] = m[13];
    mat_[8] = m[2]; mat_[9] = m[6]; mat_[10] = m[10]; mat_[11] = m[14];
    mat_[12] = m[3]; mat_[13] = m[7]; mat_[14] = m[11]; mat_[15] = m[15];
  }
  
  //! permet d'obtenir la 1er ligne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline Vector3<T> Matrix4<T>::getBaseX() const
  {
    return Vector3<T>( mat_[0], mat_[1], mat_[2]);
  }

  //! permet d'obtenir la 2e ligne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline Vector3<T> Matrix4<T>::getBaseY() const
  {
    return Vector3<T>( mat_[4], mat_[5], mat_[6]);
  }

  //! permet d'obtenir la 3e ligne de la sous matrice 3x3 ("coin" sup gauche)
  template<class T>
  inline Vector3<T> Matrix4<T>::getBaseZ() const
  {
   return Vector3<T>( mat_[8], mat_[9], mat_[10]);
  }
  
  //! permet d'obtenir la translation
  template<class T>
  inline Point3<T> Matrix4<T>::getTranslation() const
  {
    return Point3<T>( mat_[12], mat_[13], mat_[14]);
  }
  
  //----------------------------------------------------------------------------
  //! permet d'obtenir la rotation sans la translation
  template<class T>
  inline Matrix4<T> Matrix4<T>::getRotation() const
  {
    return Matrix4<T>(mat_[0], mat_[1], mat_[2], 0,
    									mat_[4], mat_[5], mat_[6], 0,
                      mat_[8], mat_[9], mat_[10], 0,
                      0, 0, 0, 1);
  }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline Matrix4<T>& Matrix4<T>::inverse()
  {
    /*trouver le determinant de la matrice, s'il est different de 0, on peut
    inverser la matrice, sinon on ne peut pas*/
    double determinant =
      (*this)(0, 0) * (*this)(1, 1) * (*this)(2, 2) * (*this)(3, 3) +
      (*this)(0, 0) * (*this)(1, 2) * (*this)(2, 3) * (*this)(3, 1) +
      (*this)(0, 0) * (*this)(1, 3) * (*this)(2, 1) * (*this)(3, 2) +
      
      (*this)(0, 1) * (*this)(1, 0) * (*this)(2, 3) * (*this)(3, 2) +
      (*this)(0, 1) * (*this)(1, 2) * (*this)(2, 0) * (*this)(3, 3) +
      (*this)(0, 1) * (*this)(1, 3) * (*this)(2, 2) * (*this)(3, 0) +
      
      (*this)(0, 2) * (*this)(1, 0) * (*this)(2, 1) * (*this)(3, 3) +
      (*this)(0, 2) * (*this)(1, 1) * (*this)(2, 3) * (*this)(3, 0) +
      (*this)(0, 2) * (*this)(1, 3) * (*this)(2, 0) * (*this)(3, 1) +
      
      (*this)(0, 3) * (*this)(1, 0) * (*this)(2, 2) * (*this)(3, 1) +
      (*this)(0, 3) * (*this)(1, 1) * (*this)(2, 0) * (*this)(3, 2) +
      (*this)(0, 3) * (*this)(1, 2) * (*this)(2, 1) * (*this)(3, 0) -
      
      (*this)(0, 0) * (*this)(1, 1) * (*this)(2, 3) * (*this)(3, 2) -
      (*this)(0, 0) * (*this)(1, 2) * (*this)(2, 1) * (*this)(3, 3) -
      (*this)(0, 0) * (*this)(1, 3) * (*this)(2, 2) * (*this)(3, 1) -
      
      (*this)(0, 1) * (*this)(1, 0) * (*this)(2, 2) * (*this)(3, 3) -
      (*this)(0, 1) * (*this)(1, 2) * (*this)(2, 3) * (*this)(3, 0) -
      (*this)(0, 1) * (*this)(1, 3) * (*this)(2, 0) * (*this)(3, 2) -
      
      (*this)(0, 2) * (*this)(1, 0) * (*this)(2, 3) * (*this)(3, 1) -
      (*this)(0, 2) * (*this)(1, 1) * (*this)(2, 0) * (*this)(3, 3) -
      (*this)(0, 2) * (*this)(1, 3) * (*this)(2, 1) * (*this)(3, 0) -
      
      (*this)(0, 3) * (*this)(1, 0) * (*this)(2, 1) * (*this)(3, 2) -
      (*this)(0, 3) * (*this)(1, 1) * (*this)(2, 2) * (*this)(3, 0) -
      (*this)(0, 3) * (*this)(1, 2) * (*this)(2, 0) * (*this)(3, 1);
    
    //si le determinant est plus grand que 0, on peut inverser la matrice
    if (-EPSILON < determinant &&
      determinant > EPSILON)
    {
      double invDet = 1.0 / determinant;
      Matrix4<T> tmp(*this);
      mat_[0] = (tmp(1, 1) * tmp(2, 2) * tmp(3, 3) +
                tmp(1, 2) * tmp(2, 3) * tmp(3, 1) +
                tmp(1, 3) * tmp(2, 1) * tmp(3, 2) -
                tmp(1, 1) * tmp(2, 3) * tmp(3, 2) -
                tmp(1, 2) * tmp(2, 1) * tmp(3, 3) -
                tmp(1, 3) * tmp(2, 2) * tmp(3, 1)) * invDet;
      
      mat_[1] = (tmp(0, 1) * tmp(2, 3) * tmp(3, 2) +
                tmp(0, 2) * tmp(2, 1) * tmp(3, 3) +
                tmp(0, 3) * tmp(2, 2) * tmp(3, 1) -
                tmp(0, 1) * tmp(2, 2) * tmp(3, 3) -
                tmp(0, 2) * tmp(2, 3) * tmp(3, 1) -
                tmp(0, 3) * tmp(2, 1) * tmp(3, 2)) * invDet;
        
      mat_[2] = (tmp(0, 1) * tmp(1, 2) * tmp(3, 3) +
                tmp(0, 2) * tmp(1, 3) * tmp(3, 1) +
                tmp(0, 3) * tmp(1, 1) * tmp(3, 2) -
                tmp(0, 1) * tmp(1, 3) * tmp(3, 2) -
                tmp(0, 2) * tmp(1, 1) * tmp(3, 3) -
                tmp(0, 3) * tmp(1, 2) * tmp(3, 1)) * invDet;
                
      mat_[3] = (tmp(0, 1) * tmp(1, 3) * tmp(2, 2) +
                tmp(0, 2) * tmp(1, 1) * tmp(2, 3) +
                tmp(0, 3) * tmp(1, 2) * tmp(2, 1) -
                tmp(0, 1) * tmp(1, 2) * tmp(2, 3) -
                tmp(0, 2) * tmp(1, 3) * tmp(2, 1) -
                tmp(0, 3) * tmp(1, 1) * tmp(2, 2)) * invDet;
                
      mat_[4] = (tmp(1, 0) * tmp(2, 3) * tmp(3, 2) +
                tmp(1, 2) * tmp(2, 0) * tmp(3, 3) +
                tmp(1, 3) * tmp(2, 2) * tmp(3, 0) -
                tmp(1, 0) * tmp(2, 2) * tmp(3, 3) -
                tmp(1, 2) * tmp(2, 3) * tmp(3, 0) -
                tmp(1, 3) * tmp(2, 0) * tmp(3, 2)) * invDet;
                
      mat_[5] = (tmp(0, 0) * tmp(2, 2) * tmp(3, 3) +
                tmp(0, 2) * tmp(2, 3) * tmp(3, 0) +
                tmp(0, 3) * tmp(2, 0) * tmp(3, 2) -
                tmp(0, 0) * tmp(2, 3) * tmp(3, 2) -
                tmp(0, 2) * tmp(2, 0) * tmp(3, 3) -
                tmp(0, 3) * tmp(2, 2) * tmp(3, 0)) * invDet;
                
      mat_[6] = (tmp(0, 0) * tmp(1, 3) * tmp(3, 2) +
                tmp(0, 2) * tmp(1, 0) * tmp(3, 3) +
                tmp(0, 3) * tmp(1, 2) * tmp(3, 0) -
                tmp(0, 0) * tmp(1, 2) * tmp(3, 3) -
                tmp(0, 2) * tmp(1, 3) * tmp(3, 0) -
                tmp(0, 3) * tmp(1, 0) * tmp(3, 2)) * invDet;
                
      mat_[7] = (tmp(0, 0) * tmp(1, 2) * tmp(2, 3) +
                tmp(0, 2) * tmp(1, 3) * tmp(2, 0) +
                tmp(0, 3) * tmp(1, 0) * tmp(2, 2) -
                tmp(0, 0) * tmp(1, 3) * tmp(2, 2) -
                tmp(0, 2) * tmp(1, 0) * tmp(2, 3) -
                tmp(0, 3) * tmp(1, 2) * tmp(2, 0)) * invDet;
                
      mat_[8] = (tmp(1, 0) * tmp(2, 1) * tmp(3, 3) +
                tmp(1, 1) * tmp(2, 3) * tmp(3, 1) +
                tmp(1, 3) * tmp(2, 0) * tmp(3, 1) -
                tmp(1, 0) * tmp(2, 3) * tmp(3, 1) -
                tmp(1, 1) * tmp(2, 0) * tmp(3, 3) -
                tmp(1, 3) * tmp(2, 1) * tmp(3, 0)) * invDet;
                
      mat_[9] = (tmp(0, 0) * tmp(2, 3) * tmp(3, 1) +
                tmp(0, 1) * tmp(2, 0) * tmp(3, 3) +
                tmp(0, 3) * tmp(2, 1) * tmp(3, 0) -
                tmp(0, 0) * tmp(2, 1) * tmp(3, 3) -
                tmp(0, 1) * tmp(2, 3) * tmp(3, 0) -
                tmp(0, 3) * tmp(2, 0) * tmp(3, 1)) * invDet;
                
      mat_[10] = (tmp(0, 0) * tmp(1, 1) * tmp(3, 3) +
                 tmp(0, 1) * tmp(1, 3) * tmp(3, 0) +
                 tmp(0, 3) * tmp(1, 0) * tmp(3, 1) -
                 tmp(0, 0) * tmp(1, 3) * tmp(3, 1) -
                 tmp(0, 1) * tmp(1, 0) * tmp(3, 3) -
                 tmp(0, 3) * tmp(1, 1) * tmp(3, 0)) * invDet;
                 
      mat_[11] = (tmp(0, 0) * tmp(1, 3) * tmp(2, 1) +
                 tmp(0, 1) * tmp(1, 0) * tmp(2, 3) +
                 tmp(0, 3) * tmp(1, 1) * tmp(2, 0) -
                 tmp(0, 0) * tmp(1, 1) * tmp(2, 3) -
                 tmp(0, 1) * tmp(1, 3) * tmp(2, 0) -
                 tmp(0, 3) * tmp(1, 0) * tmp(2, 1)) * invDet;
                 
      mat_[12] = (tmp(1, 0) * tmp(2, 2) * tmp(3, 1) +
                 tmp(1, 1) * tmp(2, 0) * tmp(3, 2) +
                 tmp(1, 2) * tmp(2, 1) * tmp(3, 0) -
                 tmp(1, 0) * tmp(2, 1) * tmp(3, 2) -
                 tmp(1, 1) * tmp(2, 2) * tmp(3, 0) -
                 tmp(1, 2) * tmp(2, 0) * tmp(3, 1)) * invDet;
                 
      mat_[13] = (tmp(0, 0) * tmp(2, 1) * tmp(3, 2) +
                 tmp(0, 1) * tmp(2, 2) * tmp(3, 0) +
                 tmp(0, 2) * tmp(2, 0) * tmp(3, 1) -
                 tmp(0, 0) * tmp(2, 2) * tmp(3, 1) -
                 tmp(0, 1) * tmp(2, 0) * tmp(3, 2) -
                 tmp(0, 2) * tmp(2, 1) * tmp(3, 0)) * invDet;
                 
      mat_[14] = (tmp(0, 0) * tmp(1, 2) * tmp(3, 1) +
                 tmp(0, 1) * tmp(1, 0) * tmp(3, 2) +
                 tmp(0, 2) * tmp(1, 1) * tmp(3, 0) -
                 tmp(0, 0) * tmp(1, 1) * tmp(3, 2) -
                 tmp(0, 1) * tmp(1, 2) * tmp(3, 0) -
                 tmp(0, 2) * tmp(1, 0) * tmp(3, 1)) * invDet;
                 
      mat_[15] = (tmp(0, 0) * tmp(1, 1) * tmp(2, 2) +
                 tmp(0, 1) * tmp(1, 2) * tmp(2, 0) +
                 tmp(0, 2) * tmp(1, 0) * tmp(2, 1) -
                 tmp(0, 0) * tmp(1, 2) * tmp(2, 1) -
                 tmp(0, 1) * tmp(1, 0) * tmp(2, 2) -
                 tmp(0, 2) * tmp(1, 1) * tmp(2, 0)) * invDet;
    }
    
    return *this;
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

  //! fonction qui affiche le contenu de la matrice utile pour le débugage
  template<class T>
  inline void Matrix4<T>::print() const
  {
  	printf( "%6.6f, %6.6f, %6.6f, %6.6f\n", mat_[0], mat_[1], mat_[2], mat_[3] );
    printf( "%6.6f, %6.6f, %6.6f, %6.6f\n", mat_[4], mat_[5], mat_[6], mat_[7] );
    printf( "%6.6f, %6.6f, %6.6f, %6.6f\n", mat_[8], mat_[9], mat_[10], mat_[11] );
    printf( "%6.6f, %6.6f, %6.6f, %6.6f\n\n", mat_[12], mat_[13], mat_[14], mat_[15] );
  }

  //! fct qui retourne un pointeur sur la matrice (pratique pour OGL par ex.)
  template<class T>
  inline const T* Matrix4<T>::getPtr() const
  { return &mat_[0]; }
  
  template<class T>
  inline T* Matrix4<T>::getNonConstPtr()
  { return &mat_[0]; }

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

  //----------------------------------------------------------------------------
  template<class T>
  inline void Matrix4<T>::operator*= (const Matrix4 &matrix)
  {
    *this = *this * matrix;
  }
  
  //----------------------------------------------------------------------------
  /*Retourne l'élément i, j de la matrice.
      i: rangé
      j: colonne
      m = a00 a01 a02 a03
          a10 a11 a12 a13
          a20 a21 a22 a23
          a30 a31 a32 a33*/
  template<class T>
  inline T Matrix4<T>::operator()(unsigned int i, unsigned int j) const
  {
    assert(i < 4 && j< 4);
    if(i < 4 && j< 4)
      return mat_[i * 4 + j];
    return T(0);
  }
  //----------------------------------------------------------------------------
  //retourne une reference a l'élément i
  template<class T>
  inline T& Matrix4<T>::operator[] (int i)
  { return mat_[i]; }

  typedef Matrix4<int>    Matrix4i;
  typedef Matrix4<float>  Matrix4f;
  typedef Matrix4<double> Matrix4d;

} //math
} // fin du namespace realisim

#endif // MATRIX_H
