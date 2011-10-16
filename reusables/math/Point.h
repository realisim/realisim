//!-----------------------------------------------------------------------------
//! \file
//! \brief Classe \c Point permet de representer un point dans l'espace 3D.
//!
//! AUTHOR:  Pierre-Olivier Beaudoin
//!          David Pinson
//!-----------------------------------------------------------------------------
#ifndef realisim_math_POINT_H
#define realisim_math_POINT_H

#include "MathDef.h"
#include <iostream>

//!-----------------------------------------------------------------------------
//! \brief Classe gerant les fonctionalites associees a un point
//!
//! Il s'agit d'une classe point tout ce qu'il a de plus simple. Recherche
//! d'une certaine efficacite a l'execution.
//!-----------------------------------------------------------------------------
namespace realisim
{
namespace math
{
  template<class U>
  class Point
  {
  public:

    // --------------- constructeurs -------------------------------------------
    inline Point();
    inline explicit Point(const U &val);
    inline Point(const U &x, const U &y, const U &z);
    template <class T>
    inline Point(const Point<T> &point);

    // --------------- destructeurs --------------------------------------------
    inline ~Point();

    // --------------- fonction set --------------------------------------------
    inline void set(const Point &point);
    inline void set(const U &val);
    inline void get(const U &val);
    inline void setX(const U &x);
    inline void setY(const U &y);
    inline void setZ(const U &z);
    inline void setXYZ(const U &x, const U &y, const U &z);

    // --------------- fonction get --------------------------------------------
    inline const U& getX() const;
    inline const U& getY() const;
    inline const U& getZ() const;
    inline void get(Point &point) const;
    inline void getXYZ(U &x, U &y, U &z) const;
    inline const U* getPtr() const;

    // --------------- fonction utiles -----------------------------------------
    inline double dist(const Point &point) const;
    inline U distSqr(const Point &point) const;
    inline double fastDist(const Point &point) const;
    inline void minCoord (const Point<U>& iP);
    inline void maxCoord (const Point<U>& iP);
    inline void print() const;

    // --------------- Overload: operateurs ------------------------------------
    inline bool  operator== (const Point &point) const;
    inline Point&        operator=  (const Point &point);

  // FAIT DU SENS???
    inline Point<U>      operator-  (const Point &point) const;
    inline Point<U>      operator+  (const Point &point) const;
    inline Point<U>&     operator-= (const Point &point);
    inline Point<U>&     operator+= (const Point &point);
    inline Point<U> operator*  (const U &val) const;
    inline Point<U>& operator*= (const U &val);
    inline Point<U>  operator/  (const U &val) const;
    inline Point<U>& operator/= (const U &val);  
  //   inline Point&        operator*= (const Point &point);
  //
  //   inline Point operator* (const Point &point) const;
  //   inline Point operator+ (const Point &point) const;
    inline Point<U> operator- (const U &val) const;
    inline bool operator< (const Point<U>&) const;
    inline bool operator<= (const Point<U>&) const;
    inline bool operator> (const Point<U>&) const;
    inline bool operator>= (const Point<U>&) const;

  protected:
  private:

    U mData[3];
  };

  //! constructeur par defaut.
  template<class U>
  inline Point<U>::Point()
  { memset( (void*)mData, 0, 3*sizeof(U) ); }

  //!---------------------------------------------------------------------------
  //! \brief  Constructeur avec parametre.
  //!
  //! Construit un objet de type \c Point ou chacune des valeurs cartesiennes
  //! vaut la meme valeur.
  //!
  //! \param val valeur d'initialisation
  //!---------------------------------------------------------------------------
  template<class U>
  inline Point<U>::Point(const U &val)
  { mData[0] = mData[1] = mData[2] = val; }

  //!---------------------------------------------------------------------------
  //! \brief  Constructeur avec parametres.
  //!
  //! Construit un objet de type \c Point avec chacune des valeurs cartesiennes
  //! passees en parametre.
  //!
  //! \param x valeur d'initialisation pour x_
  //! \param y valeur d'initialisation pour y_
  //! \param z valeur d'initialisation pour z_
  //!---------------------------------------------------------------------------
  template<class U>
  inline Point<U>::Point(const U &x, const U &y, const U &z)
  { setXYZ(x, y, z); }

