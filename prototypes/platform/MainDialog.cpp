/* */

#include "3d/Camera.h"
#include "3d/Sprite.h"
#include "3d/Texture.h"
#include "3d/Utilities.h"
#include "math/BoundingBox.h"
#include "math/Matrix4x4.h"
#include "math/Point.h"
#include "math/MathUtils.h"
#include "MainDialog.h"
#include <QKeyEvent>
#include <qlayout.h>
#include <QFile.h>

using namespace realisim;
	using namespace platform;
  using namespace math;
  using namespace treeD;

namespace 
{
}

Viewer::Viewer(Engine& iE, QWidget* ipParent /*=0*/) : 
	Widget3d(ipParent),
	Client(),
  mEngine( iE )
{
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking( true );
  //mGameCamera = getCamera();
  //mGameCamera.set( Point3d( 0.0, 0.0, 5.0 ),
//  	Point3d( 0.0, 0.0, 0.0 ),
//    Vector3d( 0.0, 1.0, 0.0 ) );
}

Viewer::~Viewer()
{}

//-----------------------------------------------------------------------------
void Viewer::draw()
{
	switch ( mEngine.getState() ) 
  {
    case Engine::sMainMenu: drawMenu(); break;
    case Engine::sConfigureMenu: drawMenu(); break;
    case Engine::sEditing: drawGame(); break;
    case Engine::sPlaying: drawGame(); break;
    case Engine::sPaused: drawMenu(); break; 
    default: break;
  }
}

//-----------------------------------------------------------------------------
void Viewer::drawGame()
{
	//projection ortho
  glMatrixMode( GL_PROJECTION );
  glPushMatrix(); glLoadIdentity();
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix(); glLoadIdentity();
  const Camera& c = mEngine.getGameCamera();
  c.applyProjectionTransformation();
  c.applyModelViewTransformation();
  
  //dessine la map
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  glColor3ub( 255, 255, 255 ); 
  const Engine::Stage& stage = mEngine.getStage();
  glPushMatrix();
  glScaled(stage.cellSize().x(), stage.cellSize().y(), 0);
  mEditionMap.draw();
  glPopMatrix();
//  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
//  for( int j = 0; j < stage.terrainSize().y(); ++j )
//	  for( int i = 0; i < stage.terrainSize().x(); ++i )
//    {
//    	utilities::drawRectangle2d(
//        Point2d( i * stage.cellSize().x(), j * stage.cellSize().y() ),
//        Vector2d( stage.cellSize() ) );
//    }
//  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  
  // dessine le joueur
  glDisable( GL_LIGHTING );
  glDisable( GL_DEPTH_TEST );
  
  glPushMatrix();
  glColor4ub( 255, 255, 255, 255 );
  glTranslated(mEngine.getPlayerPosition().x(),
  	mEngine.getPlayerPosition().y(), 0.0 );
  mPlayer.draw();
  glPopMatrix();
  
  //dessine les intersections entre le joueur et le terrain
  glDisable( GL_LIGHTING );
  glColor3ub( 255, 255, 0);
  glPointSize(2.0);
  const Intersection2d& intersection = mEngine.getPlayerIntersection();
  for( int i = 0; i < intersection.numberOfPoints(); ++i )
  {
    glBegin( GL_POINTS );
    glVertex2dv( intersection.point(i).getPtr() );
    glEnd();
    
    //normal
    Point2d n = intersection.point(i) + intersection.normal(i) * 10;
    glBegin( GL_LINES );
    glVertex2dv( intersection.point(i).getPtr() );
    glVertex2dv( n.getPtr() );
    glEnd();
  }

  
  // dessine les grilles
//  Point3d bottomLeft = c.pixelToGL( 0, c.getWindowInfo().getHeight() );
//  Point3d topRight = c.pixelToGL( c.getWindowInfo().getWidth(), 0 );
//  vector<Tile> tiles = mEngine.getTiles(  
//  	Point2d(bottomLeft.getX(), bottomLeft.getY() ), 
//  	Point2d( topRight.getX(), topRight.getY() ) );
//  glDisable( GL_LIGHTING );
//  glColor3ub( 255, 255, 255 );
//  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
//  for( size_t i = 0; i < tiles.size(); ++i )
//  {
//  	glPushMatrix();
//    glTranslated(tiles[i].x(), tiles[i].y(), 0.0);
//    glScaled( tiles[i].width(), tiles[i].height(), 0 );
//		utilities::drawRectangle2d( Point2d(0.0), Vector2d(1.0) );
//    glPopMatrix();
//  }
  
  //affiche letat du jouer
  QString playerState;
  switch ( mEngine.getPlayerState() ) 
  {
    case Engine::Player::sIdle : playerState = "idle"; break;
    case Engine::Player::sWalking : playerState = "walking"; break;
    case Engine::Player::sRunning : playerState = "running"; break;
    case Engine::Player::sFalling : playerState = "falling"; break;
    case Engine::Player::sJumping : playerState = "jumping"; break;
    default: break;
  }
  renderText(10, 10, playerState );
  
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();
}


