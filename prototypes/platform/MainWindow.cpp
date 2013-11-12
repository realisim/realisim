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
  using namespace utils;

namespace 
{
	const int kSpriteTableWidth = 8;
  
  const QString kShaderBidon = "#version 120\n"
  "uniform sampler2D texture;\n"
  "uniform vec2 playerPos;\n"
  "uniform float lightRadius;\n"
  "void main()\n"
  "{\n"
  "  float lightRadiusSquared = lightRadius*lightRadius;\n"
  "  vec4 color = texture2D(texture, gl_TexCoord[0].xy);\n"
  "  vec2 d = gl_FragCoord.xy - playerPos;\n"
  "  float normSquared = d.x*d.x + d.y*d.y;\n"
  "  float n = 1.0 / ( normSquared/lightRadiusSquared );\n"
  "  color.a = min( 1.0, n );\n"
  "  gl_FragColor = color;\n"
  "}\n";
}

Viewer::Viewer(Engine& iE, QWidget* ipParent /*=0*/) : 
	Widget3d(ipParent),
	Client(),
  mEngine( iE )
{
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking( true );
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
    case Engine::sPlaying:
    {
    	pushFrameBuffer( mFbo );
      mFbo.drawTo(0);
      glClear( GL_COLOR_BUFFER_BIT );
    	drawGame();
      popFrameBuffer();
      
      const Camera& gc = mEngine.getGameCamera();
      Point3f playerGlPos( mEngine.getPlayerPosition().x(),
      	mEngine.getPlayerPosition().y(), 0.0 );
      Point2d playerScreenPos = gc.glToPixel( playerGlPos );
      
      const Camera::WindowInfo& wi = mEngine.getGameCamera().getWindowInfo();
			ScreenSpaceProjection ssp( wi.getSize() );
      {
      	glEnable(GL_BLEND);
  			glColor4ub(255, 255, 255, 255);  
        pushShader( mPostProcessShader );
        mPostProcessShader.setUniform( "playerPos", playerScreenPos );
        mPostProcessShader.setUniform( "lightRadius", 100.0 );
        drawRectangle2d(mFbo.getTexture(0), Point2d(0.0),
          Vector2d( wi.getSize() ) );
        popShader();
        glDisable(GL_BLEND);
      }
      break;
    }
    case Engine::sPaused: drawMenu(); break; 
    default: break;
  }
}

