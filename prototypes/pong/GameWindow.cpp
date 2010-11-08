//

#include <algorithm>
#include "GameObject.h"
#include "GameWindow.h"
#include <math.h>
#include "math/MathUtils.h"
#include <qapplication.h>
#include <qcheckbox.h>
#include <QKeyEvent>
#include <qlabel.h>
#include <qlineedit.h>
#include <QCloseEvent>
#include <qlayout.h>
#include <QMouseEvent>
#include <qpushbutton.h>
#include <QTimerEvent>

using namespace realisim;
  using namespace treeD;
  using namespace math;
using namespace Pong;

namespace
{
  const int kTimeStep = 16; //16 ms -> roughly 60 frames / sec.
  const double kcpTimeStep = kTimeStep / 1000.0;
  const Vector3d kHolderPos(0.0, -3.0, 0.0);
  const QRectF kPlayerRegion(QPointF(-1, 0.5), QPointF(1, -0.5)); //in meters
  
  const quint16 kNetworkPort = 12345;
}

//--- OptionsDialog ------------------------------------------------------------
OptionsDialog::OptionsDialog(QWidget* i) : QDialog(i)
{
  QVBoxLayout* pMainLyt = new QVBoxLayout(this);
  pMainLyt->setSpacing(5);
  
  mpFullScreen = new QCheckBox(this);
  mpFullScreen->setText("Fullscreen");
  
  //network
  QHBoxLayout* pHostGameLyt = new QHBoxLayout(this);
  mpHostGame = new QCheckBox(this);
  mpHostGame->setText("Host game");
  mpServerAddress = new QLabel(this);
  mpServerAddress->setText(" (Server not started)");
  pHostGameLyt->addWidget(mpHostGame);
  pHostGameLyt->addWidget(mpServerAddress);
  
  QHBoxLayout* pJoinGameLyt = new QHBoxLayout(this);
  mpJoinGame = new QCheckBox(this);
  mpJoinGame->setText("Join game");
  mpHostServerAddress = new QLineEdit(this);
	mpHostServerAddress->setText(mOptions.mHostServerAddress);
  pJoinGameLyt->addWidget(mpJoinGame);
  pJoinGameLyt->addWidget(mpHostServerAddress);
  
  //ok-cancel
  QPushButton* pOk = new QPushButton(this);
  pOk->setText("Ok");
  QHBoxLayout* pOkCancelLyt = new QHBoxLayout(this);
  pOkCancelLyt->setSpacing(5);
  pOkCancelLyt->addStretch(1.0);
  pOkCancelLyt->addWidget(pOk);
  //pOkCancelLyt->addWidget(pCancel);
    
  pMainLyt->addWidget(mpFullScreen);
  pMainLyt->addLayout(pHostGameLyt);
  pMainLyt->addLayout(pJoinGameLyt);
  pMainLyt->addStretch(1);
  pMainLyt->addLayout(pOkCancelLyt);
  
  
  connect(mpFullScreen, SIGNAL(toggled(bool)),
    this, SIGNAL(fullScreenOptionChanged(bool)));
  connect(mpHostGame, SIGNAL(toggled(bool)),
    this, SIGNAL(hostingGameOptionChanged(bool )));
  connect(mpJoinGame, SIGNAL(toggled(bool)),
    this, SIGNAL(joinGameOptionChanged(bool )));
  connect(pOk, SIGNAL(clicked()),
    this, SIGNAL(hideOptions()));
  connect(pOk, SIGNAL(clicked()),
    this, SLOT(accept()));
//  connect(pCancel, SIGNAL(clicked()),
//    this, SLOT(reject()));
}

QString OptionsDialog::getHostServerAddress() const
{ return mpHostServerAddress->text(); }

void OptionsDialog::setServerAddress(QString iT)
{ mpServerAddress->setText("(" + iT + ")"); }

//--- Board --------------------------------------------------------------------
Board::Board() : mSize(0.0, 0.0),
  mPlayerTransformations(),
  mNetTransformations()
{}

void Board::draw() const
{
  double w = getSize().width() / 2.0;
  double h = getSize().height() / 2.0;
  glPushAttrib(GL_CURRENT_BIT | GL_POLYGON_BIT);
  glColor3ub(255, 255, 255);
  glPolygonMode(GL_FRONT, GL_LINE);
  glBegin(GL_QUADS);
  glVertex3d(-w, -h, 0);
  glVertex3d( w, -h, 0);
  glVertex3d( w,  h, 0);
  glVertex3d(-w,  h, 0);
  glEnd();
  glPopAttrib();
}

