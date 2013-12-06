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
#include "utils/SpriteCatalog.h"
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
  enum event{ eStageLoaded, eStateChanged, eFrameDone, eQuit,
    eErrorRaised };
  
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
  
  /* expliquer les valeurs possible du terrain */
  class Stage
  {
  	friend class Engine;
  	public:
    	Stage();
      virtual ~Stage();
    
    	enum cellType{ ctEmpty = 0, ctStart, ctWayPoint, ctGround,
      	ctNumberOfCellType };
      
      virtual std::vector<int> find( cellType ) const;      
virtual QString getBackgroundToken() const;
      virtual Vector2i getCellCoordinate( const Point2d& ) const;
      virtual Vector2i getCellCoordinate( int ) const;
      virtual int getCellIndex( const Point2d& ) const;
      virtual int getCellIndex( int, int ) const;
      virtual std::vector<int> getCellIndices( const Point2d&, const Vector2i& ) const;
      virtual Vector2i getCellPixelCoordinate( int ) const;
      virtual Vector2i getCellPixelCoordinate( int, int ) const;
      virtual Vector2i getCellSize() const {return mCellSize;}
      virtual QString getName() const { return mName; }
      virtual int getNumberOfLayers() const { return mLayers.size(); }
      virtual const QByteArray getTerrain() const { return mTerrain; }
      virtual int getTerrainHeight() const { return mTerrainSize.y(); };
      virtual Vector2i getTerrainSize() const {return mTerrainSize;}      
      virtual int getTerrainWidth() const { return mTerrainSize.x(); };
      virtual QString getToken( int, int ) const;
      virtual std::vector<QString> getTokens( int ) const;      
      virtual bool isLayerVisible( int ) const;
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
      
      virtual void addLayer();
      virtual void addToken( int, QString );
      virtual void clear();
      virtual void fromBinary( QByteArray );
      virtual void removeLayer(int);
    	virtual void setBackgroundToken( QString );
      
      QString mName;
      Vector2i mCellSize;
      Vector2i mTerrainSize;
      QByteArray mTerrain;
      std::vector<Layer*> mLayers;
      QString mBackgroundToken; //vector?
  };
  
  virtual void addLayer();
  virtual void addTokenToLayer( int, QString );
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
virtual realisim::utils::SpriteCatalog& getSpriteCatalog();
  virtual const Stage& getStage() const { return mStage; }
  virtual const Vector2d& getPlayerAcceleration() const { return mPlayer.mAcceleration; }
  virtual BoundingBox2d getPlayerBoundingBox() const;
  virtual double getPlayerHealth() const { return mPlayer.mHealth; }
  virtual const Intersection2d& getPlayerIntersection() const {return mPlayer.mIntersection;}
  virtual const Point2d& getPlayerPosition() const { return mPlayer.mPosition; }
  virtual Player::state getPlayerState() const { return mPlayer.mState; }
  virtual const Vector2d& getPlayerVelocity() const { return mPlayer.mVelocity; }
  virtual state getState() const { return mState; }
  virtual std::vector<int> getVisibleCells() const;
  virtual bool hasError() const;
  virtual void keyPressed( int );
  virtual void keyReleased( int );
  virtual void loadStage( QString );
  virtual void mouseMoved( Point2i );
  virtual void mousePressed( int );
  virtual void mouseReleased( int );
  virtual void newStage( QString, int, int );
  virtual void registerClient( Client* );
  virtual void removeLayer(int);
  virtual void saveStage( QString );
  virtual void setBackgroundToken( QString );
virtual void setCurrentLayer( int );
  virtual void setEditingTool( Stage::cellType iCt ) {mEditingTool = iCt;}
virtual void setEditingSpriteToken( QString i ) {mEditingSpriteToken = i;}
  virtual void setLayerAsVisible( int, bool );
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

	virtual void addError( QString ) const;
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
  virtual void loadStage( const Stage& );
  virtual void moveGameCamera();
  virtual void send( event );
  virtual void timerEvent( QTimerEvent* );
};

#endif
