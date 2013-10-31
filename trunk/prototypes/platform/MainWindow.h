

#ifndef MainWindow_hh
#define MainWindow_hh

#include "Engine.h"
#include <QMainWindow>
class QKeyEvent;
class QLineEdit;
class QTimerEvent;
#include "3d/Sprite.h"
#include "3d/Text.h"
#include "3d/Texture.h"
#include "3d/Widget3d.h"
#include <QtGui>

namespace realisim { namespace platform { class Viewer;} }

class realisim::platform::Viewer :
	public realisim::treeD::Widget3d, public Engine::Client
{
public:
  Viewer(Engine&, QWidget*);
  virtual ~Viewer();
  
protected:
	virtual void draw();
  virtual void drawMenu();
  virtual void drawGame();
  virtual void gotEvent( Engine::event );
  virtual void initializeGL();
  virtual void keyPressEvent( QKeyEvent* );
  virtual void keyReleaseEvent( QKeyEvent* );
  virtual void mouseMoveEvent( QMouseEvent* );
  virtual void mousePressEvent( QMouseEvent* );
  virtual void mouseReleaseEvent( QMouseEvent* );
  virtual void paintGL();
  virtual void refreshEditionMap();
  virtual void resizeGL(int, int);
  
  Engine& mEngine;
  treeD::Camera mGameCamera;
//  utils::SpriteCatalog& mSpriteCatalog;
  
  //pour edition
  treeD::Sprite mEditionMap;
  //treeD::Texture mBrick;
  //treeD::Texture mEditionGrid;
  //treeD::Texture mStart;
};


class MainWindow : public QMainWindow, public realisim::platform::Engine::Client
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow(){};
  
public slots:
  
protected slots:
	virtual void backgroundChanged(int);
	virtual void cellTypeChanged(int);

protected:
	virtual void gotEvent( realisim::platform::Engine::event );
  virtual void populateBackground();
  virtual void updateUi();
  
	realisim::platform::Engine mEngine;
  realisim::platform::Viewer* mpViewer;
  QFrame* mpEditionPanel;
  QComboBox* mpCellType;
  QComboBox* mpBackground;
};

#endif
