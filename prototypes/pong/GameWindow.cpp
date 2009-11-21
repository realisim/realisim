//

#include <algorithm>
#include "GameObject.h"
#include "GameWindow.h"
#include "MathUtils.h"
#include <qapplication.h>
#include <qcheckbox.h>
#include <QKeyEvent>
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
}

//--- Options ------------------------------------------------------------------
Options::Options() : mIsFullScreen(false),
  mNumberOfPlayers(2)
{}

Options::Options(const Options& i) : mIsFullScreen(i.isFullScreen()),
  mNumberOfPlayers(i.getNumberOfPlayers())
{}

const Options& Options::operator=(const Options& i)
{
  mIsFullScreen = i.isFullScreen();
  mNumberOfPlayers = i.getNumberOfPlayers();
  return *this;
}

//--- OptionsDialog ------------------------------------------------------------
OptionsDialog::OptionsDialog(QWidget* i) : QDialog(i)
{
  QVBoxLayout* pMainLyt = new QVBoxLayout(this);
  pMainLyt->setSpacing(5);
  
  mpFullScreen = new QCheckBox(this);
  mpFullScreen->setText("Fullscreen");
  
  QPushButton* pOk = new QPushButton(this);
  pOk->setText("Ok");
//  QPushButton* pCancel = new QPushButton(this);
//  pCancel->setText("Cancel");
  QHBoxLayout* pOkCancelLyt = new QHBoxLayout(this);
  pOkCancelLyt->setSpacing(5);
  pOkCancelLyt->addStretch(1.0);
  pOkCancelLyt->addWidget(pOk);
  //pOkCancelLyt->addWidget(pCancel);
    
  pMainLyt->addWidget(mpFullScreen);
  pMainLyt->addStretch(1);
  pMainLyt->addLayout(pOkCancelLyt);
  
  
  connect(mpFullScreen, SIGNAL(clicked()),
    this, SLOT(optionsChanged()));
  connect(pOk, SIGNAL(clicked()),
    this, SLOT(accept()));
//  connect(pCancel, SIGNAL(clicked()),
//    this, SLOT(reject()));
}

void OptionsDialog::optionsChanged()
{ mOptions.setFullScreen(mpFullScreen->isChecked()); }

void OptionsDialog::setOptions(const Options& i)
{
  mOptions = i;
  
  //mettre le Ui a jour
  mpFullScreen->setChecked(mOptions.isFullScreen());
}

