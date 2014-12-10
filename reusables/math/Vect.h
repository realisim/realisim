//!-----------------------------------------------------------------------------
//! \file
//! \brief Classe \c Vect permet de representer un vecteur a trois dimensions
//!
//! AUTHOR:  Pierre-Olivier Beaudoin & David Pinson
//!-----------------------------------------------------------------------------

#ifndef VECT_H
#define VECT_H

#include "Point.h"
#include "MathDef.h"

//!-----------------------------------------------------------------------------
//! \brief Classe gerant les fonctionalites associees a un vecteur
//!-----------------------------------------------------------------------------
namespace realisim
{
namespace math
{
//-----------------------------------------------------------------------------
//--- Vector3
//-----------------------------------------------------------------------------
  template<class T>
  class Vector3 
  {
  public:
  
    // --------------- constructeurs -------------------------------------------
    Vector3();
    explicit Vector3(const T &val);
    Vector3(const T &x, const T &y, const T &z);
    Vector3(const Vector3<T> &vect);    
    Vector3(const Point3<T> &pt1, const Point3<T> &pt2);
    virtual ~Vector3();
    
    // --------------- fonction set --------------------------------------------
    inline void set(const Vector3<T> &vect);
    inline void set(const T &val);
    inline void setX(const T &x);
    inline void setY(const T &y);
    inline void setZ(const T &z);
    inline void setXYZ(const T &x, const T &y, const T &z);    
    inline void set(const Point3<T> &pt1, const Point3<T> &pt2);
    
    // --------------- fonction get --------------------------------------------
    inline const T& x() const;
    inline const T& y() const;
    inline const T& z() const;
    inline void get(Vector3<T> &vect) const;
    inline void xYZ(T &x, T &y, T &z) const;
    inline const T* getPtr() const;
//inline std::String toString() const;
    
    // --------------- fonction utiles -----------------------------------------
    inline void minCoord (const Vector3<T>& iVect);
    inline void maxCoord (const Vector3<T>& iVect);
    inline T norm() const;
    inline Vector3<T>& normalise();
    inline T fastNorm() const;
    
    // --------------- Overload: operateurs unitaires --------------------------
    inline Vector3<T>&  operator=  (const Vector3<T> &vect);
    inline Vector3<T>&  operator=  (const T &val);
    
    inline Vector3<T>  operator+  (const Vector3<T> &vect) const;
    inline Vector3<T>  operator+  (const T &val) const;
    inline Vector3<T>& operator+= (const Vector3<T> &vect);
    inline Vector3<T>& operator+= (const T &val);
    
    inline Vector3<T>  operator-  (const Vector3<T> &vect) const;
    inline Vector3<T>  operator-  (const T &val) const;
    inline Vector3<T>  operator-  ();
    inline Vector3<T>& operator-= (const Vector3<T> &vect);
    inline Vector3<T>& operator-= (const T &val);
    
    inline Vector3<T>  operator*  (const T &val) const;    
    inline Vector3<T>& operator*= (const T &val);
    
    inline Vector3<T>  operator/  (const T &val) const;
    inline Vector3<T>& operator/= (const T &val);  
    inline bool operator== ( const Vector3<T>& ) const;
    inline bool operator!= ( const Vector3<T>& ) const;
  
    // -------------- Overload: produit vectoriel ------------------------------
    inline Vector3<T>  operator^  (const Vector3<T> &vect) const;
    
    // -------------- Overload: produit scalaire -------------------------------
    inline T          operator*  (const Vector3<T> &vect) const;
    
  protected:
  private:
  
    T mData[3];
  };
  
  //! constructeur par defaut.
  template<class T>
  inline Vector3<T>::Vector3()
  { mData[0] = mData[1] = mData[2] = (T)0; }
  
  //!---------------------------------------------------------------------------
  //! \brief  Constructeur avec parametre.
  //!
  //! Construit un objet de type \c Vect ou chacune des valeurs cartesiennes 
  //! vaut la meme valeur.
  //! 
  //! \param val valeur d'initialisation
  //!---------------------------------------------------------------------------
  template<class T>
  inline Vector3<T>::Vector3(const T &val)
  { set(val); }
  