//-----------------------------------------------------------------------------
void Viewer::drawGame()
{
  utils::SpriteCatalog& sc = mEngine.getSpriteCatalog();
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
  Texture background = sc.getTexture( stage.getBackgroundToken() );
	if( background.isValid() )
  {
    double nbRepeatX = 1.0, nbRepeatY = 1.0;
    Vector2i origin = stage.getCellPixelCoordinate( 0 );
    Vector2i end = stage.getCellPixelCoordinate( stage.getTerrainWidth(),
      stage.getTerrainHeight() );
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
  }
  
  //dessine le data map
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  glEnable( GL_BLEND );
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  vector<int> visibleCells = mEngine.getVisibleCells();
  if( mEngine.getState() == Engine::sEditing )
  {
    glColor4ub( 255, 255, 255, 255 );     
    bool draw = false;
    for( size_t i = 0; i < visibleCells.size(); ++i )
    {  	
      switch ( stage.value( visibleCells[i] ) )
      {
        case Engine::Stage::ctGround: draw = true; 
          glColor4ub( 255, 255, 255, 255 ); break;
        case Engine::Stage::ctStart: draw = true;
          glColor4ub( 12, 12, 255, 255 ); break;
        default: draw = false; break;
      }
      if( draw )
      {
        Vector2i t = stage.getCellPixelCoordinate( visibleCells[i] );
        drawRectangle2d(toPoint(t), stage.getCellSize() );
      }
    }
  }
  
  //dessine les layers
  for( int i = 0; i < stage.getNumberOfLayers(); ++i )
  {
  	for( size_t j = 0; j < visibleCells.size(); ++j )
	  {
    	QString st = stage.getToken( i, visibleCells[j] );
      if( !st.isNull() )
      {
        Vector2i t = stage.getCellPixelCoordinate( visibleCells[j] );
        glPushMatrix();
        glTranslated(t.x(), t.y(), 0.0);
        sc.getSprite( st ).draw();
        glPopMatrix(); 	
      }
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
      sc.getSprite("editionGrid").draw();
      glPopMatrix();
    }
  }
  
  //dessine le joueur
  Sprite* playerSprite;
  switch (mEngine.getPlayerState()) 
  {
    case Engine::Player::sIdle: 
      playerSprite = &(sc.getSprite("player idle"));
      break;
    case Engine::Player::sWalking:
      {
      const Vector2d& v = mEngine.getPlayerVelocity();
      playerSprite = v.x() > 0.0 ?  &(sc.getSprite("player run right")) :
        &(sc.getSprite("player run left")) ;
      }
      break;
    default: playerSprite = &(sc.getSprite("player idle")); break;
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
  	0, c.getWindowInfo().getHeight(), 0.2, 100.0, Camera::Projection::tOrthogonal );
	
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
void Viewer::initializeGL()
{
	Widget3d::initializeGL();
  glDisable( GL_LIGHTING );
  glDisable( GL_DEPTH_TEST );
  glClearColor(0.0, 0.0, 0.0, 0.0);

	mEngine.setSpriteCatalog( "level1.cat" );
	mFbo.addColorAttachment(true);
  
  mPostProcessShader.addFragmentSource( kShaderBidon );
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
  mFbo.resize(iW, iH);
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
        case Engine::sEditing: break;
        default: break;
      }
    break;
    case Engine::eStageLoaded:
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
  mpViewer(0),
  mEditionPath( QDir::homePath() )
{
  resize(800, 600);
  
  QWidget* pMainWidget = new QWidget( this );
  setCentralWidget(pMainWidget);
  
  QHBoxLayout* pLyt = new QHBoxLayout(pMainWidget);
  pLyt->setMargin(0);
  pLyt->setMargin( 0 );
    
  mpViewer = new Viewer( mEngine, pMainWidget);
  
  //--- paneau pour l'édition
  mpEditionPanel = new QWidget( 0 );
  mpEditionPanel->setWindowFlags( mpEditionPanel->windowFlags() | Qt::Tool |
    Qt::WindowStaysOnTopHint );
	mpEditionPanel->setMinimumSize(400, 600);
  mpEditionPanel->hide();
  {
  	QVBoxLayout* pLyt = new QVBoxLayout(mpEditionPanel);
    pLyt->setSpacing( 2 );
    pLyt->setMargin( 2 );
    
    //ouvrir et nouveau
    QHBoxLayout* pNewAndOpen = new QHBoxLayout();
    {
    	QPushButton* pNew = new QPushButton( "Nouveau...", mpEditionPanel );
      connect( pNew, SIGNAL( clicked() ), this, SLOT( newMapClicked() ) );
      QPushButton* pOpen = new QPushButton( "Ouvrir...", mpEditionPanel );
      connect( pOpen, SIGNAL( clicked() ), this, SLOT( openMapClicked() ) );
      
      pNewAndOpen->addWidget( pNew );
      pNewAndOpen->addWidget( pOpen );
      pNewAndOpen->addStretch( 1 );
    }
    
    //Layers
    QVBoxLayout* pLayersLyt = new QVBoxLayout();
    {
    	mpLayers = new QListWidget( mpEditionPanel );
      mpLayers->setAlternatingRowColors(true);
      connect( mpLayers, SIGNAL( itemChanged( QListWidgetItem* ) ),
      	this, SLOT( layerSelectionChanged( QListWidgetItem* ) ) );
      pLayersLyt->addWidget(mpLayers);
      mpLayers->setFixedHeight( 80 );
    }
    
    //sprites
    QVBoxLayout* pL1 = new QVBoxLayout();
    {
    	mpSprites = new QTableWidget( mpEditionPanel );
      connect( mpSprites, SIGNAL( cellClicked(int, int) ),
	      this, SLOT( spriteSelectionChanged(int, int) ) );
          
      QHBoxLayout* pAddRemoveLyt = new QHBoxLayout();
      {
      	QPushButton* pAdd = new QPushButton( "ajouter", mpEditionPanel );
        connect( pAdd, SIGNAL( clicked() ), 
        	this, SLOT( addSpriteToLayerClicked() ) );
        QPushButton* pRemove = new QPushButton( "enlever", mpEditionPanel );
        connect( pRemove, SIGNAL( clicked() ), 
        	this, SLOT( removeSpriteFromLayerClicked() ) );
        
        pAddRemoveLyt->addStretch(1);
        pAddRemoveLyt->addWidget(pAdd);
        pAddRemoveLyt->addWidget(pRemove);
      }
      
      pL1->addWidget( mpSprites );
      pL1->addLayout( pAddRemoveLyt );
    }
    
    //type de cellule
  	QHBoxLayout* pL2 = new QHBoxLayout();
    {
    	QLabel* pL = new QLabel("Cellule", mpEditionPanel);
      mpCellType = new QComboBox( mpEditionPanel );
      connect( mpCellType, SIGNAL(activated(int)), 
      	this, SLOT(cellTypeChanged( int ) ) );
      
      for( int i = 0; i < Engine::Stage::ctNumberOfCellType; ++i )
      	mpCellType->insertItem( i, mEngine.toString( 
        	(Engine::Stage::cellType)i ) );
      	
      pL2->addWidget(pL);
      pL2->addWidget(mpCellType);
    }
    
    //background
    QHBoxLayout* pL3 = new QHBoxLayout();
    {
    	QLabel* pl = new QLabel( "Backg.", mpEditionPanel );
      mpBackground = new QComboBox( mpEditionPanel );
      connect( mpBackground, SIGNAL( activated(int) ),
      	this, SLOT( backgroundChanged(int) ) );
        
      pL3->addWidget(pl);
      pL3->addWidget(mpBackground);
      pL3->addStretch(1);
    }
    
    pLyt->addLayout(pNewAndOpen);
    pLyt->addLayout(pLayersLyt);
    pLyt->addLayout(pL1);
    pLyt->addLayout(pL2);
    pLyt->addLayout(pL3);
    pLyt->addStretch(1);
  }
  
  pLyt->addWidget( mpViewer );
  
  mpViewer->setCameraOrientation(Camera::XY);
  Camera c = mpViewer->getCamera();
	c.setOrthoProjection( 200, 0.5, 2000 );
  mpViewer->setCamera(c, false);
  
  mEngine.registerClient( this );
  mEngine.registerClient( mpViewer );
}

//-----------------------------------------------------------------------------
void MainWindow::addSpriteToLayerClicked()
{
  SpriteCatalog& sc = mEngine.getSpriteCatalog();
  int layer = mpLayers->currentRow();
  for( int i = 0; i < (int)sc.getNumberOfSprites(); ++i )
  	mEngine.addTokenToLayer( layer, sc.getSpriteToken(i) );
    
  mpSprites->clear(); mpSprites->setRowCount(0);
  updateUi();
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
  	case Engine::eStageLoaded:
    mpLayers->clear();
    mpSprites->clear(); mpSprites->setRowCount(0);
    mpBackground->clear();
    updateUi();
    break;
  	case Engine::eStateChanged: updateUi(); break;
    case Engine::eQuit: qApp->quit(); break;
    default: break;
  }
}

