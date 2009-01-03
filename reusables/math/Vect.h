//!-----------------------------------------------------------------------------
//! \file
//! \brief Classe \c Vect permet de reprÈsenter un vecteur a trois dimensions
//!
//! AUTHOR:  Pierre-Olivier Beaudoin & David Pinson
//!-----------------------------------------------------------------------------

#ifndef VECT_H
#define VECT_H

#include "Point.h"
#include "MathDef.h"

//!-----------------------------------------------------------------------------
//! \brief Classe gÈrant les fonctionalitÈs associÈes a un vecteur
//!-----------------------------------------------------------------------------
namespace Realisim
{
  template<class T>
  class Vect 
  {
  public:
  
    // --------------- constructeurs -------------------------------------------
    inline Vect();
    inline Vect(const T &val);
    inline Vect(const T &x, const T &y, const T &z);
    inline Vect(const Vect<T> &vect);
    
    template<class U>
    inline Vect(const Point<U> &pt1, const Point<U> &pt2);
    
    // --------------- destructeurs --------------------------------------------
    virtual ~Vect();
    
    // --------------- fonction set --------------------------------------------
    inline void set(const Vect<T> &vect);
    inline void set(const T &val);
    inline void setX(const T &x);
    inline void setY(const T &y);
    inline void setZ(const T &z);
    inline void setXYZ(const T &x, const T &y, const T &z);
    
    template<class U>
    inline void set(const Point<U> &pt1, const Point<U> &pt2);
    
    // --------------- fonction get --------------------------------------------
    inline T getX() const;
    inline T getY() const;
    inline T getZ() const;
    inline void get(Vect<T> &vect) const;
    inline void getXYZ(T &x, T &y, T &z) const;
    
    // --------------- fonction utiles -----------------------------------------
    inline T norm() const;
    inline Vect<T> normalise();
    inline T fastNorm() const;
    
    // --------------- Overload: operateurs unitaires --------------------------
    inline bool operator== (const Vect<T> &vect) const;
    inline Vect<T>&  operator=  (const Vect<T> &vect);
    inline Vect<T>&  operator=  (const T &val);
    
    inline Vect<T>  operator+  (const Vect<T> &vect) const;
    inline Vect<T>  operator+  (const T &val) const;
    inline Vect<T>& operator+= (const Vect<T> &vect);
    inline Vect<T>& operator+= (const T &val);
    
    inline Vect<T>  operator-  (const Vect<T> &vect) const;
    inline Vect<T>  operator-  (const T &val) const;
    inline Vect<T>& operator-  ();
    inline Vect<T>& operator-= (const Vect<T> &vect);
    inline Vect<T>& operator-= (const T &val);
    
    inline Vect<T>  operator*  (const T &val) const;
    inline Vect<T>& operator*= (const T &val);
    //inline  Vect&  operator*= (const Matrix&) ; TODO ???
    
    inline Vect<T>  operator/  (const T &val) const;
    inline Vect<T>& operator/= (const T &val);  
  
    // -------------- Overload: produit vectoriel ------------------------------
    inline Vect<T>  operator^  (const Vect<T> &vect) const;
    
    // -------------- Overload: produit scalaire -------------------------------
    inline T          operator&  (const Vect<T> &vect) const;
    
  protected:
  private:
  
    T x_;
    T y_;
    T z_;
  };
  
  //! constructeur par dÈfaut.
  template<class T>
  inline Vect<T>::Vect()
  {
    x_=(T)0.0;
    y_=(T)0.0;
    z_=(T)0.0;
  }
  
  //!---------------------------------------------------------------------------
  //! \brief  Constructeur avec parametre.
  //!
  //! Construit un objet de type \c Vect ou chacune des valeurs cartÈsiennes 
  //! vaut la meme valeur.
  //! 
  //! \param val valeur d'initialisation
  //!---------------------------------------------------------------------------
  template<class T>
  inline Vect<T>::Vect(const T &val)
  {
    x_=val;
    y_=val;
    z_=val;
  }
  
