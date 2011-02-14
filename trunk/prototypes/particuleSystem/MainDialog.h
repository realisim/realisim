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
class QKeyEvent;
class QLineEdit;
class QTimerEvent;
#include "3d/FrameBufferObject.h"
#include "3d/Shader.h"
#include "3d/Texture.h"
#include "3d/Widget3d.h"
#include "math/PlatonicSolid.h"
#include "ParticuleSystem.h"

class QFrame;
class QLabel;
class QLineEdit;
class QListWidget;
class QComboBox;
class QCheckBox;
class QPushButton;
class QRadioButton;
class QString;
class Viewer;
namespace realisim { namespace treeD { class Texture; } }

class MainDialog : public QMainWindow
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog(){};
  
public slots:
	void addGravityHole();
	void cameraModeChanged(int);
  void computingModeChanged(int);
  void decayEnabledClicked(bool);
  void gravityHoleSelected(unsigned int);
  void gravityHoleSelectionChanged(int);
  void gravityHoleForceChanged(const QString&);
  void hidePanel(bool);
  void maxInitialLifeChanged(const QString&);
  void maxInitialVelocityChanged(const QString&);
  void maxParticulesChanged(const QString&);
  void removeGravityHole();
                
protected:
  Viewer* mpViewer;
  realisim::openCL::ParticuleSystem mParticuleSystem;
  
private:
	virtual void updateUi();
  virtual void timerEvent(QTimerEvent*);
  
  int mTimerId;
  QFrame* mpLeftPanel;
  QLineEdit* mpMaxParticules;
  QLineEdit* mpMaxInitialVelocity;
  QLineEdit* mpMaxInitialLife;
  QCheckBox* mpDecayEnabled;
  QRadioButton* mp2d;
  QRadioButton* mp3d;
  QComboBox* mpComputingMode;
  QListWidget* mpGravityHolesList;
  QLineEdit* mpGravityHoleForce;
};

#endif