#include "Math/Primitives.h"
#include "Math/intersection.h"
//-----------------------------------------------------------------------------
void Viewer::drawMenu()
{
	//projection ortho
  Camera c = getCamera();
  c.set( Point3d( 0.0, 0.0, 5.0 ),
  	Point3d( 0.0, 0.0, 0.0 ),
    Vector3d( 0.0, 1.0, 0.0 ) );
  c.setProjection( 0, c.getWindowInfo().getWidth(),
  	0, c.getWindowInfo().getHeight(), 0.2, 100.0, Camera::ORTHOGONAL );
	
  glMatrixMode( GL_PROJECTION );
  glPushMatrix(); glLoadIdentity();
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix(); glLoadIdentity();
  c.applyProjectionTransformation();
  c.applyModelViewTransformation();
  
  vector<QString> menuItems; 
  int currentMenuItem;
  switch ( mEngine.getState() ) 
  {
    case Engine::sMainMenu: 
    	menuItems = mEngine.getMainMenuItems();
      currentMenuItem = mEngine.getCurrentMainMenuItem();
      break;
    case Engine::sPaused:
    	menuItems = mEngine.getPauseMenuItems();
      currentMenuItem = mEngine.getCurrentPauseMenuItem();
      break;
    case Engine::sConfigureMenu:
    	menuItems = mEngine.getConfigureMenuItems();
      currentMenuItem = mEngine.getCurrentConfigureMenuItem();
      break;
    default: break;
  }
  
  Text t( "TEXTE CON POUR AVOIR LA HAUTEUR PAR QFONTMETRIC" );
  t.setFont( QFont("Futura", 48 ) );
  int itemsHeight = t.height() * menuItems.size();
  int y = c.getWindowInfo().getHeight() / 2 + itemsHeight / 2;
  
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
  for(size_t i = 0; i < menuItems.size(); ++i)
  {
  	t.setText( menuItems[i] );
    if( (uint)currentMenuItem == i )
    	t.setBackgroundColor( QColor("yellow") );
    else t.setBackgroundColor( QColor( 0, 0, 0, 0 ) );

    glPushMatrix();
    glTranslated( c.getWindowInfo().getWidth() / 2, y, 0 );
    Sprite s;
    s.set( t.getTexture() );
    s.draw();
    y -= t.height() + 10;
    glPopMatrix();
  }
  
  
//--------
usleep(20000);
static int _rx = 30;
_rx += 2;
_rx %= 200;
math::Circle _c( Point2d( 100, 100 ), 60 );
math::Rectangle _r( Point2d( _rx, 80 ), Vector2d( 200, 200 ) );
utilities::drawCircle2d( _c.center(), _c.radius() );
utilities::drawRectangle2d(_r.bottomLeft(), _r.size());
Intersection2d _i = intersects( _c, _r );

glPointSize(2.0);
glColor3ub( 255, 0, 0);
glDisable( GL_DEPTH_TEST );

for( int i = 0; i < _i.numberOfPoints(); ++i )
{
	glBegin( GL_POINTS );
  glVertex2dv( _i.point(i).getPtr() );
  glEnd();
  
  //normal
  Point2d n = _i.point(i) + _i.normal(i) * 10;
  glBegin( GL_LINES );
  glVertex2dv( _i.point(i).getPtr() );
  glVertex2dv( n.getPtr() );
  glEnd();
}


//on dessine le minkowski
static int _r1x = 180;
_r1x += 2;
_r1x %= 260;
Rectangle _r1( Point2d( _r1x, 220 ), Vector2d(40, 60) );
Rectangle _r2( Point2d( 200, 200 ), Vector2d(40, 40) );
glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
utilities::drawRectangle2d(_r1.bottomLeft(), _r1.size());
utilities::drawRectangle2d(_r2.bottomLeft(), _r2.size());
BoundingBox2d bb;
glColor3ub(255, 0, 0 );
for( int i = 0; i < 4; i++ )
{
	for( int j = 0; j < 4; j++ )
  {
  	bb.add( toPoint(_r1.points()[i] - _r2.points()[j] ) );
  }
}
utilities::drawRectangle2d(bb.bottomLeft(), bb.size());

glColor3ub( 255, 255, 0 );
glBegin( GL_POINTS );
for( int i = 0; i < 4; i++ )
{
	for( int j = 0; j < 4; j++ )
  {
  	glVertex2dv( (_r1.points()[i] - _r2.points()[j] ).getPtr() );
  }
}
glEnd();


glEnable( GL_DEPTH_TEST );
glColor3ub( 255, 255, 255);
//--------
  
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();
}