Board2::Board2() : Board()
{
  setSize(5.0, 20.0);
  
  //player 0 bas
  Matrix4d pt;
  pt.setTranslation(Point3d(0.0, -9.5, 0.0));
  mPlayerTransformations.push_back(pt);
  //player 1 haut
  pt = getRotationMatrix(180 * kDegreeToRadian,
    Vector3d(0.0, 0.0, 1.0));
  pt.setTranslation(Point3d(0.0, 9.5, 0.0));
  mPlayerTransformations.push_back(pt);

  //net 0 bas
  Matrix4d nt;
  nt.setTranslation(Point3d(0.0, -10.0, 0.0));
  mNetTransformations.push_back(nt);
  //net 1 haut
  nt = getRotationMatrix(180 * kDegreeToRadian,
    Vector3d(0.0, 0.0, 1.0));
  nt.setTranslation(Point3d(0.0, 10.0, 0.0));
  mNetTransformations.push_back(nt);
}

Board4::Board4() : Board()
{
  setSize(10.0, 10.0);
  
  //player 0 bas
  Matrix4d pt;
  pt.setTranslation(Point3d(0.0, -4.5, 0.0));
  mPlayerTransformations.push_back(pt);
  //player 1 haut
  pt = getRotationMatrix(180 * kDegreeToRadian,
    Vector3d(0.0, 0.0, 1.0));
  pt.setTranslation(Point3d(0.0, 4.5, 0.0));
  mPlayerTransformations.push_back(pt);
  //player 2 gauche
  pt = getRotationMatrix(-90 * kDegreeToRadian,
    Vector3d(0.0, 0.0, 1.0));
  pt.setTranslation(Point3d(-4.5, 0.0, 0.0));
  mPlayerTransformations.push_back(pt);
  //player 3 droite
  pt = getRotationMatrix(90 * kDegreeToRadian,
    Vector3d(0.0, 0.0, 1.0));
  pt.setTranslation(Point3d(4.5, 0.0, 0.0));
  mPlayerTransformations.push_back(pt);

  //net 0 bas
  Matrix4d nt;
  nt.setTranslation(Point3d(0.0, -5.0, 0.0));
  mNetTransformations.push_back(nt);
  //net 1 haut
  nt = getRotationMatrix(180 * kDegreeToRadian,
    Vector3d(0.0, 0.0, 1.0));
  nt.setTranslation(Point3d(0.0, 5.0, 0.0));
  mNetTransformations.push_back(nt);
  //net 2 gauche
  nt = getRotationMatrix(-90 * kDegreeToRadian,
    Vector3d(0.0, 0.0, 1.0));
  nt.setTranslation(Point3d(-5.0, 0.0, 0.0));
  mNetTransformations.push_back(nt);
  //net 3 droite
  nt = getRotationMatrix(90 * kDegreeToRadian,
    Vector3d(0.0, 0.0, 1.0));
  nt.setTranslation(Point3d(5.0, 0.0, 0.0));
  mNetTransformations.push_back(nt);
}

