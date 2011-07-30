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
#include "3d/Sprite.h"
#include "3d/Widget3d.h"
#include "math/Point.h"
#include <vector>

using namespace realisim;

class Virus
{
public:
	Virus();
  Virus(const Virus&);
  Virus& operator=(const Virus&);
  ~Virus(){;}
  
  enum team{tA, tB};
  const unsigned int getLife() const {return mLife;}
	const math::Point3d& getPos() const {return mPos;}
  const team getTeam() const {return mTeam;}
  void setLife(unsigned int i) {mLife = i;}
  void setPos(const math::Point3d& i) {mPos = i;}
  void setTeam(team i) {mTeam = i;}

protected:
	math::Point3d mPos;
  unsigned int mLife;
  team mTeam;
};

class Viewer : public treeD::Widget3d
{
public:
  Viewer(QWidget*);
  ~Viewer();

private:
  virtual void initializeGL();
  virtual void keyPressEvent(QKeyEvent*);
  virtual void paintGL();
  virtual void timerEvent(QTimerEvent*);
  virtual void zSort();
  
  std::vector<Virus*> mViruses;
  std::vector<Virus*> mDeadViruses;
  treeD::Sprite mBackground;
  treeD::Texture mTexture;
  std::map<Virus*, treeD::Sprite> mVirusesToAnim;
  int mAnimTimer;
//  std::vector<treeD::Sprite> mVirusesAnim;
//  std::vector<treeD::Sprite> mDeadVirusesAnim;
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