  //! constructeur copie qui permet les conversion implicite légale
  // int a double par exemple.
  template<class U>
  template <class T>
  inline Point<U>::Point(const Point<T> &point)
  {
    mData[0] = (U)point.getX();
    mData[1] = (U)point.getY();
    mData[2] = (U)point.getZ();
  }

  //! destructeur
  template<class U>
  inline Point<U>::~Point()
  {}

  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur du point.
  //!
  //! égale le point a celui passe en parametre.
  //!
  //! \param &point
  //!---------------------------------------------------------------------------
  template<class U>
  inline void Point<U>::set(const Point &point)
  {
    memcpy((void*)mData, (const void*)point.mData, 3*sizeof(U));
  }

  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur du point.
  //!
  //! egale le point a la valeur passee en parametre.
  //!
  //! \param &val
  //!---------------------------------------------------------------------------
  template<class U>
  inline void Point<U>::set(const U &val)
  { mData[0] = mData[1] = mData[2] = val; }

  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur x_ du point.
  //!
  //! \param &x
  //!---------------------------------------------------------------------------
  template<class U>
  inline void Point<U>::setX(const U &x)
  { mData[0] = x; }

  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur y_ du point.
  //!
  //! \param &y
  //!---------------------------------------------------------------------------
  template<class U>
  inline void Point<U>::setY(const U &y)
  { mData[1] = y; }

  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur z_ du point.
  //!
  //! \param &z
  //!---------------------------------------------------------------------------
  template<class U>
  inline void Point<U>::setZ(const U &z)
  { mData[2] = z;}

  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur du point.
  //!
  //! egale le point a chacune des valeurs passeees en parametre.
  //!
  //! \param &x
  //! \param &y
  //! \param &z
  //!---------------------------------------------------------------------------
  template<class U>
  inline void Point<U>::setXYZ(const U &x, const U &y, const U &z)
  { mData[0] = x; mData[1] = y; mData[2] = z; }

  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir le point.
  //!
  //! Permet d'obtenir la valeur du point dans celui passe en parametre.
  //!
  //! \param &point Le point qui va etre comme le point que l'on veut
  //!---------------------------------------------------------------------------
  template<class U>
  inline void Point<U>::get(Point &point) const
  { memcpy((void*)point.mData, (const void*)mData, 3*sizeof(U)); }

  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir la valeur x du point.
  //!
  //! \return la valeur X du point
  //!---------------------------------------------------------------------------
  template<class U>
  inline const U& Point<U>::getX() const
  { return mData[0]; }

  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir la valeur y du point.
  //!
  //! \return la valeur Y du point
  //!---------------------------------------------------------------------------
  template<class U>
  inline const U& Point<U>::getY() const
  { return mData[1]; }

  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir la valeur z du point.
  //!
  //! \return la valeur Z du point
  //!---------------------------------------------------------------------------
  template<class U>
  inline const U& Point<U>::getZ() const
  { return mData[2]; }

  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir le point.
  //!
  //! Le point est transfere dans les variables passees en parametres
  //!
  //! \param &x
  //! \param &y
  //! \param &z
  //!---------------------------------------------------------------------------
  template<class U>
  inline void Point<U>::getXYZ(U &x, U &y, U &z) const
  {
    x=mData[0];
    y=mData[1];
    z=mData[2];
  }
  
  //retourne le pointeur sur le début du tableau de coordonnées.
  template<class U>
  inline const U* Point<U>::getPtr() const
  { return &mData[0]; }

  //!---------------------------------------------------------------------------
  //! \brief  Calcule et retourne la distance entre deux points.
  //!
  //! \param &point le point avec qui calculer la distance
  //!
  //! \return la distance entre les deux points.
  //!---------------------------------------------------------------------------
  template<class U>
  inline double Point<U>::dist(const Point &point) const
  {
    double x = point.mData[0] - mData[0];
    double y = point.mData[1] - mData[1];
    double z = point.mData[2] - mData[2];

    return (sqrt(x*x + y*y + z*z));
  }

  //!---------------------------------------------------------------------------
  //! \brief  Calcule et retourne la distance au carre entre deux points.
  //!
  //! \param &point le point avec qui calculer la distance au carre
  //!
  //! \return la distance au carre entre les deux points.
  //!---------------------------------------------------------------------------
  template<class U>
  inline U Point<U>::distSqr(const Point &point) const
  {
    U x = point.mData[0] - mData[0];
    U y = point.mData[1] - mData[1];
    U z = point.mData[2] - mData[2];

    return (x*x + y*y + z*z);
  }

