/* */

#ifndef realisim_platform_engine_hh
#define realisim_platform_engine_hh

#include "3d/Camera.h"
#include "3d/Sprite.h"
#include "Math/BoundingBox.h"
#include "Math/intersection.h"
#include "Math/Vect.h"
#include "Math/Point.h"
#include <map>
#include <QObject>
#include <QImage>
#include <QTimerEvent>
namespace realisim { namespace platform { using namespace math; class Actor; } }
namespace realisim { namespace platform { using namespace treeD; class Animation; } }
namespace realisim { namespace platform { class GameEntity; } }
namespace realisim { namespace platform { class Engine; } }
namespace realisim { namespace platform { class Physics; } }
namespace realisim { namespace platform { class Projectile; } }
namespace realisim { namespace platform { class Stage; } }
namespace realisim { namespace platform { class Weapon; } }
#include "utils/SpriteCatalog.h"
#include <vector>

class realisim::platform::Animation
{
public:
	Animation() {;}
  virtual ~Animation() {;}
  
  Point2d getPosition() const {return mPosition;}
  Sprite getSprite() const {return mSprite;}
  bool isDone() const { return mSprite.isAnimationDone(); }
  void setPosition( const Point2d& p) {mPosition = p;}
  void setSprite( const Sprite& s ) { mSprite = s; mSprite.startAnimation(); } 
protected:
	Point2d mPosition;
  Sprite mSprite;
};
  
//------------------------------------------------------------------------------
class realisim::platform::GameEntity
{
public:
	GameEntity();
  virtual ~GameEntity() = 0;
  
  void addIntersection( const Intersection2d& );
  void applyGravity(bool i) {mIsGravityApplied = i;}
  void clearIntersections();
  const Vector2d& getAcceleration() const;
  const Rectangle getBoundingBox() const;
  const Circle getBoundingCircle() const;
  QString getCurrentSpriteToken() const {return mCurrentSpriteToken;}
  Intersection2d getIntersection(int i) const {return mIntersections[i];}
  const Vector2d& getMaximumAcceleration() const;
  const Vector2d& getMaximumVelocity() const;  
  int getNumberOfIntersections() const {return mIntersections.size();}
  const Point2d& getPosition() const;
  const Sprite& getSprite() const { return mSprite; }
  QString getSpriteToken( int ) const;
  const Vector2d& getVelocity() const;
  bool hasIntersections() const;
  bool isGravityApplied() const {return mIsGravityApplied;}
  bool isMarkedForDeletion() const { return mMarkedForDeletion; }
  void markForDeletion( bool iD ) { mMarkedForDeletion = iD; }
  void setAcceleration( const Vector2d& );
  void setBoundingBox( const Rectangle& );
  void setBoundingCircle( const Circle& );
  void setEngine( realisim::platform::Engine* iE ) {mpEngine = iE;}
  void setMaximumAcceleration( const Vector2d& );
  void setMaximumVelocity( const Vector2d& );
  void setPosition( const Point2d& );
  void setSprite( QString );
  void setSpriteToken( int, QString );
  void setVelocity( const Vector2d& );
  virtual void update() = 0;
  virtual void updateAi() = 0;
  
protected:
  std::map<int, QString> mSpriteTokens;
  QString mCurrentSpriteToken;
  Rectangle mBoundingBox;
  Circle mBoundingCircle;
  Point2d mPosition;
  Vector2d mVelocity;
  Vector2d mMaximumVelocity;
  Vector2d mAcceleration;
  Vector2d mMaximumAcceleration;
  vector<Intersection2d> mIntersections;
  bool mIsGravityApplied;
  realisim::platform::Engine* mpEngine;
  Sprite mSprite;
  bool mMarkedForDeletion;
};

//------------------------------------------------------------------------------
class realisim::platform::Weapon
{
public:
  Weapon();
  virtual ~Weapon();
  
  enum type{ tNone, tPellet };
  
  bool canFire() const;
  Projectile* fire( const Vector2d& );
  double getFireRate() const;
  type getType() const;
  void setFireRate( double );
  void setType(type t);
  
protected:
	type mType;
  QTime mLastFire;
  double mFireRate;
};

