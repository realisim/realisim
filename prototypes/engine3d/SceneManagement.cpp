/**/

#include <Broker.h>
#include <data/Scene.h>
#include <SceneManagement.h>


using namespace engine3d;
  using namespace core;

//---------------------------
SceneManagement::SceneManagement()
{}

//---------------------------
SceneManagement::~SceneManagement()
{}

//---------------------------
bool SceneManagement::requestStateChange(Core::state from, Core::state to)
{
  return true;
}

//---------------------------
void SceneManagement::setScene(data::Scene *ipS)
{ mScene = ipS; }