//-----------------------------------------------------------------------------
void Viewer::refreshEditionMap()
{
  const Engine::Stage& s = mEngine.getStage();
  QByteArray ba = s.terrain();
  Texture terrainTex; terrainTex.set( (void*)ba.constData(), s.terrainSize(),
  	GL_LUMINANCE, GL_UNSIGNED_BYTE );
  mEditionMap.setAnchorPoint( Sprite::aBottomLeft );
  mEditionMap.set( terrainTex );	
}

//-----------------------------------------------------------------------------
void Viewer::initializeGL()
{
	Widget3d::initializeGL();
  
//  mPlayer.set( QImage( ":/images/explosion.png" ) );
//  mPlayer.setFrameGrid(4, 4);
//  mPlayer.setNumberOfFrames( 16 );
//  mPlayer.setAnimationDuration(400);
//  mPlayer.animate();

//  Texture t;
//  t.set( QImage( ":/images/player test1.jpg" ) );
//  t.setWrapMode( GL_CLAMP );
//  mPlayer.set( t, QRect( QPoint(20, 0), QSize( 40, 20 ) ) );
//  mPlayer.setFrameGrid( 2, 1 );
//  mPlayer.setNumberOfFrames( 2 );
//  mPlayer.setAnimationDuration(400);
//  mPlayer.animate();

  Texture t;
  t.set( QImage( ":/images/player test3.png" ) );
  t.setWrapMode( GL_CLAMP_TO_EDGE );
  t.setFilter( GL_NEAREST );
  mPlayer.set( t );
  mPlayer.setFrameGrid( 1, 1 );
  mPlayer.setNumberOfFrames( 1 );
  mPlayer.animate( false );
}

//-----------------------------------------------------------------------------
void Viewer::keyPressEvent( QKeyEvent* ipE )
{ if( !ipE->isAutoRepeat() ) mEngine.keyPressed( ipE->key() ); }

//-----------------------------------------------------------------------------
void Viewer::keyReleaseEvent( QKeyEvent* ipE )
{ mEngine.keyReleased( ipE->key() ); }

//-----------------------------------------------------------------------------
void Viewer::mouseMoveEvent( QMouseEvent* ipE )
{ mEngine.mouseMoved( Point2i( ipE->x(), ipE->y() ) ); }

//-----------------------------------------------------------------------------
void Viewer::mousePressEvent( QMouseEvent* ipE )
{ mEngine.mousePressed( ipE->button() ); }

//-----------------------------------------------------------------------------
void Viewer::mouseReleaseEvent( QMouseEvent* ipE )
{ mEngine.mouseReleased( ipE->button() ); }

//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
  Widget3d::paintGL();
  draw();
}

//-----------------------------------------------------------------------------
void Viewer::resizeGL(int iW, int iH)
{
  Widget3d::resizeGL( iW, iH );
  //mGameCamera.setWindowSize(iW, iH);
//  mGameCamera.setProjection( 0, iW, 0, iH, 0.2, 100.0, Camera::ORTHOGONAL );
  update();
}

//-----------------------------------------------------------------------------
void Viewer::gotEvent( Engine::event iE )
{
	switch (iE) 
  {
  	case Engine::eStageChanged:
    switch (mEngine.getState()) 
      {
        case Engine::sEditing:
        	refreshEditionMap();
          break;
        default: break;
      }
    break;
  	case Engine::eStateChanged:
    	switch (mEngine.getState()) 
      {
        case Engine::sEditing:
        	refreshEditionMap();
          break;
        default: break;
      }
    break;
    case Engine::eFrameDone: update(); break;
    default: break;
  }
}

//-----------------------------------------------------------------------------
// --- MAIN WINDOW
//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QMainWindow(),
	Client(),
  mpViewer(0)
{
  resize(800, 600);
  
  QWidget* pMainWidget = new QWidget( this );
  setCentralWidget(pMainWidget);
  
  QHBoxLayout* pLyt = new QHBoxLayout(pMainWidget);
  pLyt->setMargin(0);
  pLyt->setMargin( 0 );
    
  mpViewer = new Viewer( mEngine, pMainWidget);
  
  pLyt->addWidget(mpViewer );
  
  mpViewer->setCameraOrientation(Camera::XY);
  Camera c = mpViewer->getCamera();
	c.setOrthoProjection( 200, 0.5, 2000 );
  mpViewer->setCamera(c, false);
  
  mEngine.registerClient( this );
  mEngine.registerClient( mpViewer );
}

//-----------------------------------------------------------------------------
void MainDialog::gotEvent( Engine::event iE )
{
	switch (iE) 
  {
    case Engine::eQuit: qApp->quit(); break;
    default: break;
  }
}