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
#include "3d/Particules.h"

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

class Viewer : public realisim::treeD::Widget3d
{
public:
	Viewer( QWidget*, const std::vector< realisim::treeD::Particules >& );
  virtual ~Viewer();
  
protected:
	virtual void initializeGL();
  virtual void paintGL();
  virtual void timerEvent( QTimerEvent* );
  
  const std::vector< realisim::treeD::Particules >& mParticules;
  int mTimerId;
};

//------------------------------------------------------------------------------
class MainDialog : public QMainWindow
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog(){};
  
protected slots:
	virtual void addClicked();
  virtual void colorClicked();
  virtual void lifeLowerRangeChanged( const QString& );
  virtual void lifeUpperRangeChanged( const QString& );
  virtual void numberOfParticulesChanged( const QString& );
  virtual void radiusChanged( int );
  virtual void rateChanged( int );
  virtual void removeClicked();
  virtual void sizeChanged( int );
  virtual void typeChanged( int );
  virtual void updateUi();
  virtual void velocityLowerRangeChanged( const QString& );
  virtual void velocityUpperRangeChanged( const QString& );
                
protected:
	virtual void createUi();
  virtual realisim::treeD::Particules& getSelectedSource();
  
  Viewer* mpViewer;
  std::vector< realisim::treeD::Particules > mParticules;
  static realisim::treeD::Particules mDummyParticules;
  
  QListWidget* mpParticules;
  QPushButton* mpColorButton;
  QLineEdit* mpNumberOfParticules;
  QComboBox* mpType;
  QLineEdit* mpLifeLowerBound;
  QLineEdit* mpLifeUpperBound;
  QLineEdit* mpVelocityLowerBound;
  QLineEdit* mpVelocityUpperBound;
  QSlider* mpRadiusSlider;
  QSlider* mpRateSlider;
  QSlider* mpSizeSlider;
};

#endif
