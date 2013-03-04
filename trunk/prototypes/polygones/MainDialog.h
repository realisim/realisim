/*
 *  MainWindow.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 08/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MainDialog_hh
#define MainDialog_hh

#include <QMainWindow>
#include "3d/Widget3d.h"
#include "math/Polygon.h"

class QComboBox;
class QFrame;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSlider;
class QString;
class QTimerEvent;
class Viewer;
namespace realisim { namespace treeD { class Texture; } }

class Data
{
public:
	Data() : //mPoints(),
  mPolygons() {};
  std::vector< realisim::math::Point3d > mPoints;
  std::vector< realisim::math::Polygon > mPolygons;
};

class Viewer : public realisim::treeD::Widget3d
{
public:
	Viewer( QWidget* );
  virtual ~Viewer();
  
protected:
	enum mode{ mCamera, mAddPoint, mEditPoint };
  virtual void drawPolygon( const realisim::math::Polygon& ) const;
  virtual void drawSceneForPicking() const;
  virtual realisim::math::Polygon& getLastPolygon();
	virtual void initializeGL();
  virtual void keyPressEvent( QKeyEvent* );
  virtual void keyReleaseEvent( QKeyEvent* );
  virtual void mouseMoveEvent( QMouseEvent* e );
  virtual void mousePressEvent( QMouseEvent* e );
  virtual void mouseReleaseEvent( QMouseEvent* e );
  virtual void paintGL();
  virtual void timerEvent( QTimerEvent* );
  
  int mTimerId;
  Data mData;
  mode mMode;
  int mSelection;
};
//------------------------------------------------------------------------------
class MainDialog : public QMainWindow
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog(){};
  
protected slots:
	virtual void updateUi();
                
protected:
	virtual void createUi();
  
  Viewer* mpViewer;
};

#endif
