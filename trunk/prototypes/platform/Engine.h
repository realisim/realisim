/* */

#ifndef realisim_platform_engine_hh
#define realisim_platform_engine_hh

#include "3d/Camera.h"
#include "Math/intersection.h"
#include <map>
#include <QObject>
#include <QImage>
#include <QTimerEvent>
namespace realisim { namespace platform { class Engine; } }
#include "TileKeeper.h"
#include <vector>

class realisim::platform::Engine : public QObject
{
	Q_OBJECT
public:
	Engine();
	virtual ~Engine();
  
  enum state{ sIdle, sMainMenu, sConfigureMenu, sPlaying, sPaused, sEditing,
  	sQuitting };
  enum mainMenuItem{ mmiStart, mmiConfigure, mmiQuit, mmiCount };
  enum configureMenuItem{ cmiDifficulty, cmiBack, cmiCount };
  enum pauseMenuItem{ pmiBack, pmiEdit, pmiQuit, pmiCount };
  enum event{ eStateChanged, eFrameDone, eQuit };
  
  class Client
  {
  	friend class Engine;
  	public:
    	Client(){}
      virtual ~Client() {}
      
    protected:
      virtual void gotEvent( event ) {};
  };
  
  /*
  	expliquer les valeurs possible du terrain
  */
  class Stage
  {
  	friend class Engine;
  	public:
    	Stage();
      virtual ~Stage() {}
    
    	virtual Vector2i cellSize() const {return mCellSize;}
      virtual Vector2i getCellCoordinate( const Point2d& ) const;
      virtual Vector2i getCellCoordinate( int ) const;
      //virtual std::vector<Vector2i> getCellCoordinates( const Point2d&, const Vector2i& ) const;
      virtual int getCellIndex( const Point2d& ) const;
      virtual std::vector<int> getCellIndices( const Point2d&, const Vector2i& ) const;
      //virtual QByteArray getTerrain( const Point2d&, const Vector2i& ) const;
      virtual Vector2i terrainSize() const {return mTerrainSize;}
    	virtual QByteArray terrain() const { return mTerrain; }
      
    protected:
    	enum cellType{ ctGround = 0, ctEmpty = 255 };
    
      Vector2i mCellSize;
      Vector2i mTerrainSize;
      QByteArray mTerrain;
  };
  
  virtual configureMenuItem getCurrentConfigureMenuItem() const;
  virtual std::vector<QString> getConfigureMenuItems() const;
  virtual const treeD::Camera& getGameCamera() const {return mGameCamera;}
  virtual pauseMenuItem getCurrentPauseMenuItem() const;
  virtual std::vector<QString> getPauseMenuItems() const;
  virtual mainMenuItem getCurrentMainMenuItem() const;
  virtual std::vector<QString> getMainMenuItems() const;
  virtual const Stage& getStage() const { return mStage; }
  virtual double getPlayerCollisionRadius() const { return mPlayer.collisionRadius; }
  virtual double getPlayerHealth() const { return mPlayer.health; }
  virtual const Intersection2d& getPlayerIntersection() const {return mPlayer.intersection;}
  virtual const Point2d& getPlayerPosition() const { return mPlayer.position; }
  virtual state getState() const { return mState; }
  //virtual std::vector<Tile> getTiles( const Point2d&, const Point2d& ) const;
  virtual void keyPressed( int );
  virtual void keyReleased( int );
  virtual void registerClient( Client* );
  virtual void unregisterClient( Client* );
  
protected:

	//a mettre public? avec une methode Player& getPlayer() const
	struct Player
  {
  	Player() : health(100.0), position( 0.0 ), velocity(0.0), mState(sIdle), 
    	collisionRadius( 10 ) {;}
      
    enum state{ sIdle, sWalking, sRunning, sFalling, sJumping };
  	double health;
    Point2d position;
    Vector2d velocity;
    state mState;
    double collisionRadius;
    Intersection2d intersection;
  };

	state mState;
	std::vector<Client*> mClients;
  int mTimerId;
  mainMenuItem mMainMenuItem;
  configureMenuItem mConfigureMenuItem;
  pauseMenuItem mPauseMenuItem;
  std::map< int, bool > mKeys;
  Player mPlayer;
  realisim::treeD::Camera mGameCamera;
  Stage mStage;

	
  virtual void goToState( state );
  virtual void handleConfigureMenu();
  virtual void handleEditing();
  virtual void handleMainMenu();
  virtual void handlePauseMenu();
  virtual void handlePlaying();
  virtual bool isKeyPressed( Qt::Key, bool = false );
  virtual void send( event );
  virtual void timerEvent( QTimerEvent* );
};

#endif
