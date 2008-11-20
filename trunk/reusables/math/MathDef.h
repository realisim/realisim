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
  static const float SMALL_REAL          = 1.0e-4;
  static const float HUGE_REAL           = 1.0e6;
  static const unsigned int HUGE_INT = 0xffffffff;

  #ifndef M_PI
  #define M_PI
  static const float M_PI        = 3.141592654;
  #endif

  static const float PI_SUR_180  = M_PI/180.0;
  static const float PI_SUR_360  = M_PI/360.0;
  static const float UN_SUR_PI   = 1.0/M_PI;
  static const float UN_SUR_2_PI = 0.5/M_PI;
  static const float DEUX_PI     = 2.0*M_PI;

}

#endif //Realisim_Math_Def_h