  //!---------------------------------------------------------------------------
  //! \brief  Constructeur avec parametres.
  //!
  //! Construit un objet de type \c Vect avec chacune des valeurs cartesiennes 
  //! passees en parametre.
  //! 
  //! \param x valeur d'initialisation pour x_
  //! \param y valeur d'initialisation pour y_
  //! \param z valeur d'initialisation pour z_
  //!---------------------------------------------------------------------------
  template<class T>
  inline Vector3<T>::Vector3(const T &x, const T &y, const T &z)
  { setXYZ(x, y, z); }
  
  //! constructeur copie
  template<class T>
  inline Vector3<T>::Vector3(const Vector3<T> &vect)
  { memcpy((void*)mData, (const void*)vect.mData, 3 * sizeof(T)); }
  
  //!---------------------------------------------------------------------------
  //! \brief  Constructeur avec parametres.
  //!
  //! Construit un objet de type \c Vect a partir de 2 points.
  //! 1--------->2  vecteur = 2 - 1
  //! 
  //! \param &pt1 le premier point
  //! \param &pt2 le deuxieme point
  //!---------------------------------------------------------------------------
  template<class T>
  inline Vector3<T>::Vector3(const Point3<T> &pt1, const Point3<T> &pt2)
  { set(pt1, pt2); }
  
  //! destructeur
  template<class T>
  Vector3<T>::~Vector3()
  {}
  
  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur du vecteur.
  //!
  //! egale le vecteur a celui passe en parametre.
  //! 
  //! \param &vect
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vector3<T>::set(const Vector3<T> &vect)
  { memcpy((void*)mData, (const void*)vect.mData, 3 * sizeof(T)); }
  
  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur du vecteur.
  //!
  //! egale le vecteur a la valeur passee en parametre.
  //! 
  //! \param &val
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vector3<T>::set(const T &val)
  { mData[0] = mData[1] = mData[2] = (T)val; }
  
  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur x_ du vecteur.
  //! 
  //! \param &x
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vector3<T>::setX(const T &x)
  { mData[0] = x; }
  
  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur y_ du vecteur.
  //! 
  //! \param &y
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vector3<T>::setY(const T &y)
  { mData[1] = y; }
  
  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur z_ du vecteur.
  //! 
  //! \param &z
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vector3<T>::setZ(const T &z)
  { mData[2] = z; }
  
  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur du vecteur.
  //!
  //! egale le vecteur a chacune des valeurs passeees en parametre.
  //! 
  //! \param &x
  //! \param &y
  //! \param &z
  //!---------------------------------------------------------------------------
    template<class T>
  inline void Vector3<T>::setXYZ(const T &x, const T &y, const T &z)
  { mData[0] =x; mData[1] = y; mData[2] = z; }
  
  //!---------------------------------------------------------------------------
  //! \brief  Constructeur avec parametres.
  //!
  //! Construit un objet de type \c Vect a partir de 2 points.
  //! 1--------->2  vecteur = 2 - 1
  //! 
  //! \param &pt1 le premier point
  //! \param &pt2 le deuxieme point
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vector3<T>::set(const Point3<T> &pt1, const Point3<T> &pt2)
  {
    mData[0] = (pt2.x() - pt1.x());
    mData[1] = (pt2.y() - pt1.y());
    mData[2] = (pt2.z() - pt1.z());
  }
  
  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir le vecteur.
  //!
  //! Permet d'obtenir la valeur du vecteur dans celui passÈ en parametre.
  //! 
  //! \param &vecteur Le vecteur qui va etre comme le celui que l'on veut
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vector3<T>::get(Vector3<T> &vect) const
  { memcpy((void*)vect.mData, (const void*)mData, 3 * sizeof(T)); }
  
  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir la valeur x du vecteur.
  //! 
  //! \return la valeur X du vecteur
  //!---------------------------------------------------------------------------
  template<class T>
  inline const T& Vector3<T>::x() const
  { return mData[0]; }
  
  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir la valeur y du vecteur.
  //! 
  //! \return la valeur Y du vecteur
  //!---------------------------------------------------------------------------
  template<class T>
  inline const T& Vector3<T>::y() const
  { return mData[1]; }
  
  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir la valeur z du vecteur.
  //! 
  //! \return la valeur Z du vecteur
  //!---------------------------------------------------------------------------
  template<class T>
  inline const T& Vector3<T>::z() const
  { return mData[2]; }
  
  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir le vecteur.
  //! 
  //! Le vecteur est transfÈrÈ dans les variables passÈes en parametres
  //!
  //! \param &x
  //! \param &y
  //! \param &z
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vector3<T>::xYZ(T &x, T &y, T &z) const
  {
    x = mData[0];
    y = mData[1];
    z = mData[2];
  }
  
