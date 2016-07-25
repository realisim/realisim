/**/

#include <Broker.h>
#include "Hub.h"
#include <render/Renderer.h>
#include <SceneManagement.h>

using namespace engine3d;
  using namespace core;

//---------------------------
Hub::Hub() :
  mpBroker(nullptr),
  mpSceneManagement(nullptr)
{}

//---------------------------
Hub::~Hub()
{}

//---------------------------
void Hub::initialize(Broker* b, SceneManagement* sm,
                     render::Renderer* r)
{
  mpBroker = b;
  mpSceneManagement = sm;
  mpRenderer = r;
  
  //give data to modifying interface.
  mpSceneManagement->setScene( &(mpBroker->mScene) );
}

