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
#include <iomanip>
#include <iostream>
#include <sstream>

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
//------------------------------------------------------------------------------
//--- Point2
//------------------------------------------------------------------------------
  template<class U>
  class Point2
  {
  public:
    Point2();
    explicit Point2(const U &val);
    Point2(const U &x, const U &y);
    template <class T> Point2(const Point2<T> &);
    virtual ~Point2();

    virtual const U& x() const;
    virtual const U& y() const;
    virtual const U* getPtr() const;
    virtual void set(const Point2&);
    virtual void set(const U&);
    virtual void set(const U&, const U&);
    virtual void setX(const U&);
    virtual void setY(const U&);

    // --------------- fonction utiles -----------------------------------------
    virtual double dist(const Point2&) const;
    virtual double distSqr(const Point2&) const;
    virtual Point2& translate( const U&, const U& );
    virtual void minCoord (const Point2&);
    virtual void maxCoord (const Point2&);

    // --------------- Overload: operateurs ------------------------------------
    virtual bool operator== (const Point2&) const;
    virtual Point2& operator=(const Point2&);
    //virtual Vector2 operator-(const Point2&) const;
    virtual bool operator< (const Point2&) const;
    virtual bool operator<= (const Point2&) const;
    virtual bool operator> (const Point2&) const;
    virtual bool operator>= (const Point2&) const;

    std::string toString(int iPrecision = 3) const;

  protected:
  private:

    U mData[2];
  };

  //! constructeur par defaut.
  template<class U>
  Point2<U>::Point2()
  { memset( (void*)mData, 0, 2*sizeof(U) ); }

  //---------------------------------------------------------------------------
  template<class U>
  Point2<U>::Point2(const U &val)
  { mData[0] = mData[1] = val; }

  //---------------------------------------------------------------------------
  template<class U>
  Point2<U>::Point2(const U &x, const U &y)
  { set(x, y); }

  //! constructeur copie qui permet les conversion implicite légale
  // int a double par exemple.
  template<class U>
  template <class T>
  Point2<U>::Point2(const Point2<T> &point)
  {
    mData[0] = (U)point.x();
    mData[1] = (U)point.y();
  }

  //! destructeur
  template<class U>
  Point2<U>::~Point2()
  {}

  //!---------------------------------------------------------------------------
  template<class U>
  void Point2<U>::set(const Point2 &point)
  {
    memcpy((void*)mData, (const void*)point.mData, 2*sizeof(U));
  }

  //!---------------------------------------------------------------------------
  template<class U>
  void Point2<U>::set(const U &val)
  { mData[0] = mData[1] = val; }
  
  //!---------------------------------------------------------------------------
  template<class U>
  void Point2<U>::set(const U &x, const U &y)
  { mData[0] = x; mData[1] = y; }

  //!---------------------------------------------------------------------------
  template<class U>
  void Point2<U>::setX(const U &x)
  { mData[0] = x; }

  //!---------------------------------------------------------------------------
  template<class U>
  void Point2<U>::setY(const U &y)
  { mData[1] = y; }

  //!---------------------------------------------------------------------------
  template<class U>
  const U& Point2<U>::x() const
  { return mData[0]; }

  //!---------------------------------------------------------------------------
  template<class U>
  const U& Point2<U>::y() const
  { return mData[1]; }
  
  //!---------------------------------------------------------------------------
  //retourne le pointeur sur le début du tableau de coordonnées.
  template<class U>
  inline const U* Point2<U>::getPtr() const
  { return &mData[0]; }

  //!---------------------------------------------------------------------------
  //! \brief  Calcule et retourne la distance entre deux points.
  //!
  //! \param &point le point avec qui calculer la distance
  //!
  //! \return la distance entre les deux points.
  //!---------------------------------------------------------------------------
  template<class U>
  double Point2<U>::dist(const Point2& p) const
  { return sqrt( distSqr(p) ); }

  //!---------------------------------------------------------------------------
  //! \brief  Calcule et retourne la distance au carre entre deux points.
  //!
  //! \param &point le point avec qui calculer la distance au carre
  //!
  //! \return la distance au carre entre les deux points.
  //!---------------------------------------------------------------------------
  template<class U>
  double Point2<U>::distSqr(const Point2& p) const
  {
    double x = p.mData[0] - mData[0];
    double y = p.mData[1] - mData[1];
    return (x*x + y*y);
  }

  //----------------------------------------------------------------------------
  template<class U>
  Point2<U>& Point2<U>::translate (const U& ix, const U& iy )
  {
    setX( x() + ix );
    setY( y() + iy );
    return *this;
  }
  
  //----------------------------------------------------------------------------
  template<class U>
  void Point2<U>::minCoord (const Point2<U>& iP)
  {
    setX(std::min (x(), iP.x() ));
    setY(std::min (y(), iP.y() )); }
  
  //----------------------------------------------------------------------------
  template<class U>
  void Point2<U>::maxCoord (const Point2<U>& iP)
  {
    setX (std::max (x(), iP.x() ) );
    setY (std::max (y(), iP.y() ) );
  }
  
  //----------------------------------------------------------------------------
  template<class U>
  inline std::string Point2<U>::toString(int iPrecision /*=3*/) const
  { 
      std::stringstream iss;
      iss << std::fixed << std::setprecision(iPrecision);
      iss << "(" << x() << ", " << y() << ")";
      return iss.str();
  }

  //! surcharge operateur = Point
  template<class U>
  Point2<U>& Point2<U>::operator= (const Point2& p)
  {
    memcpy((void*)mData, (const void*)p.mData, 2*sizeof(U));
    return *this;
  }
	//----------------------------------------------------------------------------
  //strictement egale, pour une comparaison un peu plus permissive, voir
  //mathUtil isEqual( const Vect<T> &, const Vect<T> &, double )
  template<class U>
  bool Point2<U>::operator== (const Point2 &point) const
  {
    return x() == point.x() &&
    	y() == point.y();
  }
  
  //----------------------------------------------------------------------------
  template<class U>
  bool Point2<U>::operator< (const Point2<U>& iP) const
  {
  	U epsilon = std::numeric_limits<U>::epsilon();
  	if(x() < iP.x() - epsilon) return true;
    if(x() > iP.x() + epsilon) return false;
    
    if(y() < iP.y() - epsilon) return true;
    if(y() > iP.y() + epsilon) return false;
    return false;
  }

  //----------------------------------------------------------------------------
  template<class U>
  bool Point2<U>::operator<= (const Point2<U>& iP) const
  {
    U epsilon = std::numeric_limits<U>::epsilon();
    if(x() >= iP.x() - epsilon && x() <= iP.x() + epsilon) return true;
    if(x() < iP.x() - epsilon) return true;
    if(x() > iP.x() + epsilon) return false;
    
    if(y() >= iP.y() - epsilon && y() <= iP.y() + epsilon) return true;
    if(y() < iP.y() - epsilon) return true;
    if(y() > iP.y() + epsilon) return false;
    return false;
  }

  //----------------------------------------------------------------------------
  template<class U>
  bool Point2<U>::operator> (const Point2<U>& iP) const
  {
    //Voir operateur < et corriger ici au besoin
    return x() > iP.x() && 
      y() > iP.y();
  }
  
  //----------------------------------------------------------------------------
  template<class U>
  bool Point2<U>::operator>= (const Point2<U>& iP) const
  {
    //Voir operateur <= et corriger ici au besoin
    return x() >= iP.x() && 
      y() >= iP.y();
  }
  