  template<class T>
  inline const T* Vector3<T>::getPtr() const
  { return &mData[0]; }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline void Vector3<T>::minCoord (const Vector3<T>& iVect)
  {
    setX (std::min (x (), iVect.x ()));
    setY (std::min (y (), iVect.y ()));
    setZ (std::min (z (), iVect.z ()));
  }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline void Vector3<T>::maxCoord (const Vector3<T>& iVect)
  {
    setX (std::max (x (), iVect.x ()));
    setY (std::max (y (), iVect.y ()));
    setZ (std::max (z (), iVect.z ()));
  }
    
  //!---------------------------------------------------------------------------
  //! \brief  Calcule et retourne la norme du vecteur
  //!
  //! \return la norme du vecteur
  //!---------------------------------------------------------------------------
  template<class T>
  inline T Vector3<T>::norm() const
  {
    return std::sqrt(mData[0]*mData[0] + mData[1]*mData[1] + mData[2]*mData[2]);
  }
  
  //!---------------------------------------------------------------------------
  //! \brief  Calcule et retourne un vecteur normalisÈ
  //!
  //! \return un vecteur normalisÈ
  //!---------------------------------------------------------------------------
  template<class T>
  inline Vector3<T>& Vector3<T>::normalise()
  {
    (*this) /= this->norm();
    return (*this);
  }
  
  //!---------------------------------------------------------------------------
  //! \brief  Calcule et retourne la norme du vecteur
  //!
  //! retourne une approximation de la norme du vecteur, mais le calcul 
  //! s'effectue plus vite.
  //!
  //! \return une approximation de la norme
  //!---------------------------------------------------------------------------
  template<class T>
  inline T Vector3<T>::fastNorm() const
  {
    T max, med, min, mom;
    
    max = x();
    med = y();
    min = z();
    
    max = max > 0.0  ?  max  :  -max;
    med = med > 0.0  ?  med  :  -med;
    min = min > 0.0  ?  min  :  -min;
    
    if(max<med)
      mom=max, max=med, med=mom;
  
    if(max<min)
      mom=max, max=min, min=mom;
  
    if(med<min)
      mom=med, med=min, min=mom;
  
    return( max + (11.0/32.0)*med + (1.0/4.0)*min );
  }
  //----------------------------------------------------------------------------
  //strictement egale, pour une comparaison un peu plus permissive, voir
  //mathUtil isEqual( const Vect<T> &, const Vect<T> &, double )
  template<class T>
  inline bool Vector3<T>::operator== (const Vector3<T>& iV) const
  {
  	return x() == iV.x() &&
    	y() == iV.y() &&
      z() == iV.z();
  }
  //----------------------------------------------------------------------------
  template<class T>
  inline bool Vector3<T>::operator!= ( const Vector3<T>& iV ) const
  { return !( *this == iV ); }
  //----------------------------------------------------------------------------
  //! surcharge opÈrateur = Vect
  template<class T>
  inline Vector3<T>& Vector3<T>::operator= (const Vector3<T> &vect)
  {
    mData[0] = vect.x();
    mData[1] = vect.y();
    mData[2] = vect.z();
    return *this;
  }
  
  //! surcharge opÈrateur = T
  template<class T>
  inline Vector3<T>& Vector3<T>::operator= (const T &val)
  {
    set(val);
    return *this;
  }
    
  //! surcharge opÈrateur + avec un vecteur
  template<class T>
  inline Vector3<T> Vector3<T>::operator+ (const Vector3<T> &vect) const
  {
    Vector3<T> r;
    
    r.mData[0] = x() + vect.x();
    r.mData[1] = y() + vect.y();
    r.mData[2] = z() + vect.z();
    
    return r;
  }
  
