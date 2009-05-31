//------------------------------------------------------------------------------
//!
//! \file
//! \brief Classe \c LineSegment permet de reprÈsenter une ligne dans l'espace 
//! 3D.
//!
//! AUTHOR:  Pierre-Olivier Beaudoin & David Pinson & Jean Francois Charron
//!
//------------------------------------------------------------------------------

#ifndef LINESEGMENT_H
#define LINESEGMENT_H

#include "Point.h"

//------------------------------------------------------------------------------
//!
//! \brief Classe gÈrant les fonctionalitÈs associÈes a un segment de ligne
//!
//! Il s'agit d'une classe segment ligne tout ce qu'il a de plus simple. 
//! Recherche d'une certaine efficacitÈ a l'exÈcution.
//!
//------------------------------------------------------------------------------
namespace Realisim
{
  class LineSegment
  {
  public:
  
    // --------------- constructeurs -------------------------------------------
    inline LineSegment();
    inline LineSegment(const Point3d& point1, const Point3d& point2);
    inline LineSegment(const LineSegment &lineSegment);
    
    // --------------- destructeurs --------------------------------------------
    inline ~LineSegment();

    // --------------- Fonction Get --------------------------------------------
    inline const Point3d& getPoint1() const;    
    inline const Point3d& getPoint2() const;    
    // --------------- Fonction Set --------------------------------------------
    inline void setLine(const Point3d& point1, const Point3d& point2);
    // --------------- Overload: operateurs ------------------------------------
    inline bool operator== (const LineSegment &lineSegment) const;
    inline LineSegment&  operator=  (const LineSegment &lineSegment);
  
  protected:
  private:
  
    Point3d point1_;    //! pointeur sur point1 du segemnt
    Point3d point2_;    //! pointeur sur point2 du segment
  
  };
  
  //----------------------------------------------------------------------------
  //!
  //! \brief  Constructeur par defaut
  //!
  //!
  //----------------------------------------------------------------------------
  inline LineSegment::LineSegment():point1_(0), point2_(0)
  {
  }
  
  //----------------------------------------------------------------------------
  //!
  //! \brief  Constructeur avec parametre.
  //!
  //! Construit un objet de type \c LineSegement 
  //! 
  //! \param valeurs d'initialisation des points 1 et 2 qui constituent le 
  //!        segment de ligne.
  //!
  //----------------------------------------------------------------------------
  inline LineSegment::LineSegment(const Point3d& point1, const Point3d& point2)
    : point1_(point1), point2_(point2)
  {
  }
  
  //----------------------------------------------------------------------------
  //!
  //! \brief  Constructeur copie
  //!
  //! Copie un objet de type \c LineSegement 
  //! 
  //! \param la ligne dont on dÈsire une copie
  //!
  //----------------------------------------------------------------------------
  inline LineSegment::LineSegment(const LineSegment &lineSegment)
  {
    point1_ = lineSegment.getPoint1();
    point2_ = lineSegment.getPoint2();
  }
  
  //----------------------------------------------------------------------------
  //!
  //! \brief  Destructeur
  //!
  //----------------------------------------------------------------------------
  inline LineSegment::~LineSegment()
  {
    ;
  }

  //----------------------------------------------------------------------------
  //! \brief  Retourne le point 1 du segment
  //----------------------------------------------------------------------------
   inline const Point3d& LineSegment::getPoint1() const
   {
     return point1_;
   }  

  //----------------------------------------------------------------------------
  //! \brief  Retourne le point 2 du segment
  //----------------------------------------------------------------------------
   inline const Point3d& LineSegment::getPoint2() const
   {
     return point2_;
   }  
  
  //----------------------------------------------------------------------------
  //!
  //! \brief  Assigne les 2 points de la ligne
  //! 
  //! \param valeurs d'initialisation des points 1 et 2 qui constituent le 
  //!        segment de ligne.
  //!
  //----------------------------------------------------------------------------
 inline void LineSegment::setLine(const Point3d& point1, const Point3d& point2)
  {
    point1_ = point1;
    point2_ = point2;
  }

  //! surcharge opÈrateur = LineSegment
  inline LineSegment& LineSegment::operator= (const LineSegment &lineSegment)
  {
    point1_ = lineSegment.getPoint1();
    point2_ = lineSegment.getPoint2();
    return *this;
  }
  
  //! surcharge opÈrateur ==
  inline bool LineSegment::operator== (const LineSegment &lineSegment) 
                                   const
  {
    if( (point1_ == lineSegment.getPoint1()) && 
        (point2_ == lineSegment.getPoint2()) )
      return true;
    else
      return false;
  }
} // end of namespace realisim
#endif // LINESEGMENT_H

