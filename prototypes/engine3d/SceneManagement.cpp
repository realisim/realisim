/**/

#include <Broker.h>
#include <data/Scene.h>
#include <SceneManagement.h>


using namespace engine3d;
  using namespace core;

//---------------------------
SceneManagement::SceneManagement() : Core::Client()
{}

//---------------------------
SceneManagement::~SceneManagement()
{}

//---------------------------
void SceneManagement::setScene(data::Scene *ipS)
{ mScene = ipS; }

