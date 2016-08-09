/**/

#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <math/Point.h>
#include <math/Vect.h>

namespace engine3d
{
namespace core
{
    //const double dEarthRadius = 6371000.0; //meters
    const double dEarthRadius = 50.0; //meters
    const double dDegreeToRadian = M_PI /180.0;
    const double dRadianToDegree = 180.0/ M_PI;

    realisim::math::Point3d latLongToCartesian(const realisim::math::Point2d&); //in meters
    //realisim::math::Point2d metersToLatLong(const realisim::math::Point2d&); //in meters
  
}
}