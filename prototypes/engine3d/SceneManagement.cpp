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
void SceneManagement::clearScene()
{
	mScene->mTiles.clear();
	//mScene->mTiles.clear();
	//mScene->mTiles.clear();
}

//---------------------------
void SceneManagement::setScene(data::Scene *ipS)
{ mScene = ipS; }

//---------------------------
void SceneManagement::update(double iSecElapsed)
{
	clearScene();

	const Broker& b = getHub().getBroker();
	data::Tile t =  b.getTile(40.3, -76.2);

	mScene->mTiles.push_back(t);
}
