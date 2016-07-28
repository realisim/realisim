
#include "Scene.h"

using namespace engine3d;
  using namespace data;

//-------------------------------------------------------------------
Scene::Scene()
{}

//-------------------------------------------------------------------
Scene::~Scene()
{
  mModels.clear();
  mLights.clear();
}
