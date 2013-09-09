/* */

#ifndef realisim_platform_engine_hh
#define realisim_platform_engine_hh

#include "3d/Camera.h"
#include "Math/BoundingBox.h"
#include "Math/intersection.h"
#include "Math/Vect.h"
#include <map>
#include <QObject>
#include <QImage>
#include <QTimerEvent>
namespace realisim { namespace platform { using namespace math; class Engine; } }
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
  enum event{ eStageChanged, eStateChanged, eFrameDone, eQuit };
  
  class Client
  {
  	friend class Engine;
  	public:
    	Client(){}
      virtual ~Client() {}
      
    protected:
      virtual void gotEvent( event ) {};
  };
  
  //a mettre public? avec une methode Player& getPlayer() const
	struct Player
  {
  	Player() : mSize(30, 60), mHealth(100.0), mPosition( 0.0 ), mVelocity(0.0),
    	mAcceleration(0.0), mState(sIdle){;}
      
    enum state{ sIdle, sWalking, sRunning, sFalling, sJumping };
    Vector2i mSize;
  	double mHealth;
    Point2d mPosition;
    Vector2d mVelocity;
    Vector2d mAcceleration;
    state mState;
    Intersection2d mIntersection;
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
      virtual Vector2i getCellPixelCoordinate( int ) const;
      //virtual QByteArray getTerrain( const Point2d&, const Vector2i& ) const;
      virtual Vector2i terrainSize() const {return mTerrainSize;}
    	virtual const QByteArray terrain() const { return mTerrain; }
      
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
  virtual const Point2i& getMousePos() const { return mMousePos; }
  virtual const Stage& getStage() const { return mStage; }
  virtual BoundingBox2d getPlayerBoundingBox() const;
  virtual double getPlayerHealth() const { return mPlayer.mHealth; }
  virtual const Intersection2d& getPlayerIntersection() const {return mPlayer.mIntersection;}
  virtual const Point2d& getPlayerPosition() const { return mPlayer.mPosition; }
  virtual Player::state getPlayerState() const { return mPlayer.mState; }
  virtual state getState() const { return mState; }
  virtual void keyPressed( int );
  virtual void keyReleased( int );
  virtual void mouseMoved( Point2i );
  virtual void mousePressed( int );
  virtual void mouseReleased( int );
  virtual void registerClient( Client* );
  virtual void unregisterClient( Client* );
  
protected:
	state mState;
	std::vector<Client*> mClients;
  int mTimerId;
  mainMenuItem mMainMenuItem;
  configureMenuItem mConfigureMenuItem;
  pauseMenuItem mPauseMenuItem;
  std::map< int, bool > mKeys;
  std::map< int, bool > mMouseButtons;
  Point2i mMousePos;
  Vector2i mMouseDelta;
  Player mPlayer;
  realisim::treeD::Camera mGameCamera;
  Stage mStage;

  virtual void goToState( state );
  virtual void handleConfigureMenu();
  virtual void handleEditing();
  virtual void handleMainMenu();
  virtual void handleMapCollisions();
  virtual void handlePauseMenu();
  virtual void handlePlaying();
  virtual void handlePlayerInput();
  virtual bool isKeyPressed( Qt::Key, bool = false );
  virtual bool isMousePressed( Qt::MouseButtons, bool =false );
  virtual void send( event );
  virtual void timerEvent( QTimerEvent* );
};

#endif