//--- Physics ------------------------------------------------------------------
GameWindow* Physics::mpGameWindow = 0;
vector<cpBody*> Physics::mNets;
Physics::Physics(GameWindow* ipGW, const Board& iB, vector<Ball*>& iBall, 
  vector<Net*>& iN, vector<Player*>& iP) : 
  mpSpace(0),
  mpBoard(0),
  mPlayers(),
  mBalls(),
  mShapes(),
  mDataBalls(iBall),
  mDataNets(iN),
  mDataPlayers(iP)
{
  mpGameWindow = ipGW;
  
  //-- space
  mpSpace = cpSpaceNew();
  mpSpace->iterations = 5;
  mpSpace->elasticIterations = 5;
//mpSpace->gravity = cpv(3.0f, -9.8f);
//mpSpace->damping = 0.4;
  cpSpaceResizeStaticHash(mpSpace, 2.0f, 5);
  cpSpaceResizeActiveHash(mpSpace, 0.1f, 100);
  
  //-- board
  //the board is a static object, so infinite mass and infinite inertia
  mpBoard = cpBodyNew(INFINITY, INFINITY);
  double w = iB.getSize().width() / 2.0;
  double h = iB.getSize().height() / 2.0;

  cpVect boardVerts[] = {
    cpv(-w, h),
    cpv( w, h),
    cpv( w, -h),
    cpv(-w, -h),
  };

  cpShape* pSeg;
  for(int i = 0; i < 4; ++i)
  {
    pSeg = cpSegmentShapeNew(mpBoard, boardVerts[i], boardVerts[i+1], 0.0 );
    if(i == 3)
      pSeg = cpSegmentShapeNew(mpBoard, boardVerts[i], boardVerts[0], 0.0 );
    
    pSeg->u = 0.1f;
    pSeg->e = 1.0f;
    pSeg->collision_type = ctBoard;
    //add the static shape associate with the Board body object
    cpSpaceAddStaticShape(mpSpace, pSeg);
    mShapes.push_back(pSeg);
  }
  
  //one net per player
  for(unsigned int i = 0; i < mDataNets.size(); ++i)
  {
    w = mDataNets[i]->getSize().width() / 2.0;
    h = mDataNets[i]->getSize().height() / 2.0;
    //on met les coordonné du net en coordonné global.
    Point3d v0 = Point3d(-w, h, 0.0) * mDataNets[i]->getTransformationToGlobal();
    Point3d v1 = Point3d( w, h, 0.0) * mDataNets[i]->getTransformationToGlobal();
    Point3d v2 = Point3d( w, -h, 0.0) * mDataNets[i]->getTransformationToGlobal();
    Point3d v3 = Point3d(-w, -h, 0.0) * mDataNets[i]->getTransformationToGlobal();

    cpVect netVerts[] = {
      cpv(v0.getX(), v0.getY()),
      cpv(v1.getX(), v1.getY()),
      cpv(v2.getX(), v2.getY()),
      cpv(v3.getX(), v3.getY()),
    };
    
    cpBody* pNet = cpBodyNew(0, 0);
    mNets.push_back(pNet);
    
    cpShape* pShapeNet = cpPolyShapeNew(pNet, 4, netVerts, cpvzero);
    pShapeNet->collision_type = ctNet;
    cpSpaceAddStaticShape(mpSpace, pShapeNet);
    mShapes.push_back(pShapeNet);
  }

  //-- ball
  for(unsigned int i = 0; i < mDataBalls.size(); ++i)
  {
    cpFloat wheelMoment = cpMomentForCircle(mDataBalls[i]->getMass(), 
      mDataBalls[i]->getRadius(), 0.0f, cpvzero);
    cpBody* pBall = cpBodyNew(mDataBalls[i]->getMass(), wheelMoment);
    pBall->p = cpv(mDataBalls[i]->getPosition().getX(),
      mDataBalls[i]->getPosition().getY());
    pBall->velocity_func = &ballVelocityFunc;
    cpSpaceAddBody(mpSpace, pBall);
    mBalls.push_back(pBall);
    
    cpShape* pBallShape = cpCircleShapeNew(pBall, mDataBalls[i]->getRadius(), cpvzero);
    pBallShape->u = 0.2f;
    pBallShape->e = 1.0;
    pBallShape->collision_type = ctBall;
    cpSpaceAddShape(mpSpace, pBallShape);
    mShapes.push_back(pBallShape);
  }
  
  //-- player
  for(unsigned int i = 0; i < mDataPlayers.size(); ++i)
  {
    double w = mDataPlayers[i]->getSize().width() / 2.0;
    double h = mDataPlayers[i]->getSize().height() / 2.0;
    
    //on met les coordonné du player en global
    Vector3d v0 = Vector3d(-w, h, 0.0) * mDataPlayers[i]->getTransformationToGlobal();
    Vector3d v1 = Vector3d( w, h, 0.0) * mDataPlayers[i]->getTransformationToGlobal();
    Vector3d v2 = Vector3d( w, -h, 0.0) * mDataPlayers[i]->getTransformationToGlobal();
    Vector3d v3 = Vector3d(-w, -h, 0.0) * mDataPlayers[i]->getTransformationToGlobal();

    cpVect playerVerts[] = {
      cpv(v0.getX(), v0.getY()),
      cpv(v1.getX(), v1.getY()),
      cpv(v2.getX(), v2.getY()),
      cpv(v3.getX(), v3.getY()),
    };

//    cpFloat playerMoment = cpMomentForPoly(mDataPlayers[i]->getMass(), 4,
//      playerVerts, cpvzero);
    cpBody* pPlayer = cpBodyNew(mDataPlayers[i]->getMass(), INFINITY);
    pPlayer->p = cpv(mDataPlayers[i]->getPosition().getX(),
      mDataPlayers[i]->getPosition().getY());
    cpSpaceAddBody(mpSpace, pPlayer);
    mPlayers.push_back(pPlayer);
    
    cpShape* pShapePlayer = cpPolyShapeNew(pPlayer, 4, playerVerts, cpvzero);
    pShapePlayer->u = 5.0f;
    pShapePlayer->e = 1.0f;
    pShapePlayer->collision_type = ctPlayer;
    cpSpaceAddShape(mpSpace, pShapePlayer);
    mShapes.push_back(pShapePlayer);
  }
  
  //Ajout des call backs de collision
  cpSpaceSetDefaultCollisionPairFunc(mpSpace, &defaultCollisionFunc, NULL);
  cpSpaceAddCollisionPairFunc(mpSpace, ctBall, ctNet, &ballToNetCollisionFunc, NULL);
}

