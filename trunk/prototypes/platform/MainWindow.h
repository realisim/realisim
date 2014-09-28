

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
  virtual void draw( const GameEntity& );
  virtual void draw( const Animation& );
  virtual void drawDataMap();
  virtual void drawGame();
  virtual void drawMenu();
  virtual void gotEvent( Engine::event );
  virtual void initializeGL();
  virtual void keyPressEvent( QKeyEvent* );
  virtual void keyReleaseEvent( QKeyEvent* );
  virtual treeD::Camera makeLightCamera(int, const Point2d&, const Vector2d&, double) const;
  virtual void mouseMoveEvent( QMouseEvent* );
  virtual void mousePressEvent( QMouseEvent* );
  virtual void mouseReleaseEvent( QMouseEvent* );  
  virtual void paintGL();
  virtual treeD::Texture renderLights();
  virtual void resizeGL(int, int);
  virtual void wheelEvent( QWheelEvent* );
  
  Engine& mEngine;
	treeD::FrameBufferObject mFboLightDepth;
  treeD::FrameBufferObject mFboLightMask;
  treeD::Shader mShadowMapShader;
};


class MainWindow : public QMainWindow, public realisim::platform::Engine::Client
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow(){};
  
public slots:
  
protected slots:
	virtual void addLayerClicked();
	virtual void addSpriteToLayerClicked();
	virtual void backgroundChanged(int);
	virtual void cellTypeChanged(int);
  virtual void layerSelectionChanged(int);
  virtual void layerVisibilityChanged( QListWidgetItem* );
  virtual void newMapClicked();
  virtual void openMapClicked();
  virtual void removeLayerClicked();
  virtual void removeSpriteFromLayerClicked();
  virtual void spriteSelectionChanged(int, int);

protected:
	enum tabs{ tMap, tBackground, tActor };

	virtual void gotEvent( realisim::platform::Engine::event );
  virtual void populateBackground();
  virtual void populateLayers();
  virtual void populateSprites();
  virtual void updateUi();
  
	realisim::platform::Engine mEngine;
  realisim::platform::Viewer* mpViewer;
  QString mEditionPath;
  
  QWidget* mpEditionPanel;
  QWidget* mpCellTypeFrame;
  QComboBox* mpCellType;
  QComboBox* mpBackground;
  QListWidget* mpLayers;
  QTableWidget* mpSprites;	
};

#endif
