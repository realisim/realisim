//------------------------------------------------------------------------------
//!
//! \file
//! \brief Fichier de dÈfinition des types mathÈmatique
//!
//! AUTHOR:  Pierre-Olivier Beaudoin & David Pinson
//!
//------------------------------------------------------------------------------

#ifndef Realisim_Math_Def_h
#define Realisim_Math_Def_h

#include <cmath>
#include <limits>

namespace realisim
{
namespace math
{

  //! constante utile
  static const double EPSILON          = std::numeric_limits<double>::epsilon();

  static const double PI = 3.141592654;
  static const double PI_SUR_2 = PI / 2.0;
  static const double PI_SUR_4 = PI / 4.0;
  static const double UN_SUR_PI   = 1.0/PI;
  static const double DEUX_PI     = 2.0*PI;
  static const double UN_SUR_2_PI = 0.5/DEUX_PI;

  static const double kDegreeToRadian  = PI/180.0;
  static const double kRadianToDegree = 1.0 / kDegreeToRadian;
  
  static const double MAX_DOUBLE = std::numeric_limits<double>::max();
  static const double MIN_DOUBLE = std::numeric_limits<double>::min();

} //math
} //realisim

#endif //Realisim_Math_Def_h