Physics::~Physics()
{
  cpBodyFree(mpBoard);
  for(unsigned int i = 0; i < mBalls.size(); ++i)
    cpBodyFree(mBalls[i]);
  for(unsigned int i = 0; i < mPlayers.size(); ++i)
  {
    cpBodyFree(mPlayers[i]);
    cpBodyFree(mNets[i]);
  }
  //free all of the shapes, bodies and joints that have been added to space
  cpSpaceFreeChildren(mpSpace);
  cpSpaceFree(mpSpace);
  mpSpace = 0;
}

int Physics::ballToNetCollisionFunc(cpShape* iA, cpShape* iB,
  cpContact* ipContacts, int iNumContacts, cpFloat iNormalCoef, void* ipData)
{
  vector<cpBody*>::iterator it = find(mNets.begin(), mNets.end(), iB->body);
  if(it != mNets.end())
    //cout<<"Ball in net of player: "<<distance(mNets.begin(), it)<<endl;
    mpGameWindow->score(distance(mNets.begin(), it));
  return 1;
}

void Physics::ballVelocityFunc(cpBody* ipBody, cpVect iGravity,
  cpFloat iDamping, cpFloat iDt)
{
  cpBodyUpdateVelocity(ipBody, iGravity, iDamping, iDt);
  double kw = 150; //vitesse maximal angulaire qui peut induire une deviation
  //influence angulaire normalisé
  double nw = min(kw, fabs(ipBody->w)) / kw;
  double kv = 60;
  double nv = min(kv, cpvlength(ipBody->v)) / kv;
  double d = 0.03; //magnitude maximale du vecteur de déviation
  //trouvons le vecteur laterale a la velocity
  Vector3d lat = 
    Vector3d(ipBody->v.x, ipBody->v.y, 0.0) ^ Vector3d(0.0, 0.0, 1.0);
  lat.normalise();
  cpVect cpLat = cpv((cpFloat)lat.getX(), (cpFloat)lat.getY());
  cpLat = ipBody->w <= 0 ? cpLat : cpvmult(cpLat, -1.0);
  ipBody->v = cpvadd(ipBody->v, cpvmult(cpLat, nw * d));
  
//  //faire decroitre la rotation angulaire
//  if(abs(ipBody->w) >= 20.0)
//    ipBody->w += iDt * 20.0 * ipBody->w >= 0.0 ? -1 : 1;

//  cout << "dt: " << iDt << endl;
//  cout << "Angular velocity: " << ipBody->w << endl;
//  cout << "velocity: " << cpvlength(ipBody->v) << endl;
//  cout << "velocity vect: " << ipBody->v.x << " " << ipBody->v.y << endl;
//  cout << "velocity lat: " << cpLat.x << " " << cpLat.y << endl;
//  cout << "Influence angulaire normalisé: " << nw << endl;
//  cout << "Influence vitesse normalisé: " << nv << endl;
//  cout << "Influence sur la déviation: " << nw * nv << endl << endl;
}

/*Pour chaque collision, on va mettre des flameches!*/
int Physics::defaultCollisionFunc(cpShape* iA, cpShape* iB,
  cpContact* ipContacts, int iNumContacts, cpFloat iNormalCoef, void* ipData)
{  
  /*Returning 0 will cause the collision to be discarded. This allows you to 
    do conditional collisions.*/
  cpVect p = ipContacts[0].p;
  cpVect n = ipContacts[0].n;
  mpGameWindow->addCollision(Vector3d(p.x, p.y, 0.0),
    Vector3d(-n.x, -n.y, 0.0));
	return 1;
}

void Physics::drawCollisions()
{
  cpArrayEach(mpSpace->arbiters, &drawCollisionsFunc, NULL);
}

void
Physics::drawCollisionsFunc(void *ptr, void *data)
{
  glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POINT_BIT);
  glDisable(GL_DEPTH_TEST);
  glPointSize(3.0);
  glColor3ub(255 ,0 , 0);
  glBegin(GL_POINTS);
	cpArbiter *arb = (cpArbiter *)ptr;
	for(int i=0; i<arb->numContacts; i++){
		cpVect v = arb->contacts[i].p;
		glVertex2f(v.x, v.y);
	}
  glEnd();
  glPopAttrib();
}

