/*
 *  MainWindow.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 08/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "3d/Camera.h"
#include "3d/Texture.h"
#include "3d/Utilities.h"
#include "math/Matrix4x4.h"
#include "math/Point.h"
#include "math/MathUtils.h"
#include "MainDialog.h"
#include <QKeyEvent>
#include <qlayout.h>
#include <QFile.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const float kInterval = 0.016; //interval de rafraichissement en sec
static const int kNumVirus = 1000;
static const int kNumAnim = 50;
static const float kSpeed = 3.0; //unités par seconde
static const int kMaxLife = 600;


using namespace realisim;
  using namespace math;
  using namespace treeD;
  
namespace
{
	class VirusZSorter
  {
  public:
  	VirusZSorter(const Matrix4d& i) : mMatrix(i) {}
    bool operator() (Virus* iLhs, Virus* iRhs) 
    {
      Point3d a = iLhs->getPos() * mMatrix;
      Point3d b = iRhs->getPos() * mMatrix;
      return a.getZ() < b.getZ();
    }
    
  private:  
    const Matrix4d& mMatrix;
  };
}

//--- Virus ------------------------------------------------------------------  
Virus::Virus() : mPos(rand() % 40 * (rand() % 2 == 0 ? 1.0 : -1.0),
  0.0,
  rand() % 40 * (rand() % 2 == 0 ? 1.0 : -1.0)),
  mLife(),
  mTeam(rand() % 2 == 0 ? tA : tB)
{ setLife(rand() % kMaxLife); }

Virus::Virus(const Virus& iV) : mPos(iV.getPos()), mLife(iV.getLife())
{}

Virus& Virus::operator=(const Virus& iV)
{
	mPos = iV.getPos();
  mLife = iV.getLife();
  return *this;
}

//--- Viewer ------------------------------------------------------------------
Viewer::Viewer(QWidget* ipParent /*=0*/) : Widget3d(ipParent),
 mViruses(),
// mDeadViruses(),
 mBackground(),
 mTexture(),
 mVirusesToAnim(),
 mAnimTimer(0)
{
  mAnimTimer = startTimer(kInterval * 1000);
  setFocusPolicy(Qt::StrongFocus);
}

Viewer::~Viewer()
{}

void Viewer::initializeGL()
{
  Widget3d::initializeGL();
  
  Texture t;
  t.set( QImage(":/images/fond combat.png") );
  mBackground.setTexture(t);
  //mBackground.setTexture(Texture(QImage(":/images/virus2.png")), QRect(128, 0, 64, 128));
  mBackground.setFullScreen(true);

  mTexture.set(QImage(":/images/virus2.png"));
  mTexture.generateMipmap();
  mTexture.setMinificationInterpolation( GL_LINEAR_MIPMAP_LINEAR );
  
  mViruses.reserve(kNumVirus);
  for(int i = 0; i < kNumVirus; ++i)
  {
  	Virus* v = new Virus();
  	mViruses.push_back(v);
    
    Sprite s;
    s.setTexture(mTexture, QRect(0,0,64,1023));
    s.startAnimation(600 + (rand() % 600), true, QSize(64, 64));
    s.setAnchorPoint(Sprite::aCenter);
  	mVirusesToAnim[v] = s;
  }
}

//-----------------------------------------------------------------------------
void Viewer::keyPressEvent(QKeyEvent* ipE)
{
	switch (ipE->key())
  {
    case Qt::Key_P:
      killTimer(mAnimTimer);
      break;
    case Qt::Key_R:
      mAnimTimer = startTimer(kInterval * 1000);;
      break;
    default:
      break;
  }
}

//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
  Widget3d::paintGL();
  
  glColor3ub(255, 255, 255);
  mBackground.draw(getCamera());
  
  zSort();
//#ifndef NDEBUG
//printf("-------------\n");
//for(int i = 0; i < kNumVirus; ++i)
//{
//	const Camera& c = getCamera();
//  Matrix4d m;
//  Vector3d invLook(c.getLook(), c.getPos());
//  invLook.normalise();
//  m.setRow1(c.getLat().getX(), c.getLat().getY(), c.getLat().getZ(), 0);
//  m.setRow2(c.getUp().getX(), c.getUp().getY(), c.getUp().getZ(), 0);
//  m.setRow3(invLook.getX(), invLook.getY(), invLook.getZ(), 0);	
//  m.setRow4(c.getPos().getX(), c.getPos().getY(), c.getPos().getZ(), 1.0);
//
//  Point3d a = mViruses[i]->getPos() * m.inverse();
//  
//  printf("zPos: %2.2f\n", a.getZ());
//}
//#endif
  
  Matrix4d m;
  const Camera& c = getCamera();
  Vector3d invLook(c.getLook(), c.getPos());
  invLook.normalise();
  m.setRow1(c.getLat().getX(), c.getLat().getY(), c.getLat().getZ(), 0);
  m.setRow2(c.getUp().getX(), c.getUp().getY(), c.getUp().getZ(), 0);
  m.setRow3(invLook.getX(), invLook.getY(), invLook.getZ(), 0);

  for(unsigned int i = 0; i < mViruses.size(); ++i)
  {
  	glPushMatrix();
    glTranslated(mViruses[i]->getPos().getX(),
      mViruses[i]->getPos().getY(),
      mViruses[i]->getPos().getZ());
    glMultMatrixd(m.getPtr());
    
    switch (mViruses[i]->getTeam()) 
    {
      case Virus::tA:
        glColor3ub(255, 115, 0);
        break;
      case Virus::tB:
        glColor3ub(115, 0, 220);
        break;
      default:
        break;
    }    
    mVirusesToAnim[mViruses[i]].draw(getCamera());
    glPopMatrix();
  }

	glColor3ub(0, 0, 0);
  showFps();
  
