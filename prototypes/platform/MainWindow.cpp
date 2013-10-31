/* */

#include "3d/Camera.h"
#include "3d/Sprite.h"
#include "3d/Texture.h"
#include "3d/Utilities.h"
#include "math/BoundingBox.h"
#include "math/Matrix4x4.h"
#include "math/Point.h"
#include "math/MathUtils.h"
#include "MainWindow.h"
#include <QKeyEvent>
#include <qlayout.h>
#include <QFile.h>
#include "utils/SpriteCatalog.h"

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
  utils::SpriteCatalog& sp = mEngine.getSpriteCatalog();
  const Engine::Stage& stage = mEngine.getStage();
  
	//projection ortho
  glMatrixMode( GL_PROJECTION );
  glPushMatrix(); glLoadIdentity();
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix(); glLoadIdentity();
  const Camera& c = mEngine.getGameCamera();
  c.applyProjectionTransformation();
  c.applyModelViewTransformation();
  
  //dessine le background
  {
  glColor4ub( 255, 255, 255, 255 );
  Texture background = sp.getTexture( stage.getBackgroundToken() );

  double nbRepeatX = 1.0, nbRepeatY = 1.0;
  Vector2i origin = stage.getCellPixelCoordinate( 0 );
  Vector2i end = stage.getCellPixelCoordinate( stage.terrainWidth(),
    stage.terrainHeight() );
  Vector2i size = end - origin;
  nbRepeatX = size.x() / (double)background.width();
  nbRepeatY = size.y() / (double)background.height();
  glEnable( GL_TEXTURE_2D );
  glBindTexture( GL_TEXTURE_2D, background.getId() );
  glBegin(GL_QUADS);
    glTexCoord2d( 0.0, 0.0 );
    glVertex2d( origin.x(), origin.y() );
    glTexCoord2d( 0.0, nbRepeatY );
    glVertex2d( origin.x(), origin.y() + size.y() );
    glTexCoord2d( nbRepeatX, nbRepeatY );
    glVertex2d( origin.x() + size.x(), origin.y() + size.y() );
    glTexCoord2d( nbRepeatX, 0.0 );
    glVertex2d( origin.x() + size.x(), origin.y() );
  glEnd();
  glDisable( GL_TEXTURE_2D );
  }
  
  
  //dessine la map
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

  glEnable( GL_BLEND );
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  glColor4ub( 255, 255, 255, 255 ); 
  vector<int> visibleCells = mEngine.getVisibleCells();
printf("nombre de visible cells: %d\n", (int)visibleCells.size());
  bool draw = false;
  Sprite cellSprite;
  for( size_t i = 0; i < visibleCells.size(); ++i )
  {  	
  	switch ( stage.value( visibleCells[i] ) )
    {
      case Engine::Stage::ctGround: draw = true; 
      	cellSprite = sp.getSprite("ground"); break;
      case Engine::Stage::ctStart: draw = true;
      	cellSprite = sp.getSprite("start"); break;
      default: draw = false; break;
    }
    if( draw )
    {
      Vector2i t = stage.getCellPixelCoordinate( visibleCells[i] );
    	glPushMatrix();
      glTranslated(t.x(), t.y(), 0.0);
      cellSprite.draw();
      glPopMatrix();
    }
  }
  
  //les grilles d'édition et le data map
  glColor4ub( 255, 255, 255, 120 );
  if( mEngine.getState() == Engine::sEditing )
  {
  	for( size_t i = 0; i < visibleCells.size(); ++i )
  	{
      Vector2i t = stage.getCellPixelCoordinate( visibleCells[i] );
    	glPushMatrix();
      glTranslated(t.x(), t.y(), 0.0);
      sp.getSprite("editionGrid").draw();
      glPopMatrix();
    }
      
    //data map
//    glPushMatrix();
//	  glScaled(stage.cellSize().x(), stage.cellSize().y(), 1.0);
//    glColor4ub( 255, 255, 255, 120 ); 
// 	 	mEditionMap.draw();
// 	 	glPopMatrix();
  }
  
  //dessine le joueur
  Sprite* playerSprite;
  switch (mEngine.getPlayerState()) 
  {
    case Engine::Player::sIdle: 
      playerSprite = &(sp.getSprite("player idle"));
      break;
    case Engine::Player::sWalking:
      {
      const Vector2d& v = mEngine.getPlayerVelocity();
      playerSprite = v.x() > 0.0 ?  &(sp.getSprite("player run right")) :
        &(sp.getSprite("player run left")) ;
      }
      break;
    default: playerSprite = &(sp.getSprite("player idle")); break;
  }
  glDisable( GL_BLEND );
  glPushMatrix();
  glColor4ub( 255, 255, 255, 255 );
  glTranslated(mEngine.getPlayerPosition().x(),
  	mEngine.getPlayerPosition().y(), 0.0 );
  playerSprite->draw();
  glPopMatrix();
  
  //dessine les intersections entre le joueur et le terrain
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
glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//usleep(20000);
static int _rx = 30;
_rx += 2;
_rx %= 200;
math::Circle _c( Point2d( 100, 100 ), 60 );
math::Rectangle _r( Point2d( _rx, 80 ), Vector2d( 200, 200 ) );
treeD::drawCircle2d( _c.center(), _c.radius() );
treeD::drawRectangle2d(_r.bottomLeft(), _r.size());
Intersection2d _i = intersects( _c, _r );

