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
    inline U    getX() const;
    inline U    getY() const;
    inline U    getZ() const;
    inline void get(Point &point) const;
    inline void getXYZ(U &x, U &y, U &z) const;

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

    U x_;
    U y_;
    U z_;
  };

  //! constructeur par defaut.
  template<class U>
  inline Point<U>::Point(): x_((U)0.0), y_((U)0.0), z_((U)0.0)
  {
  }

  //!---------------------------------------------------------------------------
  //! \brief  Constructeur avec parametre.
  //!
  //! Construit un objet de type \c Point ou chacune des valeurs cartesiennes
  //! vaut la meme valeur.
  //!
  //! \param val valeur d'initialisation
  //!---------------------------------------------------------------------------
  template<class U>
  inline Point<U>::Point(const U &val) : x_(val), y_(val), z_(val)
  {
  }

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
  inline Point<U>::Point(const U &x, const U &y, const U &z) : x_(x), y_(y),
    z_(z)
  {
  }

  //! constructeur copie qui permet les conversion implicite légale
  // int a double par exemple.
  template<class U>
  template <class T>
  inline Point<U>::Point(const Point<T> &point) :
    x_(point.getX()),
    y_(point.getY()),
    z_(point.getZ())
  {
  }

  //! destructeur
  template<class U>
  inline Point<U>::~Point()
  {
    ;
  }

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
    x_=point.x_;
    y_=point.y_;
    z_=point.z_;
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
  {
    x_=val;
    y_=val;
    z_=val;
  }

  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur x_ du point.
  //!
  //! \param &x
  //!---------------------------------------------------------------------------
  template<class U>
  inline void Point<U>::setX(const U &x)
  {
    x_=x;
  }

  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur y_ du point.
  //!
  //! \param &y
  //!---------------------------------------------------------------------------
  template<class U>
  inline void Point<U>::setY(const U &y)
  {
    y_=y;
  }

  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur z_ du point.
  //!
  //! \param &z
  //!---------------------------------------------------------------------------
  template<class U>
  inline void Point<U>::setZ(const U &z)
  {
    z_=z;
  }

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
  {
    x_=x;
    y_=y;
    z_=z;
  }

  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir le point.
  //!
  //! Permet d'obtenir la valeur du point dans celui passe en parametre.
  //!
  //! \param &point Le point qui va etre comme le point que l'on veut
  //!---------------------------------------------------------------------------
  template<class U>
  inline void Point<U>::get(Point &point) const
  {
    point.x_=x_;
    point.y_=y_;
    point.z_=z_;
  }

  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir la valeur x du point.
  //!
  //! \return la valeur X du point
  //!---------------------------------------------------------------------------
  template<class U>
  inline U Point<U>::getX() const
  {
    return x_;
  }

  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir la valeur y du point.
  //!
  //! \return la valeur Y du point
  //!---------------------------------------------------------------------------
  template<class U>
  inline U Point<U>::getY() const
  {
    return y_;
  }

  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir la valeur z du point.
  //!
  //! \return la valeur Z du point
  //!---------------------------------------------------------------------------
  template<class U>
  inline U Point<U>::getZ() const
  {
    return z_;
  }

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
    x=x_;
    y=y_;
    z=z_;
  }

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
    double x = point.x_-x_;
    double y = point.y_-y_;
    double z = point.z_-z_;

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
    U x = point.x_-x_;
    U y = point.y_-y_;
    U z = point.z_-z_;

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

    max = x_-point.x_;
    med = y_-point.y_;
    min = z_-point.z_;

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
    x_=point.x_;
    y_=point.y_;
    z_=point.z_;
    return *this;
  }

  //! surcharge operateur ==
  template<class U>
  inline bool Point<U>::operator== (const Point &point) const
  {
    U dx = x_ - point.x_;
    U dy = y_ - point.y_;
    U dz = z_ - point.z_;

    if(dx<(U)0.0)
      dx = -dx;
    if(dy<(U)0.0)
      dy = -dy;
    if(dz<(U)0.0)
      dz = -dz;

    if (dx<EPSILON && dy<EPSILON && dz<EPSILON)
      return 1;
    else
      return 0;
  }

  template<class U>
  inline Point<U> Point<U>::operator- (const Point &point) const
  {
    Point<U> result;

    result.x_ = x_ - point.x_;
    result.y_ = y_ - point.y_;
    result.z_ = z_ - point.z_;

    return result;
  }
  
  //----------------------------------------------------------------------------
  template<class U>
  inline bool Point<U>::operator< (const Point<U>& iP) const
  {
    return getX() < iP.getX() && 
      getY() < iP.getY() &&
      getZ() < iP.getZ();
  }

  //----------------------------------------------------------------------------
  template<class U>
  inline bool Point<U>::operator<= (const Point<U>& iP) const
  {
    return getX() <= iP.getX() && 
      getY() <= iP.getY() &&
      getZ() <= iP.getZ();
  }

  //----------------------------------------------------------------------------
  template<class U>
  inline bool Point<U>::operator> (const Point<U>& iP) const
  {
    return getX() > iP.getX() && 
      getY() > iP.getY() &&
      getZ() > iP.getZ();
  }
  
  //----------------------------------------------------------------------------
  template<class U>
  inline bool Point<U>::operator>= (const Point<U>& iP) const
  {
    return getX() >= iP.getX() && 
      getY() >= iP.getY() &&
      getZ() >= iP.getZ();
  }


  template<class U>
  inline Point<U> Point<U>::operator+  (const Point &point) const
  {
    Point<U> result;

    result.x_ = x_ + point.x_;
    result.y_ = y_ + point.y_;
    result.z_ = z_ + point.z_;

    return result;
  }

  template<class U>
  inline Point<U>& Point<U>::operator+= (const Point &point)
  {
    x_ += point.x_;
    y_ += point.y_;
    z_ += point.z_;

    return *this;
  }
  
  //----------------------------------------------------------------------------
  //! surcharge operateur * avec T
  template<class U>
  inline Point<U> Point<U>::operator* (const U &val) const
  {
    Point<U> point;
    
    point.x_=x_*val;
    point.y_=y_*val;
    point.z_=z_*val;
    
    return point;
  }
  
  //----------------------------------------------------------------------------
  template<class U>
  inline Point<U>& Point<U>::operator*= (const U &val)
  {
    x_*=val;
    y_*=val;
    z_*=val;
    return *this;
  }
  
  //----------------------------------------------------------------------------
  //! surcharge operateur / avec T
  template<class U>  
  inline Point<U> Point<U>::operator/ (const U &val) const
  {
    double vTmp;
    Point<U> p(x_, y_, z_);
    
    if(val>EPSILON || val<-EPSILON)
      vTmp=((double)1.0)/(double)val;
    else
      vTmp=(double)0.0;
    
    p.x_*=vTmp;
    p.y_*=vTmp;
    p.z_*=vTmp;
    
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
    
    x_*=vTmp;
    y_*=vTmp;
    z_*=vTmp;
    
    return *this;
  }

  //----------------------------------------------------------------------------
  template<class U>
  inline Point<U> Point<U>::operator- (const U &val) const
  {
    Point<U> result;

    result.x_ = x_ - val;
    result.y_ = y_ - val;
    result.z_ = z_ - val;

    return result;
  }

  template<class U>
  inline Point<U>& Point<U>::operator-= (const Point &point)
  {
    x_ -= point.x_;
    y_ -= point.y_;
    z_ -= point.z_;
    
    return *this;
  }
  
  typedef Point<float>        Point3f;
  typedef Point<double>       Point3d;
  typedef Point<int>          Point3i;
} // math
} // end of namespace realisim
#endif //realisim_math_POINT_H
