//

#include "ball.h"
#include "GameWindow.h"
#include "MathUtils.h"
#include "player.h"
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
  const Vector3d kHolderPos(0.0, -5.0, 0.0);
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
Board::Board() : mSize(4.0, 10.0), // 4 metres par 10 metres
  mPlayer1Transformation(),
  mPlayer2Transformation()
{
  mPlayer1Transformation.setTranslation(Point3d(0.0, -4.5, 0.0));
  mPlayer2Transformation = getRotationMatrix(-90 * kDegreeToRadian, Vector3d(0.0, 0.0, 1.0));
  mPlayer2Transformation.setTranslation(Point3d(-1.0, 4.5, 0.0));
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
Physics::Physics(const Board& iB, vector<Ball*>& iBall, 
  vector<Player*>& iP) : mpSpace(0),
  mpBoard(0),
  mPlayers(),
  mPlayerHolders(),
  mBalls(),
  mDataBalls(iBall),
  mDataPlayers(iP)
{
  //-- space
  mpSpace = cpSpaceNew();
  mpSpace->iterations = 5;
  mpSpace->elasticIterations = 5;
//mpSpace->gravity = cpv(0.0f, -9.8f);
  mpSpace->damping = 0.4;
  cpSpaceResizeStaticHash(mpSpace, 50.0f, 2000);
  cpSpaceResizeActiveHash(mpSpace, 50.0f, 100);
  
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
    //add the static shape associate with the Board body object
    cpSpaceAddStaticShape(mpSpace, pSeg);
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
    pBallShape->u = 1.0f;
    pBallShape->e = 1.0;
    cpSpaceAddShape(mpSpace, pBallShape);
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
    cpBody* pPlayer = cpBodyNew(mDataPlayers[i]->getMass(), playerMoment);
    pPlayer->p = cpv(mDataPlayers[i]->getPosition().getX(),
      mDataPlayers[i]->getPosition().getY());
    cpSpaceAddBody(mpSpace, pPlayer);
    mPlayers.push_back(pPlayer);
    
    cpShape* pShapePlayer = cpPolyShapeNew(pPlayer, 4, playerVerts, cpvzero);
    pShapePlayer->u = 1.0f;
    pShapePlayer->e = 1.0f;
    cpSpaceAddShape(mpSpace, pShapePlayer);
    
    /*on met un point statique en 0, -5 (locale) qu'on va relier aux extremitees
      de la palette afin de créer une forme stable. On applique la transfo de la
      palette a la coordonnees locale afin de la mettre en globale.*/
    cpBody* pPlayerHolder = cpBodyNew(INFINITY, INFINITY);
    Vector3d holderPos = kHolderPos * 
      mDataPlayers[i]->getTransformation();
    pPlayerHolder->p = cpvadd(pPlayer->p, cpv(holderPos.getX(), holderPos.getY()));
    mPlayerHolders.push_back(pPlayerHolder);
    
    cpSpaceAddJoint(mpSpace, cpPinJointNew(pPlayerHolder, pPlayer,
      cpvzero, cpv(v3.getX(), v3.getY()/2.0)));
    cpSpaceAddJoint(mpSpace, cpPinJointNew(pPlayerHolder, pPlayer,
      cpvzero, cpv(v2.getX(), v2.getY()/2.0)));
  }
}