  //!---------------------------------------------------------------------------
  //! \brief  Calcule et retourne une estimation de la distance entre deux
  //!         points.
  //!
  //! Calcule une estimation de la distance entre deux points, mais plus
  //! rapidement que la methode classique (qui demande une racine carre).
  //!
  //! \param &point le point avec qui calculer la distance
  //!
  //! \return une estimation de la distance entre les deux points.
  //!---------------------------------------------------------------------------
  template<class U>
  inline double Point<U>::fastDist(const Point &point) const
  {
    double max, med, min, mom;

    max = mData[0] - point.mData[0];
    med = mData[1] - point.mData[1];
    min = mData[2] - point.mData[2];

    max = max > 0.0  ?  max  :  -max;
    med = med > 0.0  ?  med  :  -med;
    min = min > 0.0  ?  min  :  -min;

    if ( max < med )
        mom=max, max=med, med=mom;

    if ( max < min )
        mom=max, max=min, min=mom;

    if ( med < min )
        mom=med, med=min, min=mom;

    return( max + (11.0/32.0)*med + (1.0/4.0)*min );
  }
  
  //----------------------------------------------------------------------------
  template<class U>
  inline void Point<U>::minCoord (const Point<U>& iP)
  {
    setX (std::min (getX (), iP.getX ()));
    setY (std::min (getY (), iP.getY ()));
    setZ (std::min (getZ (), iP.getZ ()));
  }
  
  //----------------------------------------------------------------------------
  template<class U>
  inline void Point<U>::maxCoord (const Point<U>& iP)
  {
    setX (std::max (getX (), iP.getX ()));
    setY (std::max (getY (), iP.getY ()));
    setZ (std::max (getZ (), iP.getZ ()));
  }
  
  template<class U>
  inline void Point<U>::print() const
  {
    std::cout<<getX()<<" "<<getY()<<" "<<getZ()<<std::endl;
  }
  

  //! surcharge operateur = Point
  template<class U>
  inline Point<U>& Point<U>::operator= (const Point &point)
  {
    memcpy((void*)mData, (const void*)point.mData, 3*sizeof(U));
    return *this;
  }

  //! surcharge operateur ==
  template<class U>
  inline bool Point<U>::operator== (const Point &point) const
  {
    U dx = mData[0] - point.mData[0];
    U dy = mData[1] - point.mData[1];
    U dz = mData[2] - point.mData[2];

    if(dx<(U)0.0)
      dx = -dx;
    if(dy<(U)0.0)
      dy = -dy;
    if(dz<(U)0.0)
      dz = -dz;

    if (dx<EPSILON && dy<EPSILON && dz<EPSILON)
      return true;
    else
      return false;
  }

  template<class U>
  inline Point<U> Point<U>::operator- (const Point &point) const
  {
    Point<U> result;

    result.mData[0] = mData[0] - point.mData[0];
    result.mData[1] = mData[1] - point.mData[1];
    result.mData[2] = mData[2] - point.mData[2];

    return result;
  }
  
  //----------------------------------------------------------------------------
  template<class U>
  inline bool Point<U>::operator< (const Point<U>& iP) const
  {
  	U epsilon = std::numeric_limits<U>::epsilon();
  	if(getX() < iP.getX() - epsilon) return true;
    if(getX() > iP.getX() + epsilon) return false;
    
    if(getY() < iP.getY() - epsilon) return true;
    if(getY() > iP.getY() + epsilon) return false;
    
    if(getZ() < iP.getZ() - epsilon) return true;
    return false;
    
/*Ceci est le code preexistant au 9 septembre 2011... Ce code
  semble tout aussi arbitraire que celui que je viens d'insérer
  à l'exception que le code préexistant ne permettait pas 
  d'insérer les points dans un container ordonné (map, set etc...)*/    
//return getX() < iP.getX() && 
//      getY() < iP.getY() &&
//      getZ() < iP.getZ();
  }