Point3d Physics::getClosestBallPos(Player* p) const
{
  int closestBallId = 0;
  int id = distance(mDataPlayers.begin(),
      find(mDataPlayers.begin(), mDataPlayers.end(), p));
  cpVect pPos = mPlayers[id]->p;
  cpVect closestDistance = cpv(MAX_DOUBLE, MAX_DOUBLE);
  for(unsigned int i = 0; i < mBalls.size(); ++i)
  {
		if(cpvlength(cpvsub(mBalls[i]->p, pPos)) < cpvlength(closestDistance))
      closestBallId = i;
  }
  
  return Point3d(mBalls[closestBallId]->p.x,
    mBalls[closestBallId]->p.y, 0.0);
}

void Physics::removeMarkedPlayers()
{
  if(mPlayersToRemove.empty())
    return;
    
	for(unsigned int i = 0; i < mPlayersToRemove.size(); ++i)
  {
    int id = mPlayersToRemove[i];
//cpBody* pPlayer = mPlayers[id];
    
    //remove player
    removeShapes(mPlayers[id]);
    cpSpaceRemoveBody(mpSpace, mPlayers[id]);
    cpBodyFree(mPlayers[id]);
    mPlayers.erase(mPlayers.begin() + id);
    /*voir Note de la classe Physics pour la raison qui pousse Physics a 
      effacer les items de GameWindow*/
    mDataPlayers.erase(mDataPlayers.begin() + id);
    
    //remove nets
    removeStaticShapes(mNets[id]);
    cpSpaceRemoveBody(mpSpace, mNets[id]);
    cpBodyFree(mNets[id]);
    /*voir Note de la classe Physics pour la raison qui pousse Physics a 
      effacer les items de GameWindow*/
    mNets.erase(mNets.begin() + id);
    mDataNets.erase(mDataNets.begin() + id);
  }
  cpSpaceRehashStatic(mpSpace);
  mPlayersToRemove.clear();
}

void Physics::removeShapes(cpBody* b)
{
  vector<cpShape*>::iterator it = mShapes.begin();
  for(; it != mShapes.end();) 
  {
    if((*it)->body == b)
    {
      cpSpaceRemoveShape(mpSpace, *it);
      it = mShapes.erase(it);
    }
    else
      ++it;
  }
}

void Physics::removeStaticShapes(cpBody* b)
{
  vector<cpShape*>::iterator it = mShapes.begin();
  for(; it != mShapes.end();) 
  {
    if((*it)->body == b)
    {
      cpSpaceRemoveStaticShape(mpSpace, *it);
      it = mShapes.erase(it);
    }
    else
      ++it;
  }
}

void Physics::resetBalls()
{
  for(unsigned int i = 0; i < mDataBalls.size(); ++i)
  {
    mDataBalls[i]->setPosition(Point3d(0.0));
    mBalls[i]->v = cpv(0.0, 0.0);
    mBalls[i]->p = cpv(0.0, 0.0);
  }
}

void Physics::updatePhysics()
{  
  /*appel bodySlew pour calculer la force/vitesse de la palette, lors de la
    prochaine itération de physique, pour le déplacement effectuer par
    l'usagé*/
  for(unsigned int i = 0; i < mDataPlayers.size(); ++i)
    cpBodySlew(mPlayers[i],
      cpv(mDataPlayers[i]->getPosition().getX(),
      mDataPlayers[i]->getPosition().getY()), kcpTimeStep);
      
  int nbSteps = 10;
  //conversion en millisecondes...
  double timeStep = kcpTimeStep / nbSteps; 
  for(int i = 0; i < nbSteps; ++i)
  {
    cpSpaceStep(mpSpace, timeStep);
  }
 
  //update position of ball after physics
  for(unsigned int i = 0; i < mDataBalls.size(); ++i)
  {
    cpVect p = mBalls[i]->p;
    mDataBalls[i]->setPosition(Point3d((double)p.x, (double)p.y, 0.0));
    mDataBalls[i]->setAngle(mBalls[i]->a);
  }
  
  //update position of player after physics
  for(unsigned int i = 0; i < mDataPlayers.size(); ++i)
  {
    cpVect p = mPlayers[i]->p;
    mDataPlayers[i]->setPosition(Point3d((double)p.x, (double)p.y, 0.0));
    mDataPlayers[i]->setAngle(mPlayers[i]->a);
  }
  
  //remove bodies that need to be removed...
  removeMarkedPlayers();
}

