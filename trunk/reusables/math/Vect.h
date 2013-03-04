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
  template<class T>
  class Vect 
  {
  public:
  
    // --------------- constructeurs -------------------------------------------
    inline Vect();
    inline explicit Vect(const T &val);
    inline Vect(const T &x, const T &y, const T &z);
    inline Vect(const Vect<T> &vect);    
    inline Vect(const Point<T> &pt1, const Point<T> &pt2);
    
    // --------------- destructeurs --------------------------------------------
    virtual ~Vect();
    
    // --------------- fonction set --------------------------------------------
    inline void set(const Vect<T> &vect);
    inline void set(const T &val);
    inline void setX(const T &x);
    inline void setY(const T &y);
    inline void setZ(const T &z);
    inline void setXYZ(const T &x, const T &y, const T &z);    
    inline void set(const Point<T> &pt1, const Point<T> &pt2);
    
    // --------------- fonction get --------------------------------------------
    inline const T& getX() const;
    inline const T& getY() const;
    inline const T& getZ() const;
    inline void get(Vect<T> &vect) const;
    inline void getXYZ(T &x, T &y, T &z) const;
    inline const T* getPtr() const;
//inline std::String toString() const;
    
    // --------------- fonction utiles -----------------------------------------
    inline void minCoord (const Vect<T>& iVect);
    inline void maxCoord (const Vect<T>& iVect);
    inline T norm() const;
    inline const Vect<T>& normalise();
    inline T fastNorm() const;
    
    // --------------- Overload: operateurs unitaires --------------------------
    inline Vect<T>&  operator=  (const Vect<T> &vect);
    inline Vect<T>&  operator=  (const T &val);
    
    inline Vect<T>  operator+  (const Vect<T> &vect) const;
    inline Vect<T>  operator+  (const T &val) const;
    inline Vect<T>& operator+= (const Vect<T> &vect);
    inline Vect<T>& operator+= (const T &val);
    
    inline Vect<T>  operator-  (const Vect<T> &vect) const;
    inline Vect<T>  operator-  (const T &val) const;
    inline Vect<T>  operator-  ();
    inline Vect<T>& operator-= (const Vect<T> &vect);
    inline Vect<T>& operator-= (const T &val);
    
    inline Vect<T>  operator*  (const T &val) const;
    inline Vect<T>  operator*  (const Vect<T>& vect) const;
    inline Vect<T>& operator*= (const T &val);
    
    inline Vect<T>  operator/  (const T &val) const;
    inline Vect<T>& operator/= (const T &val);  
    inline bool operator== ( const Vect<T>& ) const;
    inline bool operator!= ( const Vect<T>& ) const;
  
    // -------------- Overload: produit vectoriel ------------------------------
    inline Vect<T>  operator^  (const Vect<T> &vect) const;
    
    // -------------- Overload: produit scalaire -------------------------------
    inline T          operator&  (const Vect<T> &vect) const;
    
  protected:
  private:
  
    T mData[3];
  };
  
  //! constructeur par defaut.
  template<class T>
  inline Vect<T>::Vect()
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
  inline Vect<T>::Vect(const T &val)
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
  inline Vect<T>::Vect(const T &x, const T &y, const T &z)
  { setXYZ(x, y, z); }
  
  //! constructeur copie
  template<class T>
  inline Vect<T>::Vect(const Vect<T> &vect)
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
  inline Vect<T>::Vect(const Point<T> &pt1, const Point<T> &pt2)
  { set(pt1, pt2); }
  
  //! destructeur
  template<class T>
  Vect<T>::~Vect()
  {}
  
  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur du vecteur.
  //!
  //! egale le vecteur a celui passe en parametre.
  //! 
  //! \param &vect
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vect<T>::set(const Vect<T> &vect)
  { memcpy((void*)mData, (const void*)vect.mData, 3 * sizeof(T)); }
  
  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur du vecteur.
  //!
  //! egale le vecteur a la valeur passee en parametre.
  //! 
  //! \param &val
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vect<T>::set(const T &val)
  { mData[0] = mData[1] = mData[2] = (T)val; }
  
  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur x_ du vecteur.
  //! 
  //! \param &x
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vect<T>::setX(const T &x)
  { mData[0] = x; }
  
  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur y_ du vecteur.
  //! 
  //! \param &y
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vect<T>::setY(const T &y)
  { mData[1] = y; }
  
  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur z_ du vecteur.
  //! 
  //! \param &z
  //!---------------------------------------------------------------------------
  template<class T>
  inline void Vect<T>::setZ(const T &z)
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
  inline void Vect<T>::setXYZ(const T &x, const T &y, const T &z)
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
  inline void Vect<T>::set(const Point<T> &pt1, const Point<T> &pt2)
  {
    mData[0] = (pt2.getX() - pt1.getX());
    mData[1] = (pt2.getY() - pt1.getY());
    mData[2] = (pt2.getZ() - pt1.getZ());
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
  { memcpy((void*)vect.mData, (const void*)mData, 3 * sizeof(T)); }
  
  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir la valeur x du vecteur.
  //! 
  //! \return la valeur X du vecteur
  //!---------------------------------------------------------------------------
  template<class T>
  inline const T& Vect<T>::getX() const
  { return mData[0]; }
  
  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir la valeur y du vecteur.
  //! 
  //! \return la valeur Y du vecteur
  //!---------------------------------------------------------------------------
  template<class T>
  inline const T& Vect<T>::getY() const
  { return mData[1]; }
  
  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir la valeur z du vecteur.
  //! 
  //! \return la valeur Z du vecteur
  //!---------------------------------------------------------------------------
  template<class T>
  inline const T& Vect<T>::getZ() const
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
  inline void Vect<T>::getXYZ(T &x, T &y, T &z) const
  {
    x = mData[0];
    y = mData[1];
    z = mData[2];
  }
  
  template<class T>
  inline const T* Vect<T>::getPtr() const
  { return &mData[0]; }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline void Vect<T>::minCoord (const Vect<T>& iVect)
  {
    setX (std::min (getX (), iVect.getX ()));
    setY (std::min (getY (), iVect.getY ()));
    setZ (std::min (getZ (), iVect.getZ ()));
  }
  
  //----------------------------------------------------------------------------
  template<class T>
  inline void Vect<T>::maxCoord (const Vect<T>& iVect)
  {
    setX (std::max (getX (), iVect.getX ()));
    setY (std::max (getY (), iVect.getY ()));
    setZ (std::max (getZ (), iVect.getZ ()));
  }
    
  //!---------------------------------------------------------------------------
  //! \brief  Calcule et retourne la norme du vecteur
  //!
  //! \return la norme du vecteur
  //!---------------------------------------------------------------------------
  template<class T>
  inline T Vect<T>::norm() const
  {
    return std::sqrt(mData[0]*mData[0] + mData[1]*mData[1] + mData[2]*mData[2]);
  }
  
  //!---------------------------------------------------------------------------
  //! \brief  Calcule et retourne un vecteur normalisÈ
  //!
  //! \return un vecteur normalisÈ
  //!---------------------------------------------------------------------------
  template<class T>
  inline const Vect<T>& Vect<T>::normalise()
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
    
    max = getX();
    med = getY();
    min = getZ();
    
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
  //mathUtil equal( const Vect<T> &, const Vect<T> &, double )
  template<class T>
  inline bool Vect<T>::operator== (const Vect<T>& iV) const
  {
  	return getX() == iV.getX() &&
    	getY() == iV.getY() &&
      getZ() == iV.getZ();
  }
  //----------------------------------------------------------------------------
  template<class T>
  inline bool Vect<T>::operator!= ( const Vect<T>& iV ) const
  { return !( *this == iV ); }
  //----------------------------------------------------------------------------
  //! surcharge opÈrateur = Vect
  template<class T>
  inline Vect<T>& Vect<T>::operator= (const Vect<T> &vect)
  {
    mData[0] = vect.getX();
    mData[1] = vect.getY();
    mData[2] = vect.getZ();
    return *this;
  }
  
  //! surcharge opÈrateur = T
  template<class T>
  inline Vect<T>& Vect<T>::operator= (const T &val)
  {
    set(val);
    return *this;
  }
    
  //! surcharge opÈrateur + avec un vecteur
  template<class T>
  inline Vect<T> Vect<T>::operator+ (const Vect<T> &vect) const
  {
    Vect<T> r;
    
    r.mData[0] = getX() + vect.getX();
    r.mData[1] = getY() + vect.getY();
    r.mData[2] = getZ() + vect.getZ();
    
    return r;
  }
  
  //! surcharge opÈrateur + avec T
  template<class T>
  inline Vect<T> Vect<T>::operator+ (const T &val) const
  {
    Vect<T> r;
    
    r.mData[0] = getX() + val;
    r.mData[1] = getY() + val;
    r.mData[2] = getZ() + val;
    
    return r;
  }
  
  //! surcharge opÈrateur += avec un vecteur
  template<class T>
  inline Vect<T>& Vect<T>::operator+= (const Vect<T> &vect)
  {
    mData[0] += vect.getX();
    mData[1] += vect.getY();
    mData[2] += vect.getZ();
    return *this;
  }
    
  //! surcharge opÈrateur += avec T
  template<class T>
  inline Vect<T>& Vect<T>::operator+= (const T &val)
  {
    mData[0] += val;
    mData[1] += val;
    mData[2] += val;
    return *this;
  }
  
  //! surcharge opÈrateur - avec un vecteur
  template<class T>  
  inline Vect<T> Vect<T>::operator- (const Vect<T> &vect) const
  {
    Vect<T> r;
    
    r.mData[0] = getX() - vect.getX();
    r.mData[1] = getY() - vect.getY();
    r.mData[2] = getZ() - vect.getZ();
    
    return r;
  }
  
  //! surcharge opÈrateur - avec T
  template<class T>
  inline Vect<T> Vect<T>::operator- (const T &val) const
  {
    Vect<T> r;
    
    r.mData[0] = getX() - val;
    r.mData[1] = getY() - val;
    r.mData[2] = getZ() - val;
    
    return r;
  }
  
  //! surcharge opÈrateur -
  template<class T>
  inline Vect<T> Vect<T>::operator- ()
  { 
    Vect<T> r;
       
    r.mData[0] = -mData[0];
    r.mData[1] = -mData[1];
    r.mData[2] = -mData[2];
    
    return r;
  }
  
  //! surcharge opÈrateur -= avec un vecteur
  template<class T>
  inline Vect<T>& Vect<T>::operator-= (const Vect<T> &vect)
  {
    mData[0] -= vect.getX();
    mData[1] -= vect.getY();
    mData[2] -= vect.getZ();
    return *this;
  }
  
  //! surcharge opÈrateur -= avec T
  template<class T>
  inline Vect<T>& Vect<T>::operator-= (const T &val)
  {
    mData[0] -= val;
    mData[1] -= val;
    mData[2] -= val;
    return *this;
  }
    
  //----------------------------------------------------------------------------
  //! surcharge opÈrateur * avec T
  template<class T>
  inline Vect<T> Vect<T>::operator* (const T &val) const
  {
    Vect<T> r;
    
    r.mData[0] = getX() * val;
    r.mData[1] = getY() * val;
    r.mData[2] = getZ() * val;
    
    return r;
  }
  
  //----------------------------------------------------------------------------
  //! surcharge opÈrateur * avec T
  template<class T>
  inline Vect<T> Vect<T>::operator* (const Vect<T>& vect) const
  {
    Vect<T> r;
    
    r.mData[0] = getX() * vect.getX();
    r.mData[1] = getY() * vect.getY();
    r.mData[2] = getZ() * vect.getZ();
    
    return r;
  }
  
  //----------------------------------------------------------------------------
  //! surcharge opÈrateur *= avec T
  template<class T>
  inline Vect<T>& Vect<T>::operator*= (const T &val)
  {
    mData[0] *= val;
    mData[1] *= val;
    mData[2] *= val;
    return *this;
  }
  
  //----------------------------------------------------------------------------
  //! surcharge opÈrateur / avec T
  template<class T>  
  inline Vect<T> Vect<T>::operator/ (const T &val) const
  {
    double vTmp;
    Vect<T> vect(*this);
    
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
  inline Vect<T>& Vect<T>::operator/= (const T &val)
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
  //! Overload: produit vectoriel
  template<class T>
  inline Vect<T> Vect<T>::operator^ (const Vect<T> &vect) const
  {
    Vect<T> r;
    
    r.mData[0] = getY() * vect.getZ() - getZ() * vect.getY();
    r.mData[1] = getZ() * vect.getX() - getX() * vect.getZ();
    r.mData[2] = getX() * vect.getY() - getY() * vect.getX();
    
    return r;
  }
    
  //! Overload: produit scalaire
  template<class T>
  inline T Vect<T>::operator& (const Vect<T> &vect) const
  {
    T res = getX() * vect.getX() + getY() * vect.getY() + getZ() * vect.getZ();
    return res;
  }
  
  typedef Vect<float>   Vector3f;
  typedef Vect<double>  Vector3d;
  typedef Vect<int>     Vector3i;
  
} //math
} // enf of namespace realisim

#endif //VECT_H