  //! surcharge opÈrateur + avec T
  template<class T>
  inline Vector3<T> Vector3<T>::operator+ (const T &val) const
  {
    Vector3<T> r;
    
    r.mData[0] = x() + val;
    r.mData[1] = y() + val;
    r.mData[2] = z() + val;
    
    return r;
  }
  
  //! surcharge opÈrateur += avec un vecteur
  template<class T>
  inline Vector3<T>& Vector3<T>::operator+= (const Vector3<T> &vect)
  {
    mData[0] += vect.x();
    mData[1] += vect.y();
    mData[2] += vect.z();
    return *this;
  }
    
  //! surcharge opÈrateur += avec T
  template<class T>
  inline Vector3<T>& Vector3<T>::operator+= (const T &val)
  {
    mData[0] += val;
    mData[1] += val;
    mData[2] += val;
    return *this;
  }
  
  //! surcharge opÈrateur - avec un vecteur
  template<class T>  
  inline Vector3<T> Vector3<T>::operator- (const Vector3<T> &vect) const
  {
    Vector3<T> r;
    
    r.mData[0] = x() - vect.x();
    r.mData[1] = y() - vect.y();
    r.mData[2] = z() - vect.z();
    
    return r;
  }
  
  //! surcharge opÈrateur - avec T
  template<class T>
  inline Vector3<T> Vector3<T>::operator- (const T &val) const
  {
    Vector3<T> r;
    
    r.mData[0] = x() - val;
    r.mData[1] = y() - val;
    r.mData[2] = z() - val;
    
    return r;
  }
  
  //! surcharge opÈrateur -
  template<class T>
  inline Vector3<T> Vector3<T>::operator- ()
  { 
    Vector3<T> r;
       
    r.mData[0] = -mData[0];
    r.mData[1] = -mData[1];
    r.mData[2] = -mData[2];
    
    return r;
  }
  
  //! surcharge opÈrateur -= avec un vecteur
  template<class T>
  inline Vector3<T>& Vector3<T>::operator-= (const Vector3<T> &vect)
  {
    mData[0] -= vect.x();
    mData[1] -= vect.y();
    mData[2] -= vect.z();
    return *this;
  }
  
  //! surcharge opÈrateur -= avec T
  template<class T>
  inline Vector3<T>& Vector3<T>::operator-= (const T &val)
  {
    mData[0] -= val;
    mData[1] -= val;
    mData[2] -= val;
    return *this;
  }
    
  //----------------------------------------------------------------------------
  //! surcharge opÈrateur * avec T
  template<class T>
  inline Vector3<T> Vector3<T>::operator* (const T &val) const
  {
    Vector3<T> r;
    
    r.mData[0] = x() * val;
    r.mData[1] = y() * val;
    r.mData[2] = z() * val;
    
    return r;
  }
  
  //----------------------------------------------------------------------------
  //! surcharge opÈrateur *= avec T
  template<class T>
  inline Vector3<T>& Vector3<T>::operator*= (const T &val)
  {
    mData[0] *= val;
    mData[1] *= val;
    mData[2] *= val;
    return *this;
  }
  
  //----------------------------------------------------------------------------
  //! surcharge opÈrateur / avec T
  template<class T>  
  inline Vector3<T> Vector3<T>::operator/ (const T &val) const
  {
    double vTmp;
    Vector3<T> vect(*this);
    
    if(val>EPSILON || val<-EPSILON)
      vTmp=((double)1.0)/(double)val;
    else
      vTmp=(double)0.0;
      
    vect *= vTmp;    
    return vect;
  }
  
  //----------------------------------------------------------------------------
  //! surcharge operateur /= avec T
  template<class T>
  inline Vector3<T>& Vector3<T>::operator/= (const T &val)
  {
    double vTmp;
    
    if(val>EPSILON || val<-EPSILON)
      vTmp=((double)1.0)/(double)val;
    else
      vTmp=(double)0.0;
      
		*this *= vTmp;
    return *this;
  }
  //----------------------------------------------------------------------------
  //produit vectoriel
  template<class T>
  inline Vector3<T> Vector3<T>::operator^ (const Vector3<T> &vect) const
  {
    Vector3<T> r;
    
    r.mData[0] = y() * vect.z() - z() * vect.y();
    r.mData[1] = z() * vect.x() - x() * vect.z();
    r.mData[2] = x() * vect.y() - y() * vect.x();
    
    return r;
  }

