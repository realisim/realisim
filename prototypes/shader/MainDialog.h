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

#include <QDialog>
class QLineEdit;
class QTimerEvent;
#include "3d/Shader.h"
#include "3d/Widget3d.h"
#include "math/PlatonicSolid.h"


class Viewer : public realisim::treeD::Widget3d
{
public:
  Viewer(QWidget*);
  ~Viewer();

private:
  virtual void initializeGL();
  void drawPlatonicSolid(const realisim::math::PlatonicSolid&);
  virtual void paintGL();
  virtual void timerEvent(QTimerEvent*);
  
  realisim::math::PlatonicSolid mPs;
  realisim::treeD::Shader mShader;
  realisim::treeD::Shader mShader2;
  
};


class MainDialog : public QDialog
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog(){};
  
public slots:

                
protected:
  Viewer* mpViewer;
};

#endif
