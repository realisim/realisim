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

#include <QCheckBox>
#include <QDialog>
#include <QSpinBox>
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
  virtual ~Viewer();
  
  enum filterType { fNone, fMean, fGausian, fEdgeDetection, fSharpening,
  fEdgeDetection3d, fSharpening3d, fColorize };

  virtual const filterType getFilterType() const {return mFilterType;}
	virtual void setFilterType(filterType i);

private:
  virtual void initializeGL();
  void drawSlice();
  virtual void paintGL();
  virtual void keyPressEvent(QKeyEvent*);
  
  class ConvolutionFilter
  {
  public:
    explicit ConvolutionFilter(int);
    ~ConvolutionFilter();
    
    int mFilterSize;
    float* mKernelValues;
  };
  
  realisim::treeD::Shader mConvolutionShader;
  realisim::treeD::Shader mActiveShader;
  realisim::treeD::Shader mDefaultShader;
  realisim::treeD::Shader mColorizeShader;
  realisim::treeD::Texture mCtTexture;
  realisim::treeD::Texture mUltraSoundTexture;
  filterType mFilterType;
  ConvolutionFilter mFilter;
};


class MainDialog : public QDialog
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog(){};
  
protected slots:
  virtual void addOnBaseChanged(int);
  virtual void filterTypeChanged(int);
  virtual void scaleFactorChanged(int);
  virtual void showCtChanged(int);
  virtual void sliceChanged(int);
  virtual void viewChanged(int);
  virtual void windowingMinChanged(int);
  virtual void windowingMaxChanged(int);
                
protected:
  void refreshTexCoord();
  
  Viewer* mpViewer;
};

#endif