//-----------------------------------------------------------------------------
void MainWindow::layerSelectionChanged(QListWidgetItem* ipItem)
{
	mEngine.setCurrentLayer( mpLayers->currentRow() );
	//clear les sprites... Ils seront repeuplés par updateUi
  mpSprites->clear(); mpSprites->setRowCount(0);
	updateUi();
}

//-----------------------------------------------------------------------------
void MainWindow::newMapClicked()
{
	QDialog d;
  QVBoxLayout* l = new QVBoxLayout( &d );
  l->setMargin(5); l->setSpacing(2);
  
  QLineEdit* pName;
  QHBoxLayout* pLine1 = new QHBoxLayout();
  {
  	QLabel* pl = new QLabel( "nom:", &d );
    pName = new QLineEdit( "stage", &d );
    
    pLine1->addWidget(pl);
    pLine1->addStretch(1);
    pLine1->addWidget(pName);
  }
  
  QSpinBox *pSizeX, *pSizeY;
  QHBoxLayout* pLine2 = new QHBoxLayout();
  {
  	QLabel* pl = new QLabel( "taille:", &d );
    pSizeX = new QSpinBox( &d );
    pSizeX->setMinimum( 1 );
    pSizeX->setMaximum( 10000 );
    pSizeX->setValue( 200 );
    pSizeY = new QSpinBox( &d );
    pSizeY->setMinimum( 1 );
    pSizeY->setMaximum( 10000 );
    pSizeY->setValue( 100 );
    
    pLine2->addWidget(pl);
    pLine2->addStretch(1);
    pLine2->addWidget(pSizeX);
    pLine2->addWidget(pSizeY);
  }
  
  QHBoxLayout* pOkCancel = new QHBoxLayout();
  {
  	QPushButton* pOk = new QPushButton( "Ok", &d );
    connect( pOk, SIGNAL( clicked() ), &d, SLOT( accept() ) );
    QPushButton* pCancel = new QPushButton( "Cancel", &d );
    connect( pCancel, SIGNAL( clicked() ), &d, SLOT( reject() ) );
    
  	pOkCancel->addStretch(1);
    pOkCancel->addWidget(pOk);
    pOkCancel->addWidget(pCancel);
  }
  
  l->addLayout(pLine1);
  l->addLayout(pLine2);
  l->addLayout(pOkCancel);
  
  pName->setFocus();
  if( d.exec() == QDialog::Accepted )
  {
  	QString name = pName->text().isNull() ? "noName" : pName->text();
  	mEngine.newStage( name, pSizeX->value(), pSizeY->value() );
  }
  
}

