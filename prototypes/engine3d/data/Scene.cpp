
#include "Scene.h"

using namespace engine3d;
  using namespace data;

//-------------------------------------------------------------------
Scene::Scene()
{}

//-------------------------------------------------------------------
Scene::~Scene()
{ clear(); }

//-------------------------------------------------------------------
void Scene::clear()
{
  mModels.clear();
  mLights.clear();
}