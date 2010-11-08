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


class Viewer : public realisim::treeD::Widget3d
{
public:
  Viewer(QWidget*);
  ~Viewer();

	virtual void drawScene();
  virtual void drawGlowOverLay();

private:
  virtual void initDisplayList();
  virtual void initializeGL();
  void drawPlatonicSolid(const realisim::math::PlatonicSolid&);
  virtual void paintGL();
  virtual void resizeGL(int, int);
  virtual void timerEvent(QTimerEvent*);
  
  realisim::math::PlatonicSolid mPs;
  realisim::treeD::FrameBufferObject mFbo;
  realisim::treeD::Shader mBlurShader;
  realisim::treeD::Shader mOnlyOneTextureShader;
  realisim::treeD::Texture mTexture;
  realisim::treeD::Texture mTextureBlur;
  realisim::treeD::Texture mTexture2;
  realisim::treeD::Texture mTextureBlur2;
  GLuint mBoxDisplayList;
  GLuint mBoxDisplayList2;
};


class MainDialog : public QMainWindow
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