  //----------------------------------------------------------------------------
  //produit scalaire
  template<class T>
  inline T Vector3<T>::operator* (const Vector3<T> &vect) const
  {
    T res = x() * vect.x() + y() * vect.y() + z() * vect.z();
    return res;
  }

//-----------------------------------------------------------------------------
//--- Vector2
//-----------------------------------------------------------------------------
  template<class T>
  class Vector2
  {
  public:
    // --------------- constructeurs -------------------------------------------
    Vector2();
    explicit Vector2(const T&);
    template<class U> Vector2( const Vector2<U>& );
    Vector2(const T &, const T &);
    Vector2(const Vector2<T> &);    
    Vector2(const Point2<T> &, const Point2<T> &);
    virtual ~Vector2();
    
    virtual void set(const Vector2<T> &);
    virtual void set(const T &);
    virtual void set(const T &, const T &);
    virtual void set(const Point2<T> &, const Point2<T> &);
    virtual void setX(const T &);
    virtual void setY(const T &);
    
    virtual const T& x() const;
    virtual const T& y() const;
    virtual const T* getPtr() const;
    
    // --------------- fonction utiles -----------------------------------------
    inline Vector2<T> getUnit() const;
    inline void minCoord (const Vector2<T>& iVect);
    inline void maxCoord (const Vector2<T>& iVect);
    inline T norm() const;
    inline T normSquare() const;
    inline Vector2<T>& normalise();
    inline Vector2<T> projectOn( const Vector2<T>& );
    
    // --------------- Overload: operateurs unitaires --------------------------
    inline Vector2<T>&  operator=  (const Vector2<T> &vect);
    inline Vector2<T>&  operator=  (const T &val);
    
    inline Vector2<T>  operator+  (const Vector2<T> &vect) const;
    inline Vector2<T>  operator+  (const double &val) const;
    inline Vector2<T>& operator+= (const Vector2<T> &vect);
    inline Vector2<T>& operator+= (const double &val);
    
    inline Vector2<T>  operator-  (const Vector2<T> &vect) const;
    inline Vector2<T>  operator-  (const double &val) const;
    inline Vector2<T>  operator-  ();
    inline Vector2<T>& operator-= (const Vector2<T> &vect);
    inline Vector2<T>& operator-= (const double &val);
    
    inline Vector2<T>  operator*  (const double &val) const;    
    inline Vector2<T>& operator*= (const double &val);
    
    inline Vector2<T>  operator/  (const double &val) const;
    inline Vector2<T>& operator/= (const double &val);  
    inline bool operator== ( const Vector2<T>& ) const;
    inline bool operator!= ( const Vector2<T>& ) const;
  
    // -------------- Overload: produit vectoriel ------------------------------
    inline Vector3<T>  operator^  (const Vector2<T> &vect) const;
    
    // -------------- Overload: produit scalaire -------------------------------
    inline T          operator*  (const Vector2<T> &vect) const;
    
  protected:
  private:
  
    T mData[2];
  };
  
  template<class T>
  inline Vector2<T>::Vector2()
  { mData[0] = mData[1] = (T)0; }
  
  //!---------------------------------------------------------------------------
  /*permet la conversion implicite entre les type int, double, float etc...*/
  template<class T>
  template<class U>
  Vector2<T>::Vector2( const Vector2<U>& iV )
  { set( (T)iV.x(), (T)iV.y() ); }
  
  //!---------------------------------------------------------------------------
  template<class T>
  inline Vector2<T>::Vector2(const T &val)
  { set(val); }
  
  //!---------------------------------------------------------------------------
  template<class T>
  inline Vector2<T>::Vector2(const T &x, const T &y)
  { set(x, y); }
  
  //!---------------------------------------------------------------------------
  template<class T>
  inline Vector2<T>::Vector2(const Vector2<T> &vect)
  { memcpy((void*)mData, (const void*)vect.mData, 2 * sizeof(T)); }
  
