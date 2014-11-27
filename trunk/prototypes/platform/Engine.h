/* */

#ifndef realisim_platform_engine_hh
#define realisim_platform_engine_hh

#include "3d/Camera.h"
#include "3d/Sprite.h"
#include "GameEntity.h"
#include "Math/Vect.h"
#include "Math/Point.h"
#include <map>
#include "Physics.h"
#include <QObject>
#include <QTimerEvent>
namespace realisim { namespace platform { using namespace treeD; class Animation; } }
namespace realisim { namespace platform { class Stage; } }
#include "utils/CommandStack.h"
#include "3d/SpriteCatalog.h"
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
//--- Stage
//------------------------------------------------------------------------------
/* expliquer les valeurs possible du terrain */
class realisim::platform::Stage
{
  friend class Engine;
  friend class CommandCellEdition;
  public:
    Stage();
    ~Stage();
  
    enum cellType{ ctEmpty = 0, ctStart, ctWayPoint, ctGround,
    	ctDestructibleGround, ctNumberOfCellType };
      
    struct Layer
    {
      Layer( Vector2i iDataSize ) : 
        mData( iDataSize.x() * iDataSize.y(), 255 ),
        mTokens(), mVisibility(true), mName() {;}
      Layer( const Layer& iL ) : mData( iL.mData ), mTokens( iL.mTokens ),
        mVisibility( iL.mVisibility ), mName(iL.mName) {;} //a enlever?
      QByteArray mData;
      std::vector< QString > mTokens;
      bool mVisibility;
      QString mName;
    };

    
    bool actsAsGround( cellType ) const;
    void addLayer();
//    void addActor();
    void add( Monster* );
    void add( Weapon* );
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
    Layer* getLayer(int i) {return mLayers[i];}
    QString getLayerName(int) const;
    Monster& getMonster(int);
    Weapon& getWeapon(int);
    QString getName() const { return mName; }
    int getNumberOfActors() const;
    int getNumberOfCells() const { return mTerrain.size(); }
    int getNumberOfLayers() const { return mLayers.size(); }
    int getNumberOfMonsters() const;
    int getNumberOfWeapons() const;
    const QByteArray getTerrain() const { return mTerrain; }
    int getTerrainHeight() const { return mTerrainSize.y(); };
    int getTerrainLayerIndex() const { return 0; } // a changer des que possible...
    Vector2i getTerrainSize() const {return mTerrainSize;}      
    int getTerrainWidth() const { return mTerrainSize.x(); };
    QString getToken( int, int ) const;
    std::vector<QString> getTokens( int ) const;
    bool hasCell( const Vector2i& ) const;
    bool isLayerVisible( int ) const;
    void removeActor( int );
    void removeLayer(int);
    void removeMonster( int );
    void removeWeapon( int );
    void setBackgroundToken( QString );
    void setCellValue( int, cellType );
    void setEngine( Engine* ipE ) {mpEngine = ipE;}
    void setLayerAsVisible( int, bool = true );
    void setLayerName( int, QString );
    QByteArray toBinary() const;
    cellType value(int, int) const;
    cellType value(int) const;
    
  protected:
    Stage( QString, Vector2i );
    Stage (const Stage&);
    Stage& operator=( const Stage& );
      
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
    vector<Weapon*> mWeapons;
    //vector<NonPlayingCaracter> mNpcs;
};

//------------------------------------------------------------------------------

namespace realisim { namespace platform { class CommandCellEdition ;} } 
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
    eErrorRaised, eEditorUiChanged };
  
  class Client
  {
  	friend class Engine;
  	public:
    	Client(){}
      virtual ~Client() {;}
      
    protected:
      virtual void gotEvent( event ) {};
  };
  
  class Mouse
  {
  public:
  	Mouse() : mButtons(), mWheelDelta(0), mPosition(-1, -1), mDelta(0) {}
    ~Mouse() {}
    friend class Engine;
    
    const Vector2i& getDelta() const {return mDelta;}
    const Point2i& getPosition() const {return mPosition;}
    const double getWheelDelta() const {return mWheelDelta;}
    bool isButtonPressed( Qt::MouseButtons, bool = false ) const;
    
  protected:
    mutable std::map< int, bool > mButtons;
    double mWheelDelta;
    Point2i mPosition;
    Vector2i mDelta;
  };
  
  virtual void addProjectile( Projectile* );
  virtual void addAnimation( Animation* );
  virtual void addAnimation( const Sprite&, const Point2d& );
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
  const Mouse& getMouse() const {return mMouse;}
  virtual int getNumberOfAnimations() const { return mAnimations.size(); }
  virtual int getNumberOfProjectiles() const { return mProjectiles.size(); }
  virtual Physics& getPhysics() {return mPhysics;}
  virtual Player& getPlayer() { return mPlayer; }
  virtual Projectile& getProjectile(int i) { return *mProjectiles[i]; }
virtual realisim::treeD::SpriteCatalog& getSpriteCatalog();
  virtual Stage& getStage() { return mStage; }
  virtual state getState() const { return mState; }
  virtual const std::vector<int>& getVisibleCells() const;
  virtual void graphicsAreReady();
  virtual bool hasError() const;
  virtual bool isKeyPressed( Qt::Key, bool = false );
virtual bool isVisible( const GameEntity& ) const; // a renommer pour isWithinCameraSight?
  virtual void keyPressed( int );
  virtual void keyReleased( int );
  virtual void loadStage( QString );
  virtual void mouseMoved( Point2i );
  virtual void mousePressed( int );
  virtual void mouseReleased( int );
  virtual void mouseWheelMoved( double );
  virtual void moveGameCameraTo( const Point2d& );
  virtual void newStage( QString, int, int );
  double random() const { return qrand() / (double)RAND_MAX; }
  virtual void registerClient( Client* );
  virtual void saveStage( QString );
  virtual void setCurrentLayer( int );
  virtual void setEditingTool( Stage::cellType iCt ) {mEditingTool = iCt;}
virtual void setEditingSpriteToken( QString i ) {mEditingSpriteToken = i;}
	virtual void setGameCamera( const Camera& iC ) {mGameCamera = iC;}
  virtual void startLevel();
  virtual void startLevel(int);
  virtual QString toString( Stage::cellType );
  virtual void unregisterClient( Client* );
  
  
  //fonctions de l'éditeur
  void addLayer();
  void removeLayer( int );
  
protected:
	Physics mPhysics;
	state mState;
	std::vector<Client*> mClients;
  int mTimerId;
  mainMenuItem mMainMenuItem;
  configureMenuItem mConfigureMenuItem;
  pauseMenuItem mPauseMenuItem;
  std::map< int, bool > mKeys;
  Mouse mMouse;
  Player mPlayer;
  realisim::treeD::Camera mGameCamera;
  Stage mStage;
  Stage::cellType mEditingTool;
  realisim::treeD::SpriteCatalog mSpriteCatalog;
  mutable QString mErrors;
  int mCurrentLayer;
  QString mEditingSpriteToken;
  QString mStageFilePath;
  std::vector<Projectile*> mProjectiles;
  std::vector<Animation*> mAnimations;
  std::vector<int> mVisibleCells;
  std::vector<GameEntity*> mEntities;
  QTime mStartLevelTimer;
  int mStartLevelDelay;
  utils::CommandStack mEditorCommands;
  CommandCellEdition* mCommandCellEdition;
  QTime mMainLoopTimer;

	virtual void addError( QString ) const;
  virtual void computeVisibleCells();
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
  virtual void updateLogic();
};

#endif
