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
}

//---------------------------
void SceneManagement::setScene(data::Scene *ipS)
{ mScene = ipS; }

//---------------------------
void SceneManagement::update(double iSecElapsed)
{
	clearScene();

	Broker& b = getHub().getBroker();
    //std::vector<data::Tile> b.getTiles
	//data::Tile t =  b.getTile(40.3, -76.2);
 //   data::Tile t2 = b.getTile(40.32, -80.0);
 //   data::Tile t3 = b.getTile(40.32, -79.86);
 //   data::Tile t4 = b.getTile(40.32, -79.7);

	//mScene->mTiles.push_back(t);
 //   mScene->mTiles.push_back(t2);
 //   mScene->mTiles.push_back(t3);
 //   mScene->mTiles.push_back(t4);

    for(int i = -40; i < 40; ++i)
        for (int j = -60; j < 60; ++j)
        {
            data::Tile t;
            t.setLatitude(i);
            t.setLongitude(j);
            t.setSize( 1.0, 1.0);
            mScene->mTiles.push_back(t);
        }
}
