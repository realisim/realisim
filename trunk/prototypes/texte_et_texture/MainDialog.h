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
#include "3d/Text.h"
#include "3d/Texture.h"
#include "3d/Widget3d.h"
#include <QtGui>


class Viewer : public realisim::treeD::Widget3d
{
public:
  Viewer(QWidget*);
  ~Viewer();

	void changeTextureMinificationInterpolation();
  void changeTextureMagnificationInterpolation();
  GLenum getTexMinInterpolation() const { return mTex.getMinificationFilter(); }
  GLenum getTexMagInterpolation() const { return mTex.getMagnificationFilter(); }
  virtual void generateMipmap();
  
private:
  virtual void initializeGL();
  virtual void paintGL();
  
  realisim::treeD::Texture mTex;
  realisim::treeD::Text mText;
};


class MainDialog : public QMainWindow
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog(){};
  
public slots:
	void generateMipmap();
	void minInterpolationClicked();
  void magInterpolationClicked();
            
protected:
	virtual void updateUi();
  
  Viewer* mpViewer;
  QPushButton* mpMinInterpolation;
  QPushButton* mpMagInterpolation;
};

#endif