Physics::~Physics()
{
  cpBodyFree(mpBoard);
  for(unsigned int i = 0; i < mBalls.size(); ++i)
    cpBodyFree(mBalls[i]);
  for(unsigned int i = 0; i < mPlayers.size(); ++i)
  {
    cpBodyFree(mPlayers[i]);
    cpBodyFree(mPlayerHolders[i]);
  }
  //free all of the shapes, bodies and joints that have been added to space
  cpSpaceFreeChildren(mpSpace);
  cpSpaceFree(mpSpace);
  mpSpace = 0;
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


void Physics::updatePhysics()
{
  for(unsigned int i = 0; i < mPlayerHolders.size(); ++i)
  {
    //applique la transfo de la palette a la position relative
    //du holder afin de le mettre en coordonnees globales
    Vector3d holderPos = kHolderPos * 
      mDataPlayers[i]->getTransformation();
    mPlayerHolders[i]->p = cpvadd( cpv(mDataPlayers[i]->getPosition().getX(),
      mDataPlayers[i]->getPosition().getY()),
      cpv(holderPos.getX(), holderPos.getY()));
  }
    
  int nbSubSteps = 3;
  double timeStep = kTimeStep / 1000.0 / nbSubSteps;
  for(int i = 0; i < nbSubSteps; i++)
  {
    for(unsigned int i = 0; i < mPlayers.size(); ++i)  
      cpBodySlew(mPlayers[i],
        cpv(mDataPlayers[i]->getPosition().getX(),
        mDataPlayers[i]->getPosition().getY()), timeStep);
    cpSpaceStep(mpSpace, timeStep);
  }
 
  //update position of ball after physics
  for(unsigned int i = 0; i < mBalls.size(); ++i)
  {
    cpVect p = mBalls[i]->p;
    mDataBalls[i]->setPosition(Point3d((double)p.x, (double)p.y, 0.0));
    mDataBalls[i]->setAngle(mBalls[i]->a);
  }
  
  //update position of player after physics
  for(unsigned int i = 0; i < mPlayers.size(); ++i)
  {
    cpVect p = mPlayers[i]->p;
    mDataPlayers[i]->setPosition(Point3d((double)p.x, (double)p.y, 0.0));
    mDataPlayers[i]->setAngle(mPlayers[i]->a);
  }
}

//--- GameWindow ---------------------------------------------------------------
GameWindow::GameWindow(QWidget* ipParent /*= 0*/) : Widget3d(ipParent),
  mpOptionsDialog(0),
  mOptions(),
  mState(gsNotStarted),
  mGameTimerId(0),
  mpLocalPlayer(0),
  mpBoard(0),
  mMousePosition(0, 0),
  mBalls(),
  mpPhysics(0)
{
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
  
  //set the camera
//  setCameraMode( Camera::PERSPECTIVE );
//  setCameraOrientation(Camera::FREE);
  setCameraMode( Camera::ORTHOGONAL );
  setCameraOrientation( Camera::XY );
  
  addPlayer(new Player("Jouer Local"));
  Player* pc = new Player("Computer");
  pc->setType(Player::tComputer);
  addPlayer(pc);
  
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
  mpLocalPlayer = 0;
  delete mpBoard;
  for(unsigned int i = 0; i < mBalls.size(); ++i)
    delete mBalls[i];
}

void GameWindow::addPlayer(Player* p)
{ mPlayers.push_back(p); }

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

void GameWindow::drawBalls() const
{
  for(unsigned int i = 0; i < mBalls.size(); ++i)
    mBalls[i]->draw();
}

void GameWindow::drawGameBoard() const
{ mpBoard->draw(); }

void GameWindow::drawPlayers() const
{
  for(unsigned int i = 0; i < mPlayers.size(); ++i)
    mPlayers[i]->draw();
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
  
  Vector3d d = getCamera().pixelDeltaToGLDelta(
    deltaX, -deltaY, mpLocalPlayer->getPosition());
  /*on applique la transfo pour mettre le deplacement en coordonnées globales
  parce que la caméra retourne le delta en coordonnees locales*/
  d = d * mpLocalPlayer->getTransformation();
  
  mpLocalPlayer->setPosition(mpLocalPlayer->getPosition() + d);
  
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
  mBalls.push_back(new Ball());
  mBalls.push_back(new Ball());
  
  //create the static board game
  mpBoard = new Board();
  
  //init player position and orientation, define the local player.
  mPlayers[0]->setTransformation(mpBoard->getPlayer1Transformation());
  mPlayers[1]->setTransformation(mpBoard->getPlayer2Transformation());
  mpLocalPlayer = mPlayers[1];
  Camera c = getCamera();
  Matrix4d cm = mpLocalPlayer->getTransformation();
  cm.setTranslation(Point3d(0.0));
  c.setTransformation(cm);
  setCamera(c);
  
  //init chipMunk and physics
  cpInitChipmunk();
  mpPhysics = new Physics(*mpBoard, mBalls, mPlayers);
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
}