//------------------------------------------------------------------------------
class realisim::platform::Projectile : public realisim::platform::GameEntity
{
public:
  Projectile();
  virtual ~Projectile();
  
  enum state{ sIdle, sHorizontal, sVertical, sExploding };
  
  double getDamage() const { return mDamage; }
  Weapon::type getType() const {return mType;}
  void setDamage( double d ) { mDamage = d; }
  void setType( Weapon::type t);
  virtual void update();
  virtual void updateAi();
// int getRemainingLife() const;
// void getLifeSpan() const;
// void setLifeSpan( int );
  
protected:
	void setState( state );
  
	Weapon::type mType;
  double mDamage;
  state mState;
//	QTime mStart;
//  int mLifeSpan; //ms
};

//------------------------------------------------------------------------------
class realisim::platform::Actor : public GameEntity
{
public:
	Actor();
  virtual ~Actor() = 0;
  
  enum state{ sIdle, sRunningLeft, sRunningRight, sFalling,
    sJumping, sHit, sPushingLeft, sPushingRight, sHittingCeiling,
    sDead };
  
  void attack();
  Vector2d getAimingDirection() const {return mAimingDirection;}
  double getHealth() const;
  QString getName() const;
  state getState() const;
  const Weapon& getWeapon() const;
  void moveLeft();
  void moveRight();
  void moveUp();
  void setAimingDirection( const Vector2d& d ) {mAimingDirection = d;}
  void setHealth( double );
  void setName( QString );
  void setState( state );
  void setWeapon( const Weapon& );
  virtual void update();
  virtual void updateAi();
  
protected:
	QString mName;
  double mHealth;
  state mState;
  QTime mHitTimer;
  Weapon mWeapon;
  Vector2d mAimingDirection;
};

//------------------------------------------------------------------------------
//--- Monster
//------------------------------------------------------------------------------
class Player : public realisim::platform::Actor
{
  public:
    Player();
    virtual ~Player() {}
    virtual void update();
    virtual void updateAi();
};

//------------------------------------------------------------------------------
//--- Monster
//------------------------------------------------------------------------------
class Monster : public realisim::platform::Actor
{
  public:
    enum monsterType{ mtBrownSlime, mtBigGreen };
    
    Monster();
    Monster(monsterType);
    virtual ~Monster() {}
    
    monsterType getType() const {return mType;}
    void setType(monsterType);
    virtual void update();
    virtual void updateAi();
    
  protected:
  	monsterType mType;
};

//------------------------------------------------------------------------------
//--- Stage
//------------------------------------------------------------------------------
/* expliquer les valeurs possible du terrain */
class realisim::platform::Stage
{
  friend class Engine;
  public:
    Stage();
    ~Stage();
  
    enum cellType{ ctEmpty = 0, ctStart, ctWayPoint, ctGround,
      ctNumberOfCellType };
    
    void addLayer();
//    void addActor();
    void add( Monster* );
    void addToken( int, QString );
    std::vector<int> find( cellType ) const;      
    Actor& getActor( int );
QString getBackgroundToken() const;
    Vector2i getCellCoordinate( const Point2d& ) const;
    Vector2i getCellCoordinate( int ) const;
    int getCellIndex( const Point2d& ) const;
    int getCellIndex( int, int ) const; //Devrait etre Vector2i
    std::vector<int> getCellIndices( const Point2d&, const Vector2i& ) const;
    Vector2i getCellPixelCoordinate( int ) const;
    Vector2i getCellPixelCoordinate( int, int ) const; //devrait etre Vector2i
    Vector2i getCellSize() const {return mCellSize;}
    Monster& getMonster(int);
    QString getName() const { return mName; }
    int getNumberOfActors() const;
    int getNumberOfCells() const { return mTerrain.size(); }
    int getNumberOfLayers() const { return mLayers.size(); }
    int getNumberOfMonsters() const;
    const QByteArray getTerrain() const { return mTerrain; }
    int getTerrainHeight() const { return mTerrainSize.y(); };
    Vector2i getTerrainSize() const {return mTerrainSize;}      
    int getTerrainWidth() const { return mTerrainSize.x(); };
    QString getToken( int, int ) const;
    std::vector<QString> getTokens( int ) const;
    bool hasCell( const Vector2i& ) const;
    bool isLayerVisible( int ) const;
    void removeActor( int );
    void removeLayer(int);
    void removeMonster( int );
    void setBackgroundToken( QString );
    void setEngine( Engine* ipE ) {mpEngine = ipE;}
    void setLayerAsVisible( int, bool = true );
    QByteArray toBinary() const;
    unsigned char value(int, int) const;
    unsigned char value(int) const;
    
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
        mVisibility( iL.mVisibility ) {;} //a enlever?
      QByteArray mData;
      std::vector< QString > mTokens;
      bool mVisibility;
    };
    
    void clear();
    void fromBinary( QByteArray );
    
    Engine* mpEngine;
    QString mName;
    Vector2i mCellSize;
    Vector2i mTerrainSize;
    QByteArray mTerrain;
    std::vector<Layer*> mLayers;
    QString mBackgroundToken; //vector?
    vector<Actor*> mActors;
    vector<Monster*> mMonsters;
    //vector<NonPlayingCaracter> mNpcs;
};

