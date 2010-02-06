//

#ifndef Pong_GameWindow_hh
#define Pong_GameWindow_hh

#include <chipmunk.h>
#include <map>
namespace Pong{class Ball;}
namespace Pong{class Collision;}
namespace Pong{class Net;}
namespace Pong{class Player;}
class QCheckBox;
class QCloseEvent;
#include <QDialog>
class QKeyEvent;
class QMouseEvent;
class QTimerEvent;
#include "3d/Texture.h"
#include <vector>
#include "3d/Widget3d.h"

namespace Pong
{
using namespace std;
using namespace realisim;
  using namespace math;
  using namespace treeD;

//--- Options -----------------------------------------------------------------
class Options
{
public:
  Options();
  ~Options() {;}
  Options(const Options&);
  
  unsigned int getNumberOfPlayers() const {return mNumberOfPlayers;}
  bool isFullScreen() const {return mIsFullScreen;}
  const Options& operator=(const Options&);
  void setFullScreen(bool i){mIsFullScreen = i;}
  void setNumberOfPlayers(int i) {mNumberOfPlayers = i;}
private:
  bool mIsFullScreen;
  unsigned int mNumberOfPlayers;
};

//--- OptionsDialog-------------------------------------------------------------
class OptionsDialog : public QDialog
{
  Q_OBJECT
public:
	OptionsDialog(QWidget*);
  ~OptionsDialog(){;}
  
  const Options& getOptions() const {return mOptions;}
  void setOptions(const Options&);
  
protected slots:
  void optionsChanged();
    
protected:
  QCheckBox* mpFullScreen;
  
  Options mOptions;
};

//--- Board --------------------------------------------------------------------
class Board
{
public:
  Board();
  virtual ~Board(){;}
  
  virtual void draw() const;
  virtual const Matrix4d& getPlayerTransformation(int i) const {return mPlayerTransformations[i];}
  virtual const Matrix4d& getNetTransformation(int i) const {return mNetTransformations[i];}
  virtual unsigned int getMaxNumberOfPlayers() const = 0;
  virtual const QSizeF& getSize() const {return mSize;}
  virtual void setSize(double w, double h) {mSize = QSizeF(w, h);}

protected:
  QSizeF mSize;
  vector<Matrix4d> mPlayerTransformations;
  vector<Matrix4d> mNetTransformations;
};

class Board2 : public Board
{
public:
	Board2();
  virtual ~Board2() {;}
  
  virtual unsigned int getMaxNumberOfPlayers() const {return 2;}
};

class Board4 : public Board
{
public:
  Board4();
  virtual ~Board4() {;}
  
  virtual unsigned int getMaxNumberOfPlayers() const {return 4;}
};

//--- physics -----------------------------------------------------------------
/*
	mPlayers: vecteur des representation physique des joueurs (pour les collisions)
  mNets: vecteur des representation physiqye des but (pour les collisions)
  
  mDataPlayers: vecteur des joueurs
  mDataNets: vecteur des buts
  
  Note: Les vecteur des joueurs/buts ainsi que leur representation physique
    doivent être parfaitement synchroniser... Si on ajoute a l'un, il faut
    ajouter a l'autre en même temps (même chose pour les enlever). De plus,
    il ne faut pas ajouter/enlever durant une iteration de physique. Il faut le
    faire avant ou apres. C'est pour cette raison que la classe Physique se
    permet d'effacer les item de mDataPlayers/Nets. 
*/
class GameWindow;
class Physics
{
public:
  explicit Physics(GameWindow*, const Board&, vector<Ball*>&,
    vector<Net*>&, vector<Player*>&);
  virtual ~Physics();
  
  void drawCollisions();
  Point3d getClosestBallPos(Player* p) const;
  void markPlayerForRemoval(int iId) {mPlayersToRemove.push_back(iId);}
  void resetBalls();
  void updatePhysics();
  
protected:
  enum CollisionType {ctBall, ctBoard, ctPlayer, ctNet};
  
  void removeMarkedPlayers();
  void removeShapes(cpBody*);
  void removeStaticShapes(cpBody*);
  
  static int ballToNetCollisionFunc(cpShape*, cpShape*, cpContact*, int, cpFloat,
    void*);
  static void ballVelocityFunc(cpBody*, cpVect, cpFloat, cpFloat);
  static int defaultCollisionFunc(cpShape*, cpShape*, cpContact*, int, cpFloat,
    void*);
  static void drawCollisionsFunc(void*, void*);
  
  static GameWindow* mpGameWindow; //not owned
      
  cpSpace* mpSpace;
  cpBody* mpBoard;
  
  vector<cpBody*> mPlayers;
  vector<cpBody*> mBalls;
  static vector<cpBody*> mNets;
  vector<cpShape*> mShapes;
  
  vector<Ball*>& mDataBalls; //not owned
  vector<Net*>& mDataNets; //not owned
  vector<Player*>& mDataPlayers; //not owned
  
  vector<int> mPlayersToRemove;
};


const unsigned int cMaxCollisions = 50;
//--- GameWindow----------------------------------------------------------------
class GameWindow : public realisim::treeD::Widget3d
{
  Q_OBJECT
public:
	GameWindow(QWidget* = 0);
	virtual ~GameWindow();
  
  void addPlayer(Player*);
  void addCollision(const Vector3d&, const Vector3d&);
  virtual void score(int);
  virtual void showOptions() const;
  
protected slots:
  void applyOptions();

protected:
  enum GameState {gsNotStarted, gsPaused, gsRunning};
  
  virtual bool canPlayerMoveTo(const Player*, const Point3d&) const;
  virtual void drawBalls() const;
  virtual void drawCollisions();
  virtual void drawGameBoard() const;
  virtual void drawGamePanel() const;
  virtual void drawNets() const;
  virtual void drawPlayers() const;
  virtual void eliminatePlayer(int iId);
  const GameState getState() const {return mState;}
  virtual void initializeGL();
  virtual void keyPressEvent(QKeyEvent*);
  virtual void mouseDoubleClickEvent(QMouseEvent*) {;}
  virtual void mouseMoveEvent(QMouseEvent*);
  virtual void mousePressEvent(QMouseEvent*);
  virtual void mouseReleaseEvent(QMouseEvent*);
  //virtual void wheelEvent(QWheelEvent*) {;}
  virtual void paintGL();
  virtual void pauseGame();
//virtual void removePlayer(Player*);
  virtual void resumeGame();
  virtual void startGame();
  virtual void setState(GameState gs) {mState = gs;}
  virtual void timerEvent(QTimerEvent*);
  virtual void updateAi();
  virtual void updateUserInput();
  virtual void updatePhysics();
  virtual void updateDisplay();
//  virtual void updateNetwork();
  
  OptionsDialog* mpOptionsDialog;
  Options mOptions;
  GameState mState;
  int mGameTimerId;
  vector<Player*> mPlayers;
  vector<Player*> mEliminatedPlayers;
  Player* mpLocalPlayer;
  Board* mpBoard;
  vector<Net*> mNets;
  QPoint mMousePosition;
  vector<Ball*> mBalls;
  Physics* mpPhysics;
  Collision* mCollisions[cMaxCollisions];
  Texture mGameTexture;
};

}

#endif
