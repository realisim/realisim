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
class QKeyEvent;
class QLineEdit;
class QTimerEvent;
#include "3d/Shader.h"
#include "3d/Texture.h"
#include "3d/Widget3d.h"
#include "math/PlatonicSolid.h"


class Viewer : public realisim::treeD::Widget3d
{
public:
  Viewer(QWidget*);
  ~Viewer();

  virtual float getIsoSurface() const {return mIsoSurfaceValue;}
  virtual void setIsoSurface(float i) {mIsoSurfaceValue = i;}
private:
  virtual void initializeGL();
  void draw3dTextureCube();
  virtual void paintGL();
  virtual void keyPressEvent(QKeyEvent*);
  
  realisim::treeD::Shader mRayCastShader;
  realisim::treeD::Texture mCtTexture;
  realisim::treeD::Texture mHounsfieldLUT;
  
  float mDepth;
  float mIsoSurfaceValue;
  
};


class MainDialog : public QDialog
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog(){};
  
protected slots:
  virtual void isoSurfaceValueChanged(int);
                
protected:
  Viewer* mpViewer;
};

#endif
