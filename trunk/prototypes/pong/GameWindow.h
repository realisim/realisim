//

#ifndef Pong_GameWindow_hh
#define Pong_GameWindow_hh

#include <chipmunk.h>
namespace Pong{class Ball;}
namespace Pong{class Player;}
class QCheckBox;
class QCloseEvent;
#include <QDialog>
class QKeyEvent;
class QMouseEvent;
class QTimerEvent;
#include <vector>
#include "Widget3d.h"

namespace Pong
{
using namespace std;
using namespace realisim;
  using namespace math;

//--- Options -----------------------------------------------------------------
class Options
{
public:
  Options();
  ~Options() {;}
  Options(const Options&);
  
  int getNumberOfPlayers() const {return mNumberOfPlayers;}
  bool isFullScreen() const {return mIsFullScreen;}
  const Options& operator=(const Options&);
  void setFullScreen(bool i){mIsFullScreen = i;}
  void setNumberOfPlayers(int i) {mNumberOfPlayers = i;}
private:
  bool mIsFullScreen;
  int mNumberOfPlayers;
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
  virtual ~Board() {;}
  
  virtual void draw() const;
  const Matrix4d& getPlayer1Transformation() const {return mPlayer1Transformation;}
  const Matrix4d& getPlayer2Transformation() const {return mPlayer2Transformation;}
  const QSizeF& getSize() const {return mSize;}

protected:
  QSizeF mSize;
  Matrix4d mPlayer1Transformation;
  Matrix4d mPlayer2Transformation;
};

//--- physics -----------------------------------------------------------------
class Physics
{
public:
  explicit Physics(const Board&, vector<Ball*>&, vector<Player*>&);
  virtual ~Physics();
  
  void updatePhysics();
  void drawCollisions();
  
protected:
  static void drawCollisionsFunc(void *ptr, void *data);
  
  cpSpace* mpSpace;
  cpBody* mpBoard;
  vector<cpBody*> mPlayers;
  vector<cpBody*> mPlayerHolders;
  vector<cpBody*> mBalls;
  
  vector<Ball*>& mDataBalls;
  vector<Player*>& mDataPlayers;
};

//--- GameWindow----------------------------------------------------------------
class GameWindow : public realisim::treeD::Widget3d
{
  Q_OBJECT
public:
	GameWindow(QWidget* = 0);
	virtual ~GameWindow();
  
  void addPlayer(Player*);
  virtual void showOptions() const;
  
protected slots:
  void applyOptions();

protected:
  enum GameState {gsNotStarted, gsPaused, gsRunning};
  
  virtual void drawBalls() const;
  virtual void drawGameBoard() const;
  virtual void drawPlayers() const;
  const GameState getState() const {return mState;}
  virtual void keyPressEvent(QKeyEvent*);
  virtual void mouseDoubleClickEvent(QMouseEvent*) {;}
  virtual void mouseMoveEvent(QMouseEvent*);
  virtual void mousePressEvent(QMouseEvent*);
  virtual void mouseReleaseEvent(QMouseEvent*);
  //virtual void wheelEvent(QWheelEvent*) {;}
  virtual void paintGL();
  virtual void pauseGame();
  virtual void resumeGame();
  virtual void startGame();
  virtual void setState(GameState gs) {mState = gs;}
  virtual void timerEvent(QTimerEvent*);
  virtual void updateUserInput();
  virtual void updatePhysics();
  virtual void updateDisplay();
//  virtual void updateNetwork();
  
  OptionsDialog* mpOptionsDialog;
  Options mOptions;
  GameState mState;
  int mGameTimerId;
  vector<Player*> mPlayers;
  Player* mpLocalPlayer;
  Board* mpBoard;
  QPoint mMousePosition;
  vector<Ball*> mBalls;
  Physics* mpPhysics;
};

}

#endif