  //!---------------------------------------------------------------------------
  //! \brief  Constructeur avec parametres.
  //!
  //! Construit un objet de type \c Vect a partir de 2 points.
  //! 1--------->2  vecteur = 2 - 1
  //! 
  //! \param &pt1 le premier point
  //! \param &pt2 le deuxieme point
  //!---------------------------------------------------------------------------
  template<class T>
  inline Vector2<T>::Vector2(const Point2<T> &pt1, const Point2<T> &pt2)
  { set(pt1, pt2); }
  
  //!---------------------------------------------------------------------------
  template<class T>
  Vector2<T>::~Vector2()
  {}
  
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vector2<T>::set(const Vector2<T> &vect)
  { memcpy((void*)mData, (const void*)vect.mData, 2 * sizeof(T)); }
  
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vector2<T>::set(const T &val)
  { mData[0] = mData[1] = (T)val; }
  
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vector2<T>::setX(const T &x)
  { mData[0] = x; }
  
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vector2<T>::setY(const T &y)
  { mData[1] = y; }
  
  //!---------------------------------------------------------------------------
    template<class T>
  inline void Vector2<T>::set(const T &x, const T &y)
  { mData[0] =x; mData[1] = y; }
  
  //!---------------------------------------------------------------------------
  //! \brief  Constructeur avec parametres.
  //!
  //! Construit un objet de type \c Vect a partir de 2 points.
  //! 1--------->2  vecteur = 2 - 1
  //! 
  //! \param &pt1 le premier point
  //! \param &pt2 le deuxieme point
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vector2<T>::set(const Point2<T> &pt1, const Point2<T> &pt2)
  {
    mData[0] = (pt2.x() - pt1.x());
    mData[1] = (pt2.y() - pt1.y());
  }
  
  //!---------------------------------------------------------------------------
  template<class T>
  inline const T& Vector2<T>::x() const
  { return mData[0]; }
  
  //!---------------------------------------------------------------------------
  template<class T>
  inline const T& Vector2<T>::y() const
  { return mData[1]; }
  
  //----------------------------------------------------------------------------  
  template<class T>
  inline const T* Vector2<T>::getPtr() const
  { return &mData[0]; }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline Vector2<T> Vector2<T>::getUnit() const
  { return Vector2<T>( *this / max(this->norm(), 1.0) ); }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline void Vector2<T>::minCoord (const Vector2<T>& iVect)
  {
    setX (std::min (x(), iVect.x() ) );
    setY (std::min (y(), iVect.y() ) );
  }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline void Vector2<T>::maxCoord (const Vector2<T>& iVect)
  {
    setX (std::max (x(), iVect.x() ) );
    setY (std::max (y(), iVect.y() ) );
  }
    
  //!---------------------------------------------------------------------------
  //! \brief  Calcule et retourne la norme du vecteur
  //!
  //! \return la norme du vecteur
  //!---------------------------------------------------------------------------
  template<class T>
  inline T Vector2<T>::norm() const
  { return std::sqrt(normSquare()); }
  
  //!---------------------------------------------------------------------------
  //retourne le carré de la norme
  template<class T>
  inline T Vector2<T>::normSquare() const
  { return mData[0]*mData[0] + mData[1]*mData[1]; }
  
  //!---------------------------------------------------------------------------
  //! \brief  Calcule et retourne un vecteur normalisÈ
  //!
  //! \return un vecteur normalisÈ
  //!---------------------------------------------------------------------------
  template<class T>
  inline Vector2<T>& Vector2<T>::normalise()
  {
    (*this) /= this->norm();
    return (*this);
  }
  
  //----------------------------------------------------------------------------
  template<class T>
  Vector2<T> Vector2<T>::projectOn( const Vector2<T>& v )
  {
    double a = *this * v.getUnit();
    return v.getUnit() * a;
  }