  //----------------------------------------------------------------------------
  template<class U>
  inline bool Point<U>::operator<= (const Point<U>& iP) const
  {
    U epsilon = std::numeric_limits<U>::epsilon();
    if(getX() >= iP.getX() - epsilon && getX() <= iP.getX() + epsilon) return true;
    if(getX() < iP.getX() - epsilon) return true;
    if(getX() > iP.getX() + epsilon) return false;
    
    if(getY() >= iP.getY() - epsilon && getY() <= iP.getY() + epsilon) return true;
    if(getY() < iP.getY() - epsilon) return true;
    if(getY() > iP.getY() + epsilon) return false;
    
    if(getZ() >= iP.getZ() - epsilon && getZ() <= iP.getZ() + epsilon) return true;
    if(getZ() < iP.getZ() - epsilon) return true;
    return false;
/*Ceci est le code preexistant au 9 septembre 2011... Ce code
  semble tout aussi arbitraire que celui que je viens d'insérer
  à l'exception que le code préexistant ne permettait pas 
  d'insérer les points dans un container ordonné (map, set etc...)*/    
//return getX() <= iP.getX() && 
//  getY() <= iP.getY() &&
//  getZ() <= iP.getZ();
  }

  //----------------------------------------------------------------------------
  template<class U>
  inline bool Point<U>::operator> (const Point<U>& iP) const
  {
    //Voir operateur < et corriger ici au besoin
    return getX() > iP.getX() && 
      getY() > iP.getY() &&
      getZ() > iP.getZ();
  }
  
  //----------------------------------------------------------------------------
  template<class U>
  inline bool Point<U>::operator>= (const Point<U>& iP) const
  {
    //Voir operateur <= et corriger ici au besoin
    return getX() >= iP.getX() && 
      getY() >= iP.getY() &&
      getZ() >= iP.getZ();
  }


  template<class U>
  inline Point<U> Point<U>::operator+  (const Point &point) const
  {
    Point<U> result;

    result.mData[0] = mData[0] + point.mData[0];
    result.mData[1] = mData[1] + point.mData[1];
    result.mData[2] = mData[2] + point.mData[2];

    return result;
  }

  template<class U>
  inline Point<U>& Point<U>::operator+= (const Point &point)
  {
    mData[0] += point.mData[0];
    mData[1] += point.mData[1];
    mData[2] += point.mData[2];

    return *this;
  }
  
  //----------------------------------------------------------------------------
  //! surcharge operateur * avec T
  template<class U>
  inline Point<U> Point<U>::operator* (const U &val) const
  {
    Point<U> point;
    
    point.mData[0] = mData[0] * val;
    point.mData[1] = mData[1] * val;
    point.mData[2] = mData[2] * val;
    
    return point;
  }
  
  //----------------------------------------------------------------------------
  template<class U>
  inline Point<U>& Point<U>::operator*= (const U &val)
  {
    mData[0] *= val;
    mData[1] *= val;
    mData[2] *= val;
    return *this;
  }
  
  //----------------------------------------------------------------------------
  //! surcharge operateur / avec T
  template<class U>  
  inline Point<U> Point<U>::operator/ (const U &val) const
  {
    double vTmp;
    Point<U> p(*this);
    
    if(val>EPSILON || val<-EPSILON)
      vTmp=((double)1.0)/(double)val;
    else
      vTmp=(double)0.0;
    
    p *= vTmp;
//verifier si le *= fonctionne correctement
//p.x_*=vTmp;
//p.y_*=vTmp;
//p.z_*=vTmp;
    
    return p;
  }
  
  //! surcharge operateur /= avec T
  template<class U>
  inline Point<U>& Point<U>::operator/= (const U &val)
  {
    double vTmp;
    
    if(val>EPSILON || val<-EPSILON)
      vTmp=((double)1.0)/(double)val;
    else
      vTmp=(double)0.0;
    
		return (*this *= vTmp);    
//    x_*=vTmp;
//    y_*=vTmp;
//    z_*=vTmp;
//    
//    return *this;
  }

  //----------------------------------------------------------------------------
  template<class U>
  inline Point<U> Point<U>::operator- (const U &val) const
  {
    Point<U> result;

    result.mData[0] = mData[0] - val;
    result.mData[1] = mData[1] - val;
    result.mData[2] = mData[2] - val;

    return result;
  }

  template<class U>
  inline Point<U>& Point<U>::operator-= (const Point &point)
  {
    mData[0] -= point.mData[0];
    mData[1] -= point.mData[1];
    mData[2] -= point.mData[2];
    
    return *this;
  }
  
  typedef Point<float>        Point3f;
  typedef Point<double>       Point3d;
  typedef Point<int>          Point3i;
} // math
} // end of namespace realisim
#endif //realisim_math_POINT_H