//------------------------------------------------------------------------------
//--- Point3
//------------------------------------------------------------------------------
  template<class U>
  class Point3
  {
  public:

    // --------------- constructeurs -------------------------------------------
    inline Point3();
    inline explicit Point3(const U &val);
    inline Point3(const U &x, const U &y, const U &z);
    template <class T>
    inline Point3(const Point3<T> &Point3);

    // --------------- destructeurs --------------------------------------------
    inline ~Point3();

    // --------------- fonction set --------------------------------------------
    inline void set(const Point3&);
    inline void set(const U &val);
    inline void get(const U &val);
    inline void setX(const U &x);
    inline void setY(const U &y);
    inline void setZ(const U &z);
    inline void setXYZ(const U &x, const U &y, const U &z);

    // --------------- fonction get --------------------------------------------
    inline const U& x() const;
    inline const U& y() const;
    inline const U& z() const;
    inline void get(Point3 &Point3) const;
    inline void xYZ(U &x, U &y, U &z) const;
    inline const U* getPtr() const;

    // --------------- fonction utiles -----------------------------------------
    inline double dist(const Point3&) const;
    inline U distSqr(const Point3 &) const;
    inline double fastDist(const Point3&) const;
    inline void minCoord (const Point3<U>&);
    inline void maxCoord (const Point3<U>&);

    // --------------- Overload: operateurs ------------------------------------
    inline bool operator== (const Point3&) const;
    inline Point3& operator=  (const Point3&);

  // FAIT DU SENS???
  	inline Point3<U> operator- () const;
    inline Point3<U>& operator-= (const Point3 &Point3);
    inline Point3<U>& operator+= (const Point3 &Point3);
    inline Point3<U> operator*  (const U &val) const;
    inline Point3<U>& operator*= (const U &val);
    inline Point3<U>  operator/  (const U &val) const;
    inline Point3<U>& operator/= (const U &val);  
  //   inline Point3&        operator*= (const Point3 &Point3);
  //
  //   inline Point3 operator* (const Point3 &Point3) const;
  //   inline Point3 operator+ (const Point3 &Point3) const;
    inline Point3<U> operator- (const U &val) const;
    inline bool operator< (const Point3<U>&) const;
    inline bool operator<= (const Point3<U>&) const;
    inline bool operator> (const Point3<U>&) const;
    inline bool operator>= (const Point3<U>&) const;

    std::string toString(int iPrecision = 3) const;
  protected:
  private:

    U mData[3];
  };

  //! constructeur par defaut.
  template<class U>
  inline Point3<U>::Point3()
  { memset( (void*)mData, 0, 3*sizeof(U) ); }

  //---------------------------------------------------------------------------
  template<class U>
  inline Point3<U>::Point3(const U &val)
  { mData[0] = mData[1] = mData[2] = val; }

  //---------------------------------------------------------------------------
  template<class U>
  inline Point3<U>::Point3(const U &x, const U &y, const U &z)
  { setXYZ(x, y, z); }

  //! constructeur copie qui permet les conversion implicite légale
  // int a double par exemple.
  template<class U>
  template <class T>
  inline Point3<U>::Point3(const Point3<T> &point)
  {
    mData[0] = (U)point.x();
    mData[1] = (U)point.y();
    mData[2] = (U)point.z();
  }

  //! destructeur
  template<class U>
  inline Point3<U>::~Point3()
  {}

  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur du point.
  //!
  //! égale le point a celui passe en parametre.
  //!
  //! \param &point
  //!---------------------------------------------------------------------------
  template<class U>
  inline void Point3<U>::set(const Point3 &point)
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
  inline void Point3<U>::set(const U &val)
  { mData[0] = mData[1] = mData[2] = val; }

  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur x_ du point.
  //!
  //! \param &x
  //!---------------------------------------------------------------------------
  template<class U>
  inline void Point3<U>::setX(const U &x)
  { mData[0] = x; }

  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur y_ du point.
  //!
  //! \param &y
  //!---------------------------------------------------------------------------
  template<class U>
  inline void Point3<U>::setY(const U &y)
  { mData[1] = y; }

  //!---------------------------------------------------------------------------
  //! \brief  Ajuste la valeur z_ du point.
  //!
  //! \param &z
  //!---------------------------------------------------------------------------
  template<class U>
  inline void Point3<U>::setZ(const U &z)
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
  inline void Point3<U>::setXYZ(const U &x, const U &y, const U &z)
  { mData[0] = x; mData[1] = y; mData[2] = z; }

  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir le point.
  //!
  //! Permet d'obtenir la valeur du point dans celui passe en parametre.
  //!
  //! \param &point Le point qui va etre comme le point que l'on veut
  //!---------------------------------------------------------------------------
