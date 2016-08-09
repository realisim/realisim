/**/

#include "Conversion.h"

using namespace realisim;


namespace engine3d
{
namespace core
{

    //return a lat-long point to a planar point in meters
    math::Point3d latLongToCartesian(const realisim::math::Point2d& iLatLong)
    {
        math::Point3d r;
        r.setZ(dEarthRadius*sin(dDegreeToRadian*iLatLong.x()));
        r.setX(dEarthRadius*cos(dDegreeToRadian*iLatLong.x())*cos(dDegreeToRadian*iLatLong.y()));
        r.setY(dEarthRadius*cos(dDegreeToRadian*iLatLong.x())*sin(dDegreeToRadian*iLatLong.y()));
        return r;
    }
}
}