//--- GameWindow ---------------------------------------------------------------
GameWindow::GameWindow(QWidget* ipParent /*= 0*/) : Widget3d(ipParent),
  mpOptionsDialog(0),
  mState(gsNotStarted),
  mGameTimerId(0),
  mPlayers(),
  mEliminatedPlayers(),
  mpLocalPlayer(0),
  mpBoard(0),
  mNets(),
  mMousePosition(0, 0),
  mBalls(),
  mpPhysics(0),
  mBackgroundSprite(),
  mGameAssets()
{
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
  
//  //set the camera
//  setCameraMode( Camera::PERSPECTIVE );
//  setCameraOrientation(Camera::FREE);
//  Camera c = getCamera();
//  c.set(Point3d(0.0, -15.0, 5.0), Point3d(0.0), Vector3d(0.0, 1.0, 0.0));
//  setCamera(c);
  setCameraMode( Camera::ORTHOGONAL );
  setCameraOrientation( Camera::XY );
  
  addPlayer(new Player("Computer1"));
  addPlayer(new Player("Computer2"));
//  addPlayer(new Player("Computer3"));
//  addPlayer(new Player("Computer4"));
  
  mpOptionsDialog = new OptionsDialog(this);
  //connect dialog to GameWindow
  connect(mpOptionsDialog, SIGNAL(fullScreenOptionChanged(bool)),
    this, SLOT(fullScreenOptionChanged(bool)));
  connect(mpOptionsDialog, SIGNAL(hostingGameOptionChanged(bool)),
    this, SLOT(hostingGameOptionChanged(bool)));
  connect(mpOptionsDialog, SIGNAL(joinGameOptionChanged(bool)),
    this, SLOT(joinGameOptionChanged(bool)));
  connect(mpOptionsDialog, SIGNAL(hideOptions()),
    this, SLOT(hideOptions()));
}

GameWindow::~GameWindow()
{
  delete mpPhysics;

  for(unsigned int i = 0; i < mPlayers.size(); ++i)
    delete mPlayers[i];
  for(unsigned int i = 0; i < mEliminatedPlayers.size(); ++i)
    delete mEliminatedPlayers[i];
  for(unsigned int i = 0; i < mNets.size(); ++i)
    delete mNets[i];
    
  delete mpBoard;
  
  for(unsigned int i = 0; i < mBalls.size(); ++i)
    delete mBalls[i];
  for(unsigned int i = 0; i < cMaxCollisions; ++i)
    delete mCollisions[i];
}

void GameWindow::addPlayer(Player* p)
{
  mPlayers.push_back(p);
  //each player comes with is net
  mNets.push_back(new Net());
}

void GameWindow::addCollision(const Vector3d& p, const Vector3d& n)
{
  Collision* c = 0;
  for(unsigned int i = 0; i < cMaxCollisions; ++i)
    if(mCollisions[i]->isDone())
    { c = mCollisions[i]; break; }

  if(c)
  {
    Matrix4d m;
    m.setTranslation(toPoint(p));
    c->setTransformationToGlobal(m);
    double a = atan2(n.getY(), n.getX());
    c->setAngle(a);
    c->setNormal(n);
    c->animate();
  }
}

void GameWindow::fullScreenOptionChanged(bool iIsFullScreen)
{
  if(iIsFullScreen)
		parentWidget()->showFullScreen();
  else
    parentWidget()->showNormal();
}

/*La position passé doit être en coordonné globale*/
bool GameWindow::canPlayerMoveTo(const Player*, const Point3d& iPos) const
{
//  Matrix4d inv(mPlayers[iId]->getTransformation());
//  inv.inverse();
  //cout << "Pos: " << iPos.getX() << " " << iPos.getY() << endl;
  return true;
}

void GameWindow::drawBalls() const
{
  for(unsigned int i = 0; i < mBalls.size(); ++i)
    mBalls[i]->draw(getCamera());
}

void GameWindow::drawBackground() const
{ mBackgroundSprite.draw(getCamera()); }

void GameWindow::drawCollisions()
{
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);
  for(unsigned int i = 0; i < cMaxCollisions; ++i)
    mCollisions[i]->draw(getCamera());
  glPopAttrib();
}

void GameWindow::drawGameBoard() const
{ mpBoard->draw(); }

void GameWindow::drawGamePanel() const
{
	Sprite a;
  a.set2dPositioningOn(true);
  a.set2dPosition(Vector3i(10, 10, 0));
  a.setAnchorPoint(Sprite::aTopLeft);
  a.setTexture(mGameAssets, QRect(108, 12, 228, 60));
  
  a.draw(getCamera());
}