  //!---------------------------------------------------------------------------
  //! \brief  Constructeur avec parametres.
  //!
  //! Construit un objet de type \c Vect avec chacune des valeurs cartÈsiennes 
  //! passÈes en parametre.
  //! 
  //! \param x valeur d'initialisation pour x_
  //! \param y valeur d'initialisation pour y_
  //! \param z valeur d'initialisation pour z_
  //!---------------------------------------------------------------------------
  template<class T>
  inline Vect<T>::Vect(const T &x, const T &y, const T &z)
  {
    x_=x;
    y_=y;
    z_=z;
  }
  
  //! constructeur copie
  template<class T>
  inline Vect<T>::Vect(const Vect<T> &vect)
  {
    x_=vect.x_;
    y_=vect.y_;
    z_=vect.z_;
  }
  
  //!---------------------------------------------------------------------------
  //! \brief  Constructeur avec parametres.
  //!
  //! Construit un objet de type \c Vect a partir de 2 points.
  //! 1--------2  vecteur = 2 - 1
  //! 
  //! \param &pt1 le premier point
  //! \param &pt2 le deuxieme point
  //!---------------------------------------------------------------------------
  template<class T>
  template<class U>
  inline Vect<T>::Vect(const Point<U> &pt1, const Point<U> &pt2)
  {
    //x_ = (T)(pt2.getX() - pt1.getX());
    //y_ = (T)(pt2.getY() - pt1.getY());
    //z_ = (T)(pt2.getZ() - pt1.getZ());

    set(pt1, pt2);
  }
  
  //! destructeur
  template<class T>
  Vect<T>::~Vect()
  {
    ;
  }
  
  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur du vecteur.
  //!
  //! …gale le vecteur a celui passÈ en parametre.
  //! 
  //! \param &vect
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vect<T>::set(const Vect<T> &vect)
  {
    x_=vect.x_;
    y_=vect.y_;
    z_=vect.z_;
  }
  
  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur du vecteur.
  //!
  //! …gale le vecteur a la valeur passÈe en parametre.
  //! 
  //! \param &val
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vect<T>::set(const T &val)
  {
    x_=val;
    y_=val;
    z_=val;
  }
  
  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur x_ du vecteur.
  //! 
  //! \param &x
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vect<T>::setX(const T &x)
  {
    x_=x;
  }
  
  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur y_ du vecteur.
  //! 
  //! \param &y
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vect<T>::setY(const T &y)
  {
    y_=y;
  }
  
  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur z_ du vecteur.
  //! 
  //! \param &z
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vect<T>::setZ(const T &z)
  {
    z_=z;
  }
  
  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur du vecteur.
  //!
  //! …gale le vecteur a chacune des valeurs passÈees en parametre.
  //! 
  //! \param &x
  //! \param &y
  //! \param &z
  //!---------------------------------------------------------------------------
    template<class T>
  inline void Vect<T>::setXYZ(const T &x, const T &y, const T &z)
  {
    x_=x;
    y_=y;
    z_=z;
  }
  