//-----------------------------------------------------------------------------
void MainWindow::openMapClicked()
{
	QString f = QFileDialog::getOpenFileName( this, tr("Open Stage"),
		mEditionPath,
    tr("Stage (*.bin)") );
  if( !f.isNull() )
  {
  	mEditionPath = QDir( f ).dirName();
  	mEngine.loadStage( f );
  }
}

//-----------------------------------------------------------------------------
void MainWindow::populateBackground()
{
	utils::SpriteCatalog& sp = mEngine.getSpriteCatalog();
	if( mpBackground->count() == 0 )
  {
  	mpBackground->addItem( "aucun" );
    for( int i = 0; i < sp.getNumberOfTextures(); ++i )
    {
      mpBackground->addItem( sp.getTextureToken(i) );
    }
  }
}

//-----------------------------------------------------------------------------
void MainWindow::populateLayers()
{
	const Engine::Stage& s = mEngine.getStage();
	if( mpLayers->count() == 0 )
  {
    for( int i = 0; i < s.getNumberOfLayers(); ++i )
    {
    	QListWidgetItem* pItem = new QListWidgetItem( "Layer " + 
      	QString::number(i), mpLayers );
      pItem->setFlags( pItem->flags() | Qt::ItemIsUserCheckable );
      pItem->setCheckState( Qt::Checked );
    	mpLayers->addItem( pItem );
    }
    
    mpLayers->setCurrentRow(0);
  }	
}

//-----------------------------------------------------------------------------
void MainWindow::populateSprites()
{  
  if( mpSprites->rowCount() == 0 )
  {
  	int layerIndex = mpLayers->currentRow();
    vector<QString> st = mEngine.getStage().getTokens( layerIndex );
  	mpSprites->setRowCount( ceil(st.size() / (double)kSpriteTableWidth) );
    mpSprites->setColumnCount( kSpriteTableWidth );
		for( int i = 0; i < mpSprites->columnCount(); ++i )
      mpSprites->horizontalHeader()->resizeSection( i, 32 );
    for( int i = 0; i < mpSprites->rowCount(); ++i )
	    mpSprites->verticalHeader()->resizeSection( i, 32 );

    for( int i = 0; i < (int)st.size(); ++i )
    {
      QLabel* l = new QLabel();
      QPixmap pm; 
      pm.convertFromImage( mEngine.getSpriteCatalog().getSprite( st[i] ).
      	asQImage() );
      l->setPixmap( pm );
      mpSprites->setCellWidget( i / kSpriteTableWidth, 
      	i % kSpriteTableWidth, l );
    }
  }
}

//-----------------------------------------------------------------------------
void MainWindow::removeSpriteFromLayerClicked()
{
}

//-----------------------------------------------------------------------------
void MainWindow::spriteSelectionChanged(int i, int j)
{ 
	int currentLayer = mpLayers->currentRow();
	vector<QString> tokens = mEngine.getStage().getTokens( currentLayer );
  
	mEngine.setEditingSpriteToken( tokens[ i * kSpriteTableWidth + j ] );
}


//-----------------------------------------------------------------------------
void MainWindow::updateUi()
{
	switch( mEngine.getState() )
  {
  case Engine::sEditing: 
  	mpEditionPanel->show();
    break;
  default:
  	mpEditionPanel->hide();
    break;
  }
  
  if( mpEditionPanel->isVisible() )
  {
  	populateLayers();
    populateSprites();
  	populateBackground();
    
    //selection du type de cellule
    mpCellType->setCurrentIndex( mEngine.getEditingTool() );
    
    //selection du backgound
    int index = mpBackground->findText( mEngine.getStage().getBackgroundToken() );
    mpBackground->setCurrentIndex( index );
  }
}