//------------------------------------------------------------------------------
//--- Physics
//------------------------------------------------------------------------------
class realisim::platform::Physics
{
public:
	Physics();
  virtual ~Physics();
  
  void resolveCollisions( GameEntity&, Stage& );
  void resolveCollisions( Player&, Monster& );
  void resolveCollisions( Projectile&, Actor& );
  void update( GameEntity& );
  
protected:
  //Vector2d mGravity;
};

//------------------------------------------------------------------------------
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
  
  virtual void addProjectile( Projectile* );
  virtual void addAnimation( Animation* );
  virtual QString getAndClearLastErrors() const;
  virtual const Animation& getAnimation(int i) const {return *mAnimations[i];}
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
  virtual int getNumberOfAnimations() const { return mAnimations.size(); }
  virtual int getNumberOfProjectiles() const { return mProjectiles.size(); }
  virtual const Player& getPlayer() const { return mPlayer; }
  virtual Projectile& getProjectile(int i) { return *mProjectiles[i]; }
virtual realisim::utils::SpriteCatalog& getSpriteCatalog();
  virtual Stage& getStage() { return mStage; }
  virtual state getState() const { return mState; }
  virtual const std::vector<int>& getVisibleCells() const;
  virtual void graphicsAreReady();
  virtual bool hasError() const;
  virtual bool isKeyPressed( Qt::Key, bool = false );
  virtual bool isMousePressed( Qt::MouseButtons, bool =false );
virtual bool isVisible( const GameEntity& ) const; // a renommer pour isWithinCameraSight?
  virtual void keyPressed( int );
  virtual void keyReleased( int );
  virtual void loadStage( QString );
  virtual void mouseMoved( Point2i );
  virtual void mousePressed( int );
  virtual void mouseReleased( int );
  virtual void mouseWheelMoved( double );
  virtual void newStage( QString, int, int );
  virtual void registerClient( Client* );
  virtual void saveStage( QString );
  virtual void setCurrentLayer( int );
  virtual void setEditingTool( Stage::cellType iCt ) {mEditingTool = iCt;}
virtual void setEditingSpriteToken( QString i ) {mEditingSpriteToken = i;}
  virtual QString toString( Stage::cellType );
  virtual void unregisterClient( Client* );
  
protected:
	Physics mPhysics;
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
  std::vector<Projectile*> mProjectiles;
  std::vector<Animation*> mAnimations;
  std::vector<int> mVisibleCells;
  std::vector<GameEntity*> mEntities;

	virtual void addError( QString ) const;
  virtual void computeVisibleCells();
  virtual double getMouseWheelDelta(bool = true);
  virtual void goToState( state );
  virtual void handleConfigureMenu();
  virtual void handleEditing();
  virtual void handleMainMenu();
  virtual void handlePauseMenu();
  virtual void handlePlaying();
  virtual void loadStage( const Stage& );
  virtual void moveGameCamera();
  virtual void refreshGameEntityList();
  virtual void send( event );
  virtual void setSpriteCatalog( QString );
  virtual void timerEvent( QTimerEvent* );
};

#endif