glPointSize(2.0);
glColor3ub( 255, 0, 0);

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
treeD::drawRectangle2d(_r1.bottomLeft(), _r1.size());
treeD::drawRectangle2d(_r2.bottomLeft(), _r2.size());
BoundingBox2d bb;
glColor3ub(255, 0, 0 );
for( int i = 0; i < 4; i++ )
{
	for( int j = 0; j < 4; j++ )
  {
  	bb.add( toPoint(_r1.points()[i] - _r2.points()[j] ) );
  }
}
treeD::drawRectangle2d(bb.bottomLeft(), bb.size());

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
glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


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
  glDisable( GL_LIGHTING );
  glDisable( GL_DEPTH_TEST );

	mEngine.setSpriteCatalog( "level1.cat" );

	//mBrick.set( QImage( ":/images/brick 32x32.jpg" ) );
  //mEditionGrid.set( QImage( ":/images/editionGrid.png" ) );
  //mStart.set( QImage( ":/images/start.png" ) );
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
  update();
}

//-----------------------------------------------------------------------------
void Viewer::gotEvent( Engine::event iE )
{
	switch (iE) 
  {
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
MainWindow::MainWindow() : QMainWindow(),
	Client(),
  mEngine(),
  mpViewer(0)
{
  resize(800, 600);
  
  QWidget* pMainWidget = new QWidget( this );
  setCentralWidget(pMainWidget);
  
  QHBoxLayout* pLyt = new QHBoxLayout(pMainWidget);
  pLyt->setMargin(0);
  pLyt->setMargin( 0 );
    
  mpViewer = new Viewer( mEngine, pMainWidget);
  
  //--- paneau pour l'édition
  mpEditionPanel = new QFrame( pMainWidget );
  mpEditionPanel->hide();
  {
  	QVBoxLayout* pLyt = new QVBoxLayout(mpEditionPanel);
    
    //type de cellule
  	QHBoxLayout* pL1 = new QHBoxLayout();
    {
    	QLabel* pL = new QLabel("Cellule", mpEditionPanel);
      mpCellType = new QComboBox( mpEditionPanel );
      connect( mpCellType, SIGNAL(activated(int)), 
      	this, SLOT(cellTypeChanged( int ) ) );
      
      for( int i = 0; i < Engine::Stage::ctNumberOfCellType; ++i )
      	mpCellType->insertItem( i, mEngine.toString( 
        	(Engine::Stage::cellType)i ) );
      	
      pL1->addWidget(pL);
      pL1->addWidget(mpCellType);
    }
    
    //background
    QHBoxLayout* pL2 = new QHBoxLayout();
    {
    	QLabel* pl = new QLabel( "Backg.", mpEditionPanel );
      mpBackground = new QComboBox( mpEditionPanel );
      connect( mpBackground, SIGNAL( activated(int) ),
      	this, SLOT( backgroundChanged(int) ) );
        
      pL2->addWidget(pl);
      pL2->addWidget(mpBackground);
      pL2->addStretch(1);
    }
    
    pLyt->addLayout(pL1);
    pLyt->addLayout(pL2);
    pLyt->addStretch(1);
  }
  
  pLyt->addWidget( mpViewer, 3 );
  pLyt->addWidget( mpEditionPanel, 1 );
  
  mpViewer->setCameraOrientation(Camera::XY);
  Camera c = mpViewer->getCamera();
	c.setOrthoProjection( 200, 0.5, 2000 );
  mpViewer->setCamera(c, false);
  
  mEngine.registerClient( this );
  mEngine.registerClient( mpViewer );
}

//-----------------------------------------------------------------------------
void MainWindow::backgroundChanged(int iIndex)
{
	QString token = mpBackground->currentText();
  mEngine.setBackgroundToken( token );
  updateUi();
}

//-----------------------------------------------------------------------------
void MainWindow::cellTypeChanged( int iCt )
{ mEngine.setEditingTool( (Engine::Stage::cellType)iCt ); }

//-----------------------------------------------------------------------------
void MainWindow::gotEvent( Engine::event iE )
{
	switch (iE) 
  {
  	case Engine::eStateChanged:
    	updateUi();
      break;
    case Engine::eQuit: qApp->quit(); break;
    default: break;
  }
}

//-----------------------------------------------------------------------------
void MainWindow::populateBackground()
{
	utils::SpriteCatalog& sp = mEngine.getSpriteCatalog();
	if( mpBackground->count() == 0 )
  {
    for( int i = 0; i < sp.getNumberOfTextures(); ++i )
    {
      mpBackground->addItem( sp.getTextureToken(i) );
    }
  }
}

//-----------------------------------------------------------------------------
void MainWindow::updateUi()
{
	switch( mEngine.getState() )
  {
  case Engine::sEditing: 
  	mpEditionPanel->show();
    break;
  default: mpEditionPanel->hide(); break;
  }
  
  if( mpEditionPanel->isVisible() )
  {
  	populateBackground();
  }
}
