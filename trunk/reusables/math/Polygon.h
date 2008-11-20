//!-----------------------------------------------------------------------------
//! \file
//! \brief Classe \c Polygon permet de reprÈsenter un polygone dans l'espace 3D.
//!
//! AUTHOR:  Pierre-Olivier Beaudoin
//!          David Pinson
//!-----------------------------------------------------------------------------
#ifndef POLYGON_H
#define POLYGON_H

#include "Point.h"
#include "Vect.h"
#include "LineSegment.h"

#include <vector>

//!-----------------------------------------------------------------------------
//! \brief Classe gÈrant les fonctionalitÈs associÈes a un polygon
//!
//! Il s'agit d'une classe polygon tout ce qu'il a de plus simple. Recherche 
//! d'une certaine efficacitÈ a l'exÈcution. La classe prend 3 points afin de 
//! creer des Polygones triangulaires. Le fait de limiter les polygones a 3
//! points assure que ceux-ci sont coplanaire. Egalement faire attention si 
//!cette classeest utilisÈ avec OpenGL, bien se rapeller qu'OpenGL n'affiche que
//! des polygones concaves (ou convexes, me souviens plus... TODO vÈrifiÈ ca)
//!-----------------------------------------------------------------------------
namespace Realisim
{
  class Polygon
  {
  public:
  
    // --------------- constructeurs -------------------------------------------
    inline Polygon();
    inline Polygon(const Point3f& p1, const Point3f& p2, const Point3f& p3);
    inline Polygon(const Polygon& polygon);
    
    // --------------- destructeurs --------------------------------------------
    inline ~Polygon();

    // --------------- fonction Get --------------------------------------------
    inline const Point3f& getPoint1() const;
    inline const Point3f& getPoint2() const;
    inline const Point3f& getPoint3() const;

    inline const Vector3f& getNormal() const;
    
    // --------------- fonction Set --------------------------------------------
    inline void setPolygon(const Point3f& p1, const Point3f& p2, const Point3f& p3);
    inline void setNormal(const Vector3f& vect);
    
    // --------------- fonction utiles -----------------------------------------
    inline void invertNormal();  //inverse la normale
    inline void calculateNormal(); //! fonction qui calcule la normale
    
    // --------------- Overload: operateurs ------------------------------------
    inline bool operator== (const Polygon &polygon) const;
    inline Polygon&      operator=  (const Polygon &polygon);
  
  protected:
  private:
  
    //! il est Ègalement constituÈ de 3 points
    Point3f point1_;
    Point3f point2_;
    Point3f point3_;

    Vector3f normal_; //! normal du Polygon
  };
  
  //----------------------------------------------------------------------------
  //!
  //! \brief  Constructeur par defaut
  //!
  //! Construit un objet de type \c Polygon. Un triangle dans le plan y=0.
  //!
  //----------------------------------------------------------------------------
  inline Polygon::Polygon() : point1_(0), point2_(0), point3_(0), normal_()
  {
  }
  
  //----------------------------------------------------------------------------
  //!
  //! \brief  Constructeur avec parametre.
  //!
  //! Construit un objet de type \c Polygon. La normale est calculÈe en fonction
  //! des points constituant le polygone. On considere que tous les points 
  //! fournis sont coplanaires.
  //! 
  //! \param listPoint valeur d'initialisation
  //!
  //----------------------------------------------------------------------------
  inline Polygon::Polygon(const Point3f& p1, const Point3f& p2, const Point3f& p3)
  {
    point1_ = p1;
    point2_ = p2;
    point3_ = p3;
    calculateNormal();
  }

  //! Constructeur Copie
  inline Polygon::Polygon(const Polygon &polygon)
  {
    point1_ = polygon.getPoint1();
    point2_ = polygon.getPoint2();
    point3_ = polygon.getPoint3();
    normal_ = polygon.getNormal();
  }
  
  //! destructeur
  inline Polygon::~Polygon()
  {
  }

  //! retourne un Pointeur sur un Point
  inline const Point3f& Polygon::getPoint1() const
  {
    return point1_;
  }

  //! retourne un Pointeur sur un Point
  inline const Point3f& Polygon::getPoint2() const
  {
    return point2_;
  }

  //! retourne un Pointeur sur un Point
  inline const Point3f& Polygon::getPoint3() const
  {
    return point3_;
  }

  //! retourne la normal
  inline const Vector3f& Polygon::getNormal() const
  {
    return normal_;
  }

  //! Set le polygone avec 3 Point3f
  inline void Polygon::setPolygon(const Point3f& p1, const Point3f& p2, const Point3f& p3)
  {
    point1_ = p1;
    point2_ = p2;
    point3_ = p3;
    //! TODO peut etre pas necessaire puisqu'a la construction on assigne la 
    //! normal
    calculateNormal();
  }
  
  inline void Polygon::setNormal(const Vector3f& vect)
  {
    normal_ = vect;
  }

  //! inverse la normale
    //! Permet d'inverser la normal d'un polygon
  inline void Polygon::invertNormal()
  {
    //Afin d'inverser la normale d'un polygon, il suffit d'inverser l'ordre
    //des points constituants le polygone. Ainsi, la methode qui calcule la
    //normale calculera la nouvelle normale dans un sens oppose a la normale
    //actuelle
    const Point3f& pointTemp = getPoint1();
    point1_ = getPoint3();
    point3_ = pointTemp;
    
  }
  
  //! surcharge opÈrateur = Polygon
  inline Polygon& Polygon::operator= (const Polygon &polygon)
  {
    point1_ = polygon.getPoint1();
    point2_ = polygon.getPoint2();
    point3_ = polygon.getPoint3();
    normal_ = polygon.getNormal();
    return *this;
  }
  
  //----------------------------------------------------------------------------
  //!
  //! \brief  surcharge opÈrateur ==
  //!
  //! VÈrifie l'ÈgalitÈ entre deux polygon. 
  //! \param polygon valeur d'initialisation
  //!
  //----------------------------------------------------------------------------
  inline bool Polygon::operator== (const Polygon &polygon) const
  {
    if ((point1_ == polygon.getPoint1()) && (point2_ == polygon.getPoint2()) &&
         (point3_ == polygon.getPoint3()))
    {
      return true;
    }
    else
    {
      return false;
    }

  }
  
  //! fonction qui calcule la normale au plan dans lequel se trouve le polygone
  inline void Polygon::calculateNormal()
  {
    // on calcule la normale. produit vectoriel entre deux vecteurs (3 points)
    // constituant le polygone.
    Vector3f tmp1(getPoint1(), getPoint2());
    Vector3f tmp2(getPoint1(), getPoint3());
    normal_ = tmp1^tmp2;
    normal_.normalise();
  }
} // end of namespace realisim
#endif // POLYGON_H