//--- Board --------------------------------------------------------------------
Board::Board() : mSize(10.0, 10.0), // metres par metres
  mPlayerTransformations(),
  mNetTransformations()
{
  //player 0 bas
  Matrix4d pt;
  pt.setTranslation(Point3d(0.0, -4.5, 0.0));
  mPlayerTransformations.push_back(pt);
  //player 1 haut
  pt = getRotationMatrix(180 * kDegreeToRadian,
    Vector3d(0.0, 0.0, 1.0));
  pt.setTranslation(Point3d(2.0, 4.5, 0.0));
  mPlayerTransformations.push_back(pt);
  //player 2 gauche
  pt = getRotationMatrix(90 * kDegreeToRadian,
    Vector3d(0.0, 0.0, 1.0));
  pt.setTranslation(Point3d(-4.5, 0.0, 0.0));
  mPlayerTransformations.push_back(pt);
  //player 3 droite
  pt = getRotationMatrix(-90 * kDegreeToRadian,
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
  nt = getRotationMatrix(90 * kDegreeToRadian,
    Vector3d(0.0, 0.0, 1.0));
  nt.setTranslation(Point3d(-5.0, 0.0, 0.0));
  mNetTransformations.push_back(nt);
  //net 3 droite
  nt = getRotationMatrix(-90 * kDegreeToRadian,
    Vector3d(0.0, 0.0, 1.0));
  nt.setTranslation(Point3d(5.0, 0.0, 0.0));
  mNetTransformations.push_back(nt);

}

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

//--- Physics ------------------------------------------------------------------
GameWindow* Physics::mpGameWindow = 0;
vector<cpBody*> Physics::mNets;
Physics::Physics(GameWindow* ipGW, const Board& iB, vector<Ball*>& iBall, 
  vector<Net*>& iN, vector<Player*>& iP) : 
  mpSpace(0),
  mpBoard(0),
  mPlayers(),
  mPlayerHolders(),
  mJoints(),
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
//mpSpace->gravity = cpv(0.0f, -9.8f);
  mpSpace->damping = 0.4;
  cpSpaceResizeStaticHash(mpSpace, 0.1f, 100);
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
    
    pSeg->u = 0.0f;
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
    //on applique la transformation de la palette au coordonnees afin
    //quelles soit en coordonnées locale du joueur
    Point3d v0 = Point3d(-w, h, 0.0) * mDataNets[i]->getTransformation();
    Point3d v1 = Point3d( w, h, 0.0) * mDataNets[i]->getTransformation();
    Point3d v2 = Point3d( w, -h, 0.0) * mDataNets[i]->getTransformation();
    Point3d v3 = Point3d(-w, -h, 0.0) * mDataNets[i]->getTransformation();

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
    
    //on applique la transformation de la palette au coordonnees afin
    //quelle soit en coordonnées globales
    Vector3d v0 = Vector3d(-w, h, 0.0) * mDataPlayers[i]->getTransformation();
    Vector3d v1 = Vector3d( w, h, 0.0) * mDataPlayers[i]->getTransformation();
    Vector3d v2 = Vector3d( w, -h, 0.0) * mDataPlayers[i]->getTransformation();
    Vector3d v3 = Vector3d(-w, -h, 0.0) * mDataPlayers[i]->getTransformation();

    cpVect playerVerts[] = {
      cpv(v0.getX(), v0.getY()),
      cpv(v1.getX(), v1.getY()),
      cpv(v2.getX(), v2.getY()),
      cpv(v3.getX(), v3.getY()),
    };

    cpFloat playerMoment = cpMomentForPoly(mDataPlayers[i]->getMass(), 4,
      playerVerts, cpvzero);
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
    
    /*on met un point statique en 0, -5 (locale) qu'on va relier aux extremitees
      de la palette afin de créer une forme stable. On applique la transfo de la
      palette a la coordonnees locale afin de la mettre en globale.*/
//    cpBody* pPlayerHolder = cpBodyNew(10*mDataPlayers[i]->getMass(), 2);
//    Vector3d holderPos = kHolderPos *  mDataPlayers[i]->getTransformation();
//    pPlayerHolder->p = cpvadd(pPlayer->p, cpv(holderPos.getX(), holderPos.getY()));
//    cpSpaceAddBody(mpSpace, pPlayerHolder);
//    mPlayerHolders.push_back(pPlayerHolder);
//    
//    cpJoint* pj1 = cpPinJointNew(pPlayerHolder, pPlayer,
//      cpvzero, cpv(v3.getX(), v3.getY()/2.0));
//    cpSpaceAddJoint(mpSpace, pj1);
//    cpJoint* pj2 = cpPinJointNew(pPlayerHolder, pPlayer,
//      cpvzero, cpv(v2.getX(), v2.getY()/2.0));
//    cpSpaceAddJoint(mpSpace, pj2);
//    mJoints.push_back(pj1);
//    mJoints.push_back(pj2);
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
//cpBodyFree(mPlayerHolders[i]);
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

/*Pour chaque collision, on va mettre des flameches!*/
int Physics::defaultCollisionFunc(cpShape* iA, cpShape* iB,
  cpContact* ipContacts, int iNumContacts, cpFloat iNormalCoef, void* ipData)
{  
  /*Returning 0 will cause the collision to be discarded. This allows you to 
    do conditional collisions.*/
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

void Physics::movePlayer(int iId, int x, int y)
{
  cpBodyApplyForce(mPlayers[iId], cpv(x, y), cpvzero);
}

void Physics::removeMarkedPlayers()
{
  if(mPlayersToRemove.empty())
    return;
    
	for(unsigned int i = 0; i < mPlayersToRemove.size(); ++i)
  {
    int id = mPlayersToRemove[i];
    cpBody* pPlayer = mPlayers[id];
    //remove joints that were tied to player
    vector<cpJoint*>::iterator it = mJoints.begin();
    for(; it != mJoints.end(); )
    {
      if((*it)->a == pPlayer || (*it)->b == pPlayer)
      {
        cpSpaceRemoveJoint(mpSpace, *it);
        cpJointFree((*it));
        it = mJoints.erase(it); //erase i and increment as side effect
      }
      else
        ++it;
    }
    
    //remove player
    removeShapes(mPlayers[id]);
    cpSpaceRemoveBody(mpSpace, mPlayers[id]);
    cpBodyFree(mPlayers[id]);
    mPlayers.erase(mPlayers.begin() + id);
////remove player holders
//removeShapes(mPlayerHolders[id]);
//cpSpaceRemoveBody(mpSpace, mPlayerHolders[id]);
//cpBodyFree(mPlayerHolders[id]);
//mPlayerHolders.erase(mPlayerHolders.begin() + id);
    //remove nets
    removeStaticShapes(mNets[id]);
    cpSpaceRemoveBody(mpSpace, mNets[id]);
    cpBodyFree(mNets[id]);
    mNets.erase(mNets.begin() + id);
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
//  for(unsigned int i = 0; i < mPlayerHolders.size(); ++i)
//  {
//    //applique la transfo de la palette a la position relative
//    //du holder afin de le mettre en coordonnees globales
//    Vector3d holderPos = kHolderPos * 
//      mDataPlayers[i]->getTransformation();
//    mPlayerHolders[i]->p = cpvadd( cpv(mDataPlayers[i]->getPosition().getX(),
//      mDataPlayers[i]->getPosition().getY()),
//      cpv(holderPos.getX(), holderPos.getY()));
//  }
  
  /*appel bodySlew pour calculer la force/vitesse de la palette, lors de la
    prochaine itération de physique, pour le déplacement effectuer par
    l'usagé*/

  for(unsigned int i = 0; i < mDataPlayers.size(); ++i)
    cpBodySlew(mPlayers[i],
      cpv(mDataPlayers[i]->getPosition().getX(),
      mDataPlayers[i]->getPosition().getY()), kcpTimeStep);
      
  int nbSteps = 3;
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
  mOptions(),
  mState(gsNotStarted),
  mGameTimerId(0),
  mPlayers(),
  mEliminatedPlayers(),
  mpLocalPlayer(0),
  mpBoard(0),
  mNets(),
  mMousePosition(0, 0),
  mBalls(),
  mpPhysics(0)
{
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
  
  //set the camera
//  setCameraMode( Camera::PERSPECTIVE );
//  setCameraOrientation(Camera::FREE);
//  Camera c = getCamera();
//  c.set(Point3d(0.0, -15.0, 5.0), Point3d(0.0), Vector3d(0.0, 1.0, 0.0));
//  setCamera(c);
  setCameraMode( Camera::ORTHOGONAL );
  setCameraOrientation( Camera::XY );
  
  addPlayer(new Player("Jouer Local"));
  Player* pc2 = new Player("Computer2");
  pc2->setType(Player::tComputer);
  addPlayer(pc2);
  
  Player* pc3 = new Player("Computer3");
  pc3->setType(Player::tComputer);
  addPlayer(pc3);
  
  Player* pc4 = new Player("Computer4");
  pc4->setType(Player::tComputer);
  addPlayer(pc4);
  
  mpOptionsDialog = new OptionsDialog(this);
  //connect dialog to GameWindow
  connect(mpOptionsDialog, SIGNAL(accepted()),
    this, SLOT(applyOptions()));
}

GameWindow::~GameWindow()
{
  delete mpPhysics;

  for(unsigned int i = 0; i < mPlayers.size(); ++i)
    delete mPlayers[i];
  for(unsigned int i = 0; i < mEliminatedPlayers.size(); ++i)
    delete mEliminatedPlayers[i];
  mpLocalPlayer = 0;
  for(unsigned int i = 0; i < mNets.size(); ++i)
    delete mNets[i];
  delete mpBoard;
  for(unsigned int i = 0; i < mBalls.size(); ++i)
    delete mBalls[i];
}

void GameWindow::addPlayer(Player* p)
{
  mPlayers.push_back(p);
  //each player comes with is net
  mNets.push_back(new Net());
}

void GameWindow::applyOptions()
{
  setUpdatesEnabled(false);
  mOptions = mpOptionsDialog->getOptions();
  if(mOptions.isFullScreen())
		parentWidget()->showFullScreen();
  else
    parentWidget()->showNormal();

  //start ot resume game
  if(getState() == gsNotStarted)
    startGame();
  else
    resumeGame();
  setUpdatesEnabled(true);
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

void GameWindow::drawBalls() const
{
  for(unsigned int i = 0; i < mBalls.size(); ++i)
    mBalls[i]->draw();
}

void GameWindow::drawGameBoard() const
{ mpBoard->draw(); }

void GameWindow::drawNets() const
{
  for(unsigned int i = 0; i < mNets.size(); ++i)
    mNets[i]->draw();
}

void GameWindow::drawPlayers() const
{
  for(unsigned int i = 0; i < mPlayers.size(); ++i)
    mPlayers[i]->draw();
}

void GameWindow::eliminatePlayer(int iId)
{
  //move player to eliminate player list
  Player* p = mPlayers[iId];
  mPlayers.erase(mPlayers.begin() + iId);
  mEliminatedPlayers.push_back(p);
  
  //remove player from physics, this remove player's paddle, player's holder
  //and player's net
  mpPhysics->markPlayerForRemoval(iId);
  
  //remove graphical player net
  mNets.erase(mNets.begin() + iId);
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
  
	if(mpLocalPlayer)
  {
    Vector3d d = getCamera().pixelDeltaToGLDelta(
      deltaX, -deltaY, mpLocalPlayer->getPosition());
    /*on applique la transfo pour mettre le deplacement en coordonnées globales
    parce que la caméra retourne le delta en coordonnees locales*/
    d = d * getCamera().getTransformationToGlobal();
    mpLocalPlayer->setPosition(mpLocalPlayer->getPosition() + d);
    //mpPhysics->movePlayer(0, deltaX*10, -deltaY*10);
  }
  
  mMousePosition = e->pos();
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
    drawGameBoard();
    drawNets();
    drawPlayers();
    drawBalls();
    mpPhysics->drawCollisions();
  }
}

void GameWindow::pauseGame()
{
  setCursor(Qt::ArrowCursor);
  setState(gsPaused);
  killTimer(mGameTimerId);
}

void GameWindow::resumeGame()
{
  //replace mouse in the middle of the screen
  QPoint p(width()/2, height()/2);
  mMousePosition = p;
  QCursor::setPos(mapToGlobal(p));
  //setCursor(Qt::BlankCursor);
  setState(gsRunning);
  //start the game and update the
  //frame every 16 ms -> roughly 60 frames / sec
  mGameTimerId = startTimer(kTimeStep); 
}

void GameWindow::showOptions() const
{
  mpOptionsDialog->setOptions(mOptions);
  mpOptionsDialog->exec();
}

void GameWindow::startGame() 
{
  //add balls
  mBalls.push_back(new Ball());
  
  //create the static board game
  mpBoard = new Board();
  
  //init player position and orientation, define the local player.
  for(unsigned int i = 0; i < mPlayers.size(); ++i)
  {
    mPlayers[i]->setTransformation(mpBoard->getPlayerTransformation(i));
    mNets[i]->setTransformation(mpBoard->getNetTransformation(i));
  }
  
  mpLocalPlayer = mPlayers[0];
  Camera c = getCamera();
  Matrix4d cm = mpLocalPlayer->getTransformation();
  cm.setTranslation(Point3d(0.0));
  c.setTransformationToLocal(cm);
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
    updatePhysics();
    updateDisplay();
  }
  else
    Widget3d::timerEvent(e);

}

void GameWindow::updateDisplay()
{
  update();
}

void GameWindow::updatePhysics()
{
  mpPhysics->updatePhysics();
}

void GameWindow::updateUserInput()
{
  if(qApp->hasPendingEvents())
    qApp->processEvents();
}//