  //----------------------------------------------------------------------------
  //strictement egale, pour une comparaison un peu plus permissive, voir
  //mathUtil isEqual( const Vect<T> &, const Vect<T> &, double )
  template<class T>
  inline bool Vector2<T>::operator== (const Vector2<T>& iV) const
  { return x() == iV.x() && y() == iV.y(); }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline bool Vector2<T>::operator!= ( const Vector2<T>& iV ) const
  { return !( *this == iV ); }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline Vector2<T>& Vector2<T>::operator= (const Vector2<T> &vect)
  {
    mData[0] = vect.x();
    mData[1] = vect.y();
    return *this;
  }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline Vector2<T>& Vector2<T>::operator= (const T &val)
  {
    set(val);
    return *this;
  }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline Vector2<T> Vector2<T>::operator+ (const Vector2<T> &vect) const
  {
    Vector2<T> r;
    r.mData[0] = x() + vect.x();
    r.mData[1] = y() + vect.y();
    return r;
  }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline Vector2<T> Vector2<T>::operator+ (const double &val) const
  {
    Vector2<T> r;
    r.mData[0] = x() + val;
    r.mData[1] = y() + val;
    return r;
  }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline Vector2<T>& Vector2<T>::operator+= (const Vector2<T> &vect)
  {
    mData[0] += vect.x();
    mData[1] += vect.y();
    return *this;
  }
    
  //----------------------------------------------------------------------------
  template<class T>
  inline Vector2<T>& Vector2<T>::operator+= (const double &val)
  {
    mData[0] += val;
    mData[1] += val;
    return *this;
  }
  
  //----------------------------------------------------------------------------
  template<class T>  
  inline Vector2<T> Vector2<T>::operator- (const Vector2<T> &vect) const
  {
    Vector2<T> r;
    r.mData[0] = x() - vect.x();
    r.mData[1] = y() - vect.y();
    return r;
  }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline Vector2<T> Vector2<T>::operator- (const double &val) const
  {
    Vector2<T> r;
    r.mData[0] = x() - val;
    r.mData[1] = y() - val;
    return r;
  }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline Vector2<T> Vector2<T>::operator- ()
  { 
    Vector2<T> r;
    r.mData[0] = -mData[0];
    r.mData[1] = -mData[1];
    return r;
  }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline Vector2<T>& Vector2<T>::operator-= (const Vector2<T> &vect)
  {
    mData[0] -= vect.x();
    mData[1] -= vect.y();
    return *this;
  }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline Vector2<T>& Vector2<T>::operator-= (const double &val)
  {
    mData[0] -= val;
    mData[1] -= val;
    return *this;
  }
    
  //----------------------------------------------------------------------------
  template<class T>
  inline Vector2<T> Vector2<T>::operator* (const double &val) const
  {
    Vector2<T> r;
    r.mData[0] = x() * val;
    r.mData[1] = y() * val;
    return r;
  }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline Vector2<T>& Vector2<T>::operator*= (const double &val)
  {
    mData[0] *= val;
    mData[1] *= val;
    return *this;
  }
  
  //----------------------------------------------------------------------------
  template<class T>  
  inline Vector2<T> Vector2<T>::operator/ (const double &val) const
  {
    double vTmp;
    Vector2<T> vect(*this);
    
    if(val>EPSILON || val<-EPSILON)
      vTmp=((double)1.0)/(double)val;
    else
      vTmp=(double)0.0;
      
    vect *= vTmp;
    return vect;
  }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline Vector2<T>& Vector2<T>::operator/= (const double &val)
  {
    double vTmp;
    
    if(val>EPSILON || val<-EPSILON)
      vTmp=((double)1.0)/(double)val;
    else
      vTmp=(double)0.0;
      
		*this *= vTmp;
    return *this;
  }
  //----------------------------------------------------------------------------
  //produit vectoriel
  template<class T>
  inline Vector3<T> Vector2<T>::operator^ (const Vector2<T> &vect) const
  {
    Vector3<T> r;
    r.mData[0] = 0;
    r.mData[1] = 0;
    r.mData[2] = x() * vect.y() - y() * vect.x();
    return r;
  }

  //----------------------------------------------------------------------------
  //produit scalaire
  template<class T>
  inline T Vector2<T>::operator* (const Vector2<T> &vect) const
  {
    T res = x() * vect.x() + y() * vect.y();
    return res;
  }
  
  typedef Vector3<float>   Vector3f;
  typedef Vector3<double>  Vector3d;
  typedef Vector3<int>     Vector3i;
  typedef Vector2<float>   Vector2f;
  typedef Vector2<double>   Vector2d;
  typedef Vector2<int>   Vector2i;
  
} //math
} // enf of namespace realisim

#endif //VECT_H
