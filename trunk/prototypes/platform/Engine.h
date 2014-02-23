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
namespace realisim { namespace platform { using namespace math; class Actor; } }
namespace realisim { namespace platform { using namespace math; class Engine; } }
#include "utils/SpriteCatalog.h"
#include <vector>

class realisim::platform::Actor
{
public:
	Actor();
  Actor( const Actor& );
  Actor& operator=( const Actor& );
  virtual ~Actor();
  
  enum state{ sIdle, sWalking, sRunning, sFalling, sJumping, sHit };
  
  virtual void addIntersection( const Intersection2d& );
  virtual void clearIntersections();
  virtual const Vector2d& getAcceleration() const;
  virtual const Rectangle getBoundingBox() const;
  virtual const Circle getBoundingCircle() const;
  virtual double getHealth() const;
  virtual const vector<Intersection2d>& getIntersections() const;
  virtual const Vector2d& getMaximumAcceleration() const;
  virtual const Vector2d& getMaximumVelocity() const;  
  virtual QString getName() const;
  virtual const Point2d& getPosition() const;
  virtual QString getSpriteName() const;
  virtual QString getSpriteToken() const;
  virtual state getState() const;
  virtual const Vector2d& getVelocity() const;
  virtual void setAcceleration( const Vector2d& );
  virtual void setBoundingBox( const Rectangle& );
  virtual void setBoundingCircle( const Circle& );
  virtual void setHealth( double );
  virtual void setMaximumAcceleration( const Vector2d& );
  virtual void setMaximumVelocity( const Vector2d& );
  virtual void setName( QString );
  virtual void setPosition( const Point2d& );
  virtual void setSpriteName( QString );
  virtual void setSpriteToken( QString );
  virtual void setState( state );
  virtual void setVelocity( const Vector2d& );  
  
protected:
	QString mName;
  QString mSpriteName;
  QString mSpriteToken;  
  Rectangle mBoundingBox;
  Circle mBoundingCircle;
  double mHealth;
  Point2d mPosition;
  Vector2d mVelocity;
  Vector2d mMaximumVelocity;
  Vector2d mAcceleration;
  Vector2d mMaximumAcceleration;
  state mState;
  vector<Intersection2d> mIntersections;
  QTime mHitTimer;
};

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
  enum event{ eStageLoaded, eStateChanged, eFrameDone, eQuit,
    eErrorRaised };
  
  class Client
  {
  	friend class Engine;
  	public:
    	Client(){}
      virtual ~Client() {;}
      
    protected:
      virtual void gotEvent( event ) {};
  };
  
  //a mettre public? avec une methode Player& getPlayer() const
	class Player : public Actor
  {
  	public:
  		Player() : Actor() { setName( "Player1" ); setSpriteName("player"); }
  };
  /* expliquer les valeurs possible du terrain */
  class Stage
  {
  	friend class Engine;
  	public:
    	Stage();
      virtual ~Stage();
    
    	enum cellType{ ctEmpty = 0, ctStart, ctWayPoint, ctGround,
      	ctNumberOfCellType };
      
      virtual void addActor();
      virtual void addLayer();
      virtual void addToken( int, QString );
      virtual std::vector<int> find( cellType ) const;      
		  virtual Actor& getActor( int );
virtual QString getBackgroundToken() const;
      virtual Vector2i getCellCoordinate( const Point2d& ) const;
      virtual Vector2i getCellCoordinate( int ) const;
      virtual int getCellIndex( const Point2d& ) const;
      virtual int getCellIndex( int, int ) const; //Devrait etre Vector2i
      virtual std::vector<int> getCellIndices( const Point2d&, const Vector2i& ) const;
      virtual Vector2i getCellPixelCoordinate( int ) const;
      virtual Vector2i getCellPixelCoordinate( int, int ) const; //devrait etre Vector2i
      virtual Vector2i getCellSize() const {return mCellSize;}
      virtual QString getName() const { return mName; }
      virtual int getNumberOfActors() const { return mActors.size(); }
      virtual int getNumberOfCells() const { return mTerrain.size(); }
      virtual int getNumberOfLayers() const { return mLayers.size(); }
      virtual const QByteArray getTerrain() const { return mTerrain; }
      virtual int getTerrainHeight() const { return mTerrainSize.y(); };
      virtual Vector2i getTerrainSize() const {return mTerrainSize;}      
      virtual int getTerrainWidth() const { return mTerrainSize.x(); };
      virtual QString getToken( int, int ) const;
      virtual std::vector<QString> getTokens( int ) const;
      virtual bool hasCell( const Vector2i& ) const;
      virtual bool isLayerVisible( int ) const;
      virtual void removeActor( int );
      virtual void removeLayer(int);
    	virtual void setBackgroundToken( QString );
      virtual void setLayerAsVisible( int, bool = true );
      virtual QByteArray toBinary() const;
      virtual unsigned char value(int, int) const;
      virtual unsigned char value(int) const;
      
    protected:
    	Stage( QString, Vector2i );
      Stage (const Stage&);
      Stage& operator=( const Stage& );
    
    	struct Layer
      {
      	Layer( Vector2i iDataSize ) : 
        	mData( iDataSize.x() * iDataSize.y(), 255 ),
          mTokens(), mVisibility(true) {;}
        Layer( const Layer& iL ) : mData( iL.mData ), mTokens( iL.mTokens ),
          mVisibility( iL.mVisibility ) {;}
      	QByteArray mData;
        std::vector< QString > mTokens;
        bool mVisibility;
      };
      
      virtual void clear();
      virtual void fromBinary( QByteArray );
      
      QString mName;
      Vector2i mCellSize;
      Vector2i mTerrainSize;
      QByteArray mTerrain;
      std::vector<Layer*> mLayers;
      QString mBackgroundToken; //vector?
      vector<Actor*> mActors;
      static Actor mDummyActor;
  };
  
  virtual QString getAndClearLastErrors() const;
  virtual configureMenuItem getCurrentConfigureMenuItem() const;
  virtual int getCurrentLayer() const;
  virtual std::vector<QString> getConfigureMenuItems() const;
  virtual Stage::cellType getEditingTool() const { return mEditingTool; }
