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

	Broker& b = getHub().getBroker();
	data::Tile t =  b.getTile(40.3, -76.2);
    data::Tile t2 = b.getTile(40.32, -80.0);
    data::Tile t3 = b.getTile(40.32, -79.86);
    data::Tile t4 = b.getTile(40.32, -79.7);

	mScene->mTiles.push_back(t);
    mScene->mTiles.push_back(t2);
    mScene->mTiles.push_back(t3);
    mScene->mTiles.push_back(t4);
}