void GameWindow::drawNets() const
{
  for(unsigned int i = 0; i < mNets.size(); ++i)
    mNets[i]->draw(getCamera());
}

void GameWindow::drawPlayers() const
{
  for(unsigned int i = 0; i < mPlayers.size(); ++i)
    mPlayers[i]->draw(getCamera());
}

void GameWindow::eliminatePlayer(int iId)
{
	/*Lors de l'elimination d'un joueur, on veut enlever les elements graphiques
    (palette et but) ainsi que les representations physiques. On ne peut pas
    enlever les representations physique en plein milieu d'une itération de 
    physique. C'est pourquoi, on marque un joueur a etre enlever et la methode
    Physics::updatePhysics a la fin de chaque iteration regarde s'il y a des
    joueurs a etre enlever et les enlevera correctement. De plus, le vecteur de
    joueur mPlayer et de buts mNets de GameWindow et ceux de Physics doivent 
    etre parfaitement synchronisé. C'est pourquoi la méthode 
    Physics::removeMarkedPlayer efface les jouers de GameWindow ainsi que les
    representation physiques.*/
  mpPhysics->markPlayerForRemoval(iId);
}

void GameWindow::hideOptions()
{
  //start ot resume game
  if(getState() == gsNotStarted)
    startGame();
  else
    resumeGame();
}

void GameWindow::hostingGameOptionChanged(bool iHosting)
{
  if(iHosting)
  {
    //mNetwork.setServerPort(kNetworkPort);
    //mNetwork.startServer();
    mpOptionsDialog->setServerAddress("Server started.");
  }
  else
  {
    //mNetwork.stopServer();
    mpOptionsDialog->setServerAddress("Server not started");
  }
}

void GameWindow::initializeGL()
{
  Widget3d::initializeGL();
  glClearColor(0, 0, 0, 0);
  
  //initialize texture holding all game sprites.
  QImage b(":/images/texture1.png");
  assert(!b.isNull());
  mGameAssets.set(QImage(":/images/texture1.png"));
  QImage a(":/images/background.jpg");
  assert(!a.isNull());
  mBackgroundSprite.setTexture(Texture(QImage(":/images/background.jpg")));
  mBackgroundSprite.setAnchorPoint(Sprite::aCenter);
  mBackgroundSprite.set2dPositioningOn(true);
  mBackgroundSprite.set2dPosition(getCamera().getWindowInfo().getWidth()/2,
    getCamera().getWindowInfo().getHeight()/2, -1.0);
}

void GameWindow::joinGameOptionChanged(bool iJoining)
{
  //mNetwork.connectToTcpServer(mpOptionsDialog->getHostServerAddress(), kNetworkPort);
}

void GameWindow::keyPressEvent(QKeyEvent* e)
{
  makeCurrent();
  
  switch (e->key()) 
  {
    case Qt::Key_Escape:
      pauseGame();
      //show options
      showOptions();
      break;
    default:
      break;
  }
}

void GameWindow::mouseMoveEvent(QMouseEvent* e)
{
  makeCurrent();
  
  int deltaX = e->x() - mMousePosition.x();
  int deltaY = e->y() - mMousePosition.y();
  //cout << "mouse pos: " << e->x() << " " << e->y() <<endl;
  //cout << "mouseDelta: " << deltaX  << " " << deltaY <<endl;
  
	if(mpLocalPlayer)
  {
    Player* p = mpLocalPlayer;
    Vector3d d = getCamera().pixelDeltaToGLDelta(
      deltaX, -deltaY, p->getPosition());
    
    /*La position du joueur est en globale et le delta en local. On applique
      donc la transfo toGlobal au delta et on l'ajoute à la pos du joueur.*/
    d = d * p->getTransformationToGlobal();
    if(canPlayerMoveTo(mpLocalPlayer, p->getPosition() + d))
	    p->setPosition(p->getPosition() + d);
      
//    cout<<"player d: " << d.getX() << " " << d.getY() << endl;
//    cout<<"player pos: " << p->getPosition().getX() << " " << p->getPosition().getY() << endl;
  }
  
  mMousePosition = e->pos();
  
  //reset mouse position to middle if it goes too close to the window edges
  if( e->x() <= 10 || e->x() >= width() - 10 ||
    e->y() <= 10 || e->y() >= height() - 10)
  {
    QPoint m(width() / 2, height() / 2);
    mMousePosition = m;
		QCursor::setPos(mapToGlobal(m));
  }
}