  //! fonction permettant de setter un vecteur avec 2 points
  template<class T>
  template<class U>
  inline void Vect<T>::set(const Point<U> &pt1, const Point<U> &pt2)
  {
    x_ = (T)(pt2.getX() - pt1.getX());
    y_ = (T)(pt2.getY() - pt1.getY());
    z_ = (T)(pt2.getZ() - pt1.getZ());
  }
  
  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir le vecteur.
  //!
  //! Permet d'obtenir la valeur du vecteur dans celui passÈ en parametre.
  //! 
  //! \param &vecteur Le vecteur qui va etre comme le celui que l'on veut
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vect<T>::get(Vect<T> &vect) const
  {
    vect.x_=x_;
    vect.y_=y_;
    vect.z_=z_;
  }
  
  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir la valeur x du vecteur.
  //! 
  //! \return la valeur X du vecteur
  //!---------------------------------------------------------------------------
  template<class T>
  inline T Vect<T>::getX() const
  {
    return x_;
  }
  
  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir la valeur y du vecteur.
  //! 
  //! \return la valeur Y du vecteur
  //!---------------------------------------------------------------------------
  template<class T>
  inline T Vect<T>::getY() const
  {
    return y_;
  }
  
  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir la valeur z du vecteur.
  //! 
  //! \return la valeur Z du vecteur
  //!---------------------------------------------------------------------------
  template<class T>
  inline T Vect<T>::getZ() const
  {
    return z_;
  }
  
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
  inline void Vect<T>::getXYZ(T &x, T &y, T &z) const
  {
    x=x_;
    y=y_;
    z=z_;
  }
  
  //!---------------------------------------------------------------------------
  //! \brief  Calcule et retourne la norme du vecteur
  //!
  //! \return la norme du vecteur
  //!---------------------------------------------------------------------------
  template<class T>
  inline T Vect<T>::norm() const
  {
    return std::sqrt(x_*x_ + y_*y_ + z_*z_);
  }
  
