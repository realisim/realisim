//------------------------------------------------------------------------------
//!
//! \file
//! \brief Fichier de définition des types mathématique
//!
//! AUTHOR:  Pierre-Olivier Beaudoin & David Pinson
//!
//------------------------------------------------------------------------------

#ifndef Realisim_Math_Def_h
#define Realisim_Math_Def_h

#include <cmath>

namespace Realisim
{

  //! constante utile
  static const double SMALL_REAL          = 1.0e-4;
  static const double HUGE_REAL           = 1.0e6;
  static const unsigned int HUGE_INT = 0xffffffff;

  static const double PI = 3.141592654;

  static const double PI_SUR_180  = PI/180.0;
  static const double PI_SUR_360  = PI/360.0;
  static const double UN_SUR_PI   = 1.0/PI;
  static const double UN_SUR_2_PI = 0.5/PI;
  static const double DEUX_PI     = 2.0*PI;

}

#endif //Realisim_Math_Def_h
