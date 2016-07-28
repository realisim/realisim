
#include "Light.h"

using namespace engine3d;
  using namespace data;

//-------------------------------------------------------------------
Light::Light() :
mType(tDirectional),
mColor(Qt::white),
mTransform()
{}

//-------------------------------------------------------------------
Light::~Light()
{}