//  template<class U>
//  inline void Point3<U>::get(Point3 &point) const
//  { memcpy((void*)point.mData, (const void*)mData, 3*sizeof(U)); }

  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir la valeur x du point.
  //!
  //! \return la valeur X du point
  //!---------------------------------------------------------------------------
  template<class U>
  inline const U& Point3<U>::x() const
  { return mData[0]; }

  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir la valeur y du point.
  //!
  //! \return la valeur Y du point
  //!---------------------------------------------------------------------------
  template<class U>
  inline const U& Point3<U>::y() const
  { return mData[1]; }

  //!---------------------------------------------------------------------------
  //! \brief  Permet d'obtenir la valeur z du point.
  //!
  //! \return la valeur Z du point
  //!---------------------------------------------------------------------------
  template<class U>
  inline const U& Point3<U>::z() const
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
  inline void Point3<U>::xYZ(U &x, U &y, U &z) const
  {
    x=mData[0];
    y=mData[1];
    z=mData[2];
  }
  
  //retourne le pointeur sur le début du tableau de coordonnées.
  template<class U>
  inline const U* Point3<U>::getPtr() const
  { return &mData[0]; }

  //!---------------------------------------------------------------------------
  //! \brief  Calcule et retourne la distance entre deux points.
  //!
  //! \param &point le point avec qui calculer la distance
  //!
  //! \return la distance entre les deux points.
  //!---------------------------------------------------------------------------
  template<class U>
  inline double Point3<U>::dist(const Point3 &point) const
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
  inline U Point3<U>::distSqr(const Point3 &point) const
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
  inline double Point3<U>::fastDist(const Point3 &point) const
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
  inline void Point3<U>::minCoord (const Point3<U>& iP)
  {
    setX (std::min (x (), iP.x ()));
    setY (std::min (y (), iP.y ()));
    setZ (std::min (z (), iP.z ()));
  }
  
  //----------------------------------------------------------------------------
  template<class U>
  inline void Point3<U>::maxCoord (const Point3<U>& iP)
  {
    setX (std::max (x (), iP.x ()));
    setY (std::max (y (), iP.y ()));
    setZ (std::max (z (), iP.z ()));
  }
  
  //----------------------------------------------------------------------------
  template<class U>
  inline std::string Point3<U>::toString(int iPrecision /*=3*/) const
  { 
      std::stringstream iss;
      iss << std::fixed << std::setprecision(iPrecision);
      iss << "(" << x() << ", " << y() << ", " << z() << ")";
      return iss.str();
  }

  //! surcharge operateur = Point
  template<class U>
  inline Point3<U>& Point3<U>::operator= (const Point3 &point)
  {
    memcpy((void*)mData, (const void*)point.mData, 3*sizeof(U));
    return *this;
  }
	//----------------------------------------------------------------------------
  //strictement egale, pour une comparaison un peu plus permissive, voir
  //mathUtil isEqual( const Vect<T> &, const Vect<T> &, double )
  template<class U>
  inline bool Point3<U>::operator== (const Point3 &point) const
  {
    return x() == point.x() &&
    	y() == point.y() &&
      z() == point.z();
  }
  
  //----------------------------------------------------------------------------
  template<class U>
  inline bool Point3<U>::operator< (const Point3<U>& iP) const
  {
  	U epsilon = std::numeric_limits<U>::epsilon();
  	if(x() < iP.x() - epsilon) return true;
    if(x() > iP.x() + epsilon) return false;
    
    if(y() < iP.y() - epsilon) return true;
    if(y() > iP.y() + epsilon) return false;
    
    if(z() < iP.z() - epsilon) return true;
    return false;
    
/*Ceci est le code preexistant au 9 septembre 2011... Ce code
  semble tout aussi arbitraire que celui que je viens d'insérer
  à l'exception que le code préexistant ne permettait pas 
  d'insérer les points dans un container ordonné (map, set etc...)*/    
//return x() < iP.x() && 
//      y() < iP.y() &&
//      z() < iP.z();
  }

  //----------------------------------------------------------------------------
  template<class U>
  inline bool Point3<U>::operator<= (const Point3<U>& iP) const
  {
    U epsilon = std::numeric_limits<U>::epsilon();
    if(x() >= iP.x() - epsilon && x() <= iP.x() + epsilon) return true;
    if(x() < iP.x() - epsilon) return true;
    if(x() > iP.x() + epsilon) return false;
    
    if(y() >= iP.y() - epsilon && y() <= iP.y() + epsilon) return true;
    if(y() < iP.y() - epsilon) return true;
    if(y() > iP.y() + epsilon) return false;
    
    if(z() >= iP.z() - epsilon && z() <= iP.z() + epsilon) return true;
    if(z() < iP.z() - epsilon) return true;
    return false;
/*Ceci est le code preexistant au 9 septembre 2011... Ce code
  semble tout aussi arbitraire que celui que je viens d'insérer
  à l'exception que le code préexistant ne permettait pas 
  d'insérer les points dans un container ordonné (map, set etc...)*/    
//return x() <= iP.x() && 
//  y() <= iP.y() &&
//  z() <= iP.z();
  }

  //----------------------------------------------------------------------------
  template<class U>
  inline bool Point3<U>::operator> (const Point3<U>& iP) const
  {
    //Voir operateur < et corriger ici au besoin
    return x() > iP.x() && 
      y() > iP.y() &&
      z() > iP.z();
  }
  
  //----------------------------------------------------------------------------
  template<class U>
  inline bool Point3<U>::operator>= (const Point3<U>& iP) const
  {
    //Voir operateur <= et corriger ici au besoin
    return x() >= iP.x() && 
      y() >= iP.y() &&
      z() >= iP.z();
  }

  template<class U>
  inline Point3<U>& Point3<U>::operator+= (const Point3 &point)
  {
    mData[0] += point.mData[0];
    mData[1] += point.mData[1];
    mData[2] += point.mData[2];

    return *this;
  }
  
  //----------------------------------------------------------------------------
  //! surcharge operateur * avec T
  template<class U>
  inline Point3<U> Point3<U>::operator* (const U &val) const
  {
    Point3<U> point;
    
    point.mData[0] = mData[0] * val;
    point.mData[1] = mData[1] * val;
    point.mData[2] = mData[2] * val;
    
    return point;
  }
  
  //----------------------------------------------------------------------------
  template<class U>
  inline Point3<U>& Point3<U>::operator*= (const U &val)
  {
    mData[0] *= val;
    mData[1] *= val;
    mData[2] *= val;
    return *this;
  }
  
  //----------------------------------------------------------------------------
  //! surcharge operateur / avec T
  template<class U>  
  inline Point3<U> Point3<U>::operator/ (const U &val) const
  {
    double vTmp;
    Point3<U> p(*this);
    
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
  inline Point3<U>& Point3<U>::operator/= (const U &val)
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
  /*permet de mettre un - devant un Point3d  */
  template<class T>
  inline Point3<T> Point3<T>::operator- () const
  { 
    Point3<T> r;       
    r.mData[0] = -mData[0];
    r.mData[1] = -mData[1];
    r.mData[2] = -mData[2];
    return r;
  }
  
  //----------------------------------------------------------------------------
  template<class U>
  inline Point3<U> Point3<U>::operator- (const U &val) const
  {
    Point3<U> result;

    result.mData[0] = mData[0] - val;
    result.mData[1] = mData[1] - val;
    result.mData[2] = mData[2] - val;

    return result;
  }

  template<class U>
  inline Point3<U>& Point3<U>::operator-= (const Point3 &point)
  {
    mData[0] -= point.mData[0];
    mData[1] -= point.mData[1];
    mData[2] -= point.mData[2];
    
    return *this;
  }
  
  typedef Point3<float>        Point3f;
  typedef Point3<double>       Point3d;
  typedef Point3<int>          Point3i;
  typedef Point2<float>        Point2f;
  typedef Point2<double>        Point2d;
  typedef Point2<int>        Point2i;
} // math
} // end of namespace realisim
#endif //realisim_math_POINT_H