void GameWindow::mousePressEvent(QMouseEvent* e)
{
  makeCurrent();
}

void GameWindow::mouseReleaseEvent(QMouseEvent* e)
{
  makeCurrent();
}

void GameWindow::paintGL()
{  
  Widget3d::paintGL();
  if(getState() == gsRunning)
  {  
    drawBackground();
    drawGameBoard();
    drawNets();
    drawPlayers();
    drawBalls();
    drawCollisions();
#ifndef NDEBUG
    mpPhysics->drawCollisions();
#endif

	  drawGamePanel();
    
    showFps();
  }
}

void GameWindow::pauseGame()
{
  setCursor(Qt::ArrowCursor);
  setState(gsPaused);
  killTimer(mGameTimerId);
}

void GameWindow::resizeGL(int iWidth, int iHeight)
{
  Widget3d::resizeGL(iWidth, iHeight);
  mBackgroundSprite.set2dPosition(iWidth/2, iHeight/2);
}

void GameWindow::resumeGame()
{
  //replace mouse in the middle of the screen
  QPoint p(width()/2, height()/2);
  mMousePosition = p;
  QCursor::setPos(mapToGlobal(p));
  setCursor(Qt::BlankCursor);
  setState(gsRunning);
  //start the game and update the
  //frame every 16 ms -> roughly 60 frames / sec
  mGameTimerId = startTimer(kTimeStep); 
}

/*The ball entered iPlayers net*/
void GameWindow::score(int iId)
{
  int lifesLeft = mPlayers[iId]->getLifes();
  mPlayers[iId]->setLifes(--lifesLeft);
  if(mPlayers[iId]->getLifes() == 0)
  {
    //eliminate player from game, if only one player left, he won the game
    eliminatePlayer(iId);
  }
  
  //on replace les balles au centre de la game
  mpPhysics->resetBalls();
}

void GameWindow::showOptions() const
{ mpOptionsDialog->exec(); }

void GameWindow::startGame() 
{
  //initialize collision object
  for(unsigned int i = 0; i < cMaxCollisions; ++i)
  {
    mCollisions[i] = new Collision();
    mCollisions[i]->setTexture(mGameAssets, QRect(264, 168, 72, 960));
  }
  
  //add balls
  mBalls.push_back(new Ball());
  //load textured object
  for(unsigned int i = 0; i < mBalls.size(); ++i)
    mBalls[i]->setTexture(mGameAssets, QRect(12, 12, 48, 48));
    
  //create the static board game
  mpBoard = new Board2();
  
  //init player position and orientation, define the local player.
  for(unsigned int i = 0; i < mPlayers.size(); ++i)
  {
    mPlayers[i]->setTransformationToGlobal(mpBoard->getPlayerTransformation(i));
    mNets[i]->setTransformationToGlobal(mpBoard->getNetTransformation(i));
  }
  
  mpLocalPlayer = mPlayers[0];
  mpLocalPlayer->setType(Player::tHuman);
  Camera c = getCamera();
  Matrix4d cm = mpLocalPlayer->getTransformationToGlobal();
  cm.setTranslation(Point3d(0.0));
  c.setTransformationToGlobal(cm);
  setCamera(c);
  
  //init chipMunk and physics
  cpInitChipmunk();
  mpPhysics = new Physics(this, *mpBoard, mBalls, mNets, mPlayers);
  resumeGame();
}

void GameWindow::timerEvent(QTimerEvent* e)
{
  if(e->timerId() == mGameTimerId)
  {
    updateUserInput();
    updateNetwork();
    updateAi();
    updatePhysics();
    updateDisplay();
  }
  else
    Widget3d::timerEvent(e);
}

void GameWindow::updateAi()
{
  for(unsigned int i = 0; i < mPlayers.size(); ++i)
  {
    Player* p = mPlayers[i];
    if(p->getType() == Player::tComputer)
    {
      Point3d ballPosGlobal = mpPhysics->getClosestBallPos(p); 
      Point3d ballPosLocal = ballPosGlobal * p->getTransformationToLocal();
      ballPosLocal.setXYZ(ballPosLocal.getX(), 0.0, 0.0);
      p->setPosition(ballPosLocal * p->getTransformationToGlobal()); 
    }
  }
}

void GameWindow::updateDisplay()
{
  update();
}

void GameWindow::updateNetwork()
{
}

void GameWindow::updatePhysics()
{
  mpPhysics->updatePhysics();
}

void GameWindow::updateUserInput()
{
  if(qApp->hasPendingEvents())
    qApp->processEvents();
}