//  printf("num virus: %d\n", mViruses.size());
}

//-----------------------------------------------------------------------------
void Viewer::timerEvent(QTimerEvent* ipEvent)
{
	
  Point3d offset(0.0);
  std::vector<Virus*>::iterator it = mViruses.begin();
  for(; it != mViruses.end();)
  {    
    Virus* v = *it;
    int life = v->getLife();
    if(--life == 0)
    {
    	v->setLife(-1);
      //on change l'anim du virus
      //l'anim de mort
      Sprite sDeath;
      sDeath.setTexture(mTexture, QRect(64,0,127,1023));
      sDeath.startAnimation(800, false, QSize(64, 64));
      //sDeath.setTexture(mTexture, QRect(128,0,127,1023));
      //sDeath.startAnimation(1200, false, QSize(64, 128));
      sDeath.setAnchorPoint(Sprite::aCenter);
      mVirusesToAnim[v] = sDeath;
      
      //on cree un nouveau virus
      Virus* newV = new Virus();
    	it = mViruses.insert(it, newV);
      
      //on lui fait une nouvelle animation
      Sprite s;
      s.setTexture(mTexture, QRect(0,0,64,1023));
      s.startAnimation(600 + (rand() % 600), true, QSize(64, 64));
      s.setAnchorPoint(Sprite::aCenter);
  	  mVirusesToAnim[newV] = s;
    }
    else if(life < 0) //le virus est mort et on l'anime
    {
    	/*L'animation de mort est terminée, on enleve l'anim de la
        map, on enleve le virus de la liste et on delete le pointeur.*/
      if(mVirusesToAnim[v].getState() == Sprite::sAnimatingDone)
      {
        mVirusesToAnim.erase(v);
        it = mViruses.erase(it);
        delete v;
        v = 0;
      }
      else
      { ++it; }
    }
    else
    {
	    v->setLife(life);
      
      int dir = rand() % 4; //0 to 3
      if(dir == 0)
        offset.setXYZ(0.0, 0.0, 1.0); //nord
      else if(dir == 1)
        offset.setXYZ(0.0, 0.0, -1.0);  //sud
      else if(dir == 2)
        offset.setXYZ(1.0, 0.0, 0.0); //est
      else
        offset.setXYZ(-1.0, 0.0, 0.0);  //ouest
      
      Virus* v = *it;
      v->setPos(v->getPos() + offset * kSpeed * kInterval);
    
      ++it;
    }
  }
  
  updateGL();
}

//-----------------------------------------------------------------------------
void Viewer::zSort()
{
	//on fait une matrice avec la camera et on l'inverse afin de pouvoir
  //transformer toutes les positions des virus en coordonnéé de caméra
  //et ensuite faire un sort en z.
  const Camera& c = getCamera();
  Matrix4d m;
  Vector3d invLook(c.getLook(), c.getPos());
  invLook.normalise();
  m.setRow1(c.getLat().getX(), c.getLat().getY(), c.getLat().getZ(), 0);
  m.setRow2(c.getUp().getX(), c.getUp().getY(), c.getUp().getZ(), 0);
  m.setRow3(invLook.getX(), invLook.getY(), invLook.getZ(), 0);	
  m.setRow4(c.getPos().getX(), c.getPos().getY(), c.getPos().getZ(), 1.0);
  m.inverse();
  
	std::sort(mViruses.begin(), mViruses.end(), VirusZSorter(m));
}

//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QMainWindow(),
  mpViewer(0)
{
  srand(time(NULL));
  resize(800, 600);
  
  QHBoxLayout* pLyt = new QHBoxLayout(this);
  pLyt->setMargin(5);
  mpViewer = new Viewer(this);
  mpViewer->setCameraOrientation(Camera::FREE);
  pLyt->addWidget(mpViewer, 1);
  setCentralWidget(mpViewer);
  
//  Camera c = mpViewer->getCamera();
//  Matrix4d m;
//  m.setTranslation(Point3d(8.0, 0.0, 0.0));
//  c.setTransformationToGlobal(m);
//  mpViewer->setCamera(c, false);
}