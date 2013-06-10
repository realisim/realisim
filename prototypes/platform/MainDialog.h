

#ifndef MainDialog_hh
#define MainDialog_hh

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
  virtual void initializeEditing();
  virtual void initializeGL();
  virtual void keyPressEvent( QKeyEvent* );
  virtual void keyReleaseEvent( QKeyEvent* );
  virtual void paintGL();
  virtual void resizeGL(int, int);
  
  Engine& mEngine;
  treeD::Camera mGameCamera;
  
  //pour edition
  treeD::Sprite mTerrain;
};


class MainDialog : public QMainWindow, public realisim::platform::Engine::Client
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog(){};
  
public slots:
            
protected:
	virtual void gotEvent( realisim::platform::Engine::event );
	realisim::platform::Engine mEngine;
  realisim::platform::Viewer* mpViewer;
};

#endif