  //!---------------------------------------------------------------------------
  //! \brief  Calcule et retourne un vecteur normalisÈ
  //!
  //! \return un vecteur normalisÈ
  //!---------------------------------------------------------------------------
  template<class T>
  inline Vect<T> Vect<T>::normalise()
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
  inline T Vect<T>::fastNorm() const
  {
    T max, med, min, mom;
    
    max = x_;
    med = y_;
    min = z_;
    
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
  
  //! surcharge opÈrateur ==
  template<class T>
  inline bool Vect<T>::operator== (const Vect<T> &vect) const
  {
    T dx = x_ - vect.x_;
    T dy = y_ - vect.y_;
    T dz = z_ - vect.z_;
  
    if(dx<(T)0.0)
      dx = -dx;
    if(dy<(T)0.0)
      dy = -dy;
    if(dz<(T)0.0)
      dz = -dz;
  
    if (dx<SMALL_REAL && dy<SMALL_REAL && dz<SMALL_REAL)
      return true;
    else
      return false;
  }
  
  //! surcharge opÈrateur = Vect
  template<class T>
  inline Vect<T>& Vect<T>::operator= (const Vect<T> &vect)
  {
    x_=vect.x_;
    y_=vect.y_;
    z_=vect.z_;
    return *this;
  }
  
  //! surcharge opÈrateur = T
  template<class T>
  inline Vect<T>& Vect<T>::operator= (const T &val)
  {
    x_=val;
    y_=val;
    z_=val;
    return *this;
  }
    
  //! surcharge opÈrateur + avec un vecteur
  template<class T>
  inline Vect<T> Vect<T>::operator+ (const Vect<T> &vect) const
  {
    Vect<T> vectResult;
    
    vectResult.x_=x_+vect.x_;
    vectResult.y_=y_+vect.y_;
    vectResult.z_=z_+vect.z_;
    
    return vectResult;
  }
  
  //! surcharge opÈrateur + avec T
  template<class T>
  inline Vect<T> Vect<T>::operator+ (const T &val) const
  {
    Vect<T> vect;
    
    vect.x_=x_+val;
    vect.y_=y_+val;
    vect.z_=z_+val;
    
    return vect;
  }
  
  //! surcharge opÈrateur += avec un vecteur
  template<class T>
  inline Vect<T>& Vect<T>::operator+= (const Vect<T> &vect)
  {
    x_+=vect.x_;
    y_+=vect.y_;
    z_+=vect.z_;
    return *this;
  }
    
  //! surcharge opÈrateur += avec T
  template<class T>
  inline Vect<T>& Vect<T>::operator+= (const T &val)
  {
    x_+=val;
    y_+=val;
    z_+=val;
    return *this;
  }
  
  //! surcharge opÈrateur - avec un vecteur
  template<class T>  
  inline Vect<T> Vect<T>::operator- (const Vect<T> &vect) const
  {
    Vect<T> vectResult;
    
    vectResult.x_=x_-vect.x_;
    vectResult.y_=y_-vect.y_;
    vectResult.z_=z_-vect.z_;
    
    return vectResult;
  }
  
  //! surcharge opÈrateur - avec T
  template<class T>
  inline Vect<T> Vect<T>::operator- (const T &val) const
  {
    Vect<T> vect;
    
    vect.x_=x_-val;
    vect.y_=y_-val;
    vect.z_=z_-val;
    
    return vect;
  }
  
  //! surcharge opÈrateur -
  template<class T>
  inline Vect<T>& Vect<T>::operator- ()
  {    
    x_ = -x_;
    y_ = -y_;
    z_ = -z_;
    
    return *this;
  }
  
  //! surcharge opÈrateur -= avec un vecteur
  template<class T>
  inline Vect<T>& Vect<T>::operator-= (const Vect<T> &vect)
  {
    x_-=vect.x_;
    y_-=vect.y_;
    z_-=vect.z_;
    return *this;
  }
  
  //! surcharge opÈrateur -= avec T
  template<class T>
  inline Vect<T>& Vect<T>::operator-= (const T &val)
  {
    x_-=val;
    y_-=val;
    z_-=val;
    return *this;
  }
    
  //! surcharge opÈrateur * avec T
  template<class T>
  inline Vect<T> Vect<T>::operator* (const T &val) const
  {
    Vect<T> vect;
    
    vect.x_=x_*val;
    vect.y_=y_*val;
    vect.z_=z_*val;
    
    return vect;
  }
  
  //! surcharge opÈrateur *= avec T
  template<class T>
  inline Vect<T>& Vect<T>::operator*= (const T &val)
  {
    x_*=val;
    y_*=val;
    z_*=val;
    return *this;
  }
  
  //! surcharge opÈrateur / avec T
  template<class T>  
  inline Vect<T> Vect<T>::operator/ (const T &val) const
  {
    T vTmp;
    Vect<T> vect(x_, y_, z_);
    
    if(val>SMALL_REAL || val<-SMALL_REAL)
      vTmp=((T)1.0)/val;
    else
      vTmp=(T)0.0;
      
    vect.x_*=vTmp;
    vect.y_*=vTmp;
    vect.z_*=vTmp;
    
    return vect;
  }
  
  //! surcharge operateur /= avec T
  template<class T>
  inline Vect<T>& Vect<T>::operator/= (const T &val)
  {
    T vTmp;
    
    if(val>SMALL_REAL || val<-SMALL_REAL)
      vTmp=((T)1.0)/val;
    else
      vTmp=(T)0.0;
      
    x_*=vTmp;
    y_*=vTmp;
    z_*=vTmp;
    
    return *this;
  }
  
  //! Overload: produit vectoriel
  template<class T>
  inline Vect<T> Vect<T>::operator^ (const Vect<T> &vect) const
  {
    Vect<T> vectResult;
    
    vectResult.x_ = y_*vect.z_ - z_*vect.y_;
    vectResult.y_ = z_*vect.x_ - x_*vect.z_;
    vectResult.z_ = x_*vect.y_ - y_*vect.x_;
    
    return vectResult;
  }
    
  //! Overload: produit scalaire
  template<class T>
  inline T Vect<T>::operator& (const Vect<T> &vect) const
  {
    T res = x_*vect.x_ + y_*vect.y_ + z_*vect.z_;
    return res;
  }
  
  typedef Vect<float>   Vector3f;
  typedef Vect<double>  Vector3d;
  typedef Vect<int>     Vector3i;
  
} // enf of namespace realisim

#endif //VECT_H