virtual QString getEditingSpriteToken() const { return mEditingSpriteToken; }
  virtual const treeD::Camera& getGameCamera() const {return mGameCamera;}
  virtual pauseMenuItem getCurrentPauseMenuItem() const;
  virtual std::vector<QString> getPauseMenuItems() const;
  virtual mainMenuItem getCurrentMainMenuItem() const;
  virtual std::vector<QString> getMainMenuItems() const;
  virtual const Point2i& getMousePos() const { return mMousePos; }
  virtual const Player& getPlayer() const { return mPlayer; }
virtual realisim::utils::SpriteCatalog& getSpriteCatalog();
  virtual Stage& getStage() { return mStage; }
  virtual state getState() const { return mState; }
  virtual std::vector<int> getVisibleCells() const;
  virtual bool hasError() const;
  virtual void keyPressed( int );
  virtual void keyReleased( int );
  virtual void loadStage( QString );
  virtual void mouseMoved( Point2i );
  virtual void mousePressed( int );
  virtual void mouseReleased( int );
  virtual void mouseWheelMoved( double );
  virtual void newStage( QString, int, int );
  virtual void registerClient( Client* );
  virtual void resolveCollision( Actor&, Intersection2d& );
  virtual void saveStage( QString );
  virtual void setCurrentLayer( int );
  virtual void setEditingTool( Stage::cellType iCt ) {mEditingTool = iCt;}
virtual void setEditingSpriteToken( QString i ) {mEditingSpriteToken = i;}
virtual void setSpriteCatalog( QString );
  virtual QString toString( Stage::cellType );
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
  double mMouseWheelDelta;
  Point2i mMousePos;
  Vector2i mMouseDelta;
  Player mPlayer;
  realisim::treeD::Camera mGameCamera;
  Stage mStage;
  Stage::cellType mEditingTool;
  realisim::utils::SpriteCatalog mSpriteCatalog;
  mutable QString mErrors;
  int mCurrentLayer;
  QString mEditingSpriteToken;
  QString mStageFilePath;

	virtual void addError( QString ) const;
  virtual void afterCollision( Actor& );
  virtual void applyPhysics( Actor& );
  virtual double getMouseWheelDelta(bool = true);
  virtual void goToState( state );
  virtual void handleActorInput( Actor& );
  virtual void handleActorCollisions();
  virtual void handleActorCollisions( Actor& );
  virtual void handleConfigureMenu();
  virtual void handleEditing();
  virtual void handleMainMenu();
  virtual void handleMapCollisions();
  virtual void handleMapCollisions(Actor&);
  virtual void handlePauseMenu();
  virtual void handlePlaying();
  virtual void handlePlayerInput();
  virtual bool isKeyPressed( Qt::Key, bool = false );
  virtual bool isMousePressed( Qt::MouseButtons, bool =false );
  virtual void loadStage( const Stage& );
  virtual void moveGameCamera();
  virtual void moveLeft( Actor& );
  virtual void moveRight( Actor& );
  virtual void moveUp( Actor& );
  virtual void send( event );
  virtual void timerEvent( QTimerEvent* );
  virtual void updateSpriteToken( Actor& );
};

#endif
