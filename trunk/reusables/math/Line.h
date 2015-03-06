//------------------------------------------------------------------------------
//!
//! \file
//! \brief Classe \c Line permet de reprÈsenter une ligne dans l'espace 3D.
//!
//! AUTHOR:  Pierre-Olivier Beaudoin & David Pinson
//!
//------------------------------------------------------------------------------

#ifndef LINE_H
#define LINE_H

#include "MathDef.h"
#include "Point.h"
#include "Vect.h"

//------------------------------------------------------------------------------
//!
//! \brief Classe gÈrant les fonctionalitÈs associÈes a une ligne
//!
//! Il s'agit d'une classe ligne tout ce qu'il a de plus simple. Recherche 
//! d'une certaine efficacitÈ a l'exÈcution.
//!
//------------------------------------------------------------------------------
namespace realisim
{
namespace math
{

  class Line
  {
  public:
  
    // --------------- constructeurs -------------------------------------------
    inline Line();
    inline Line(Point3d &origin, Vector3d &direction);
    inline Line(const Line &line);
    
    // --------------- destructeurs --------------------------------------------
    virtual ~Line();
    
    // --------------- Overload: operateurs ------------------------------------
    inline bool  operator== (const Line &line) const;
    inline Line&         operator=  (const Line &line);
  
  protected:
  private:
  
    Point3d  origin_;    //! origin de la ligne
    Vector3d direction_;    //! direction de la ligne
  
  };
  
  //! constructeur par defaut
  inline Line::Line() : origin_(), direction_()
  {
  }
  
  //----------------------------------------------------------------------------
  //!
  //! \brief  Constructeur avec parametre.
  //!
  //! Construit un objet de type \c Line 
  //! 
  //! \param val valeur d'initialisation
  //!
  //----------------------------------------------------------------------------
  inline Line::Line(Point3d &origin, Vector3d &direction)
    : origin_(origin), direction_(direction)
  {
  }
  
  //! constructeur copie
  inline Line::Line(const Line &line)
  {
    origin_   = line.origin_;
    direction_ = line.direction_;
  }
  
  //! destructeur
  Line::~Line()
  {
    ;
  }
  
  //! surcharge opÈrateur = Line
  inline Line& Line::operator= (const Line &line)
  {
    origin_   = line.origin_;
    direction_ = line.direction_;
    return *this;
  }
  
  //! surcharge opÈrateur ==
  inline bool Line::operator== (const Line &line) const
  {
    if( (origin_ == line.origin_) && (direction_ == line.direction_) )
      return true;
    else
      return false;
  }
  
} //math
} // end of namespace realisim
#endif // LINE_H
