/* */

#include "3d/Utilities.h"
#include "MainDialog.h"
#include <qlayout.h>
#include <QFile.h>
#include "utils/utilities.h"

using namespace realisim;
  using namespace math;
  using namespace treeD;
  
namespace
{
	const QString kShowAlphaShader = "#120\n"
  	"uniform sampler2D texture;\n"
    "uniform int textureWidth;"
    "uniform int textureHeight;"
    "void main()"
    "{\n"
    "ivec2 pixel = ivec2( gl_TexCoord[0].x * float(textureWidth), gl_TexCoord[0].y * float(textureHeight) );\n"
    "vec4 color = texture2D(texture, gl_TexCoord[0].xy);\n"
    "if(color.a == 0.0)"
    "{\n"
    	"bool on = false;"
    	"if( int(mod( float(pixel.x / 10), 2.0 )) == 0 )\n"
      	"on = true;"
      "if( int(mod( float(pixel.y / 10), 2.0 )) != 0 )\n"
      	"on = !on;"
      "if(on) color = vec4(0.84, 0.84, 0.84, 1.0);\n"
    "}"
    "gl_FragColor = color;\n"
    "}";
}  

Viewer::Viewer(MainDialog& m, QWidget* ipParent, const QGLWidget* ipShared /*=0*/) : 
	Widget3d(ipParent, ipShared),
  mMain(m),
  mState(sNavigation),
  mMouseState(msIdle),
  mMousePos(0),
  mMouseDelta(0),
  mDragDelta(0),
  mHoverId(-1),
  mSelectedId(-1),
  mStateButton()
{ 
	setMouseTracking(true);
  QFont f = mStateButton.getFont();
  f.setPointSize( 12 );
  mStateButton.setFont( f );
}

//-----------------------------------------------------------------------------
void Viewer::drawModeButton(bool iPicking /*=false*/) const
{  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  Camera cam;
  cam.set( Point3d(0.0, 0.0, 5.0), Point3d(0.0), Vector3d(0.0, 1.0, 0.0) );
  cam.setWindowSize( width(), height() );
  cam.setProjection( 0, width(), 
    0, height(), 0.5, 100.0,
    Camera::ORTHOGONAL );
  cam.applyModelViewTransformation();
  cam.applyProjectionTransformation();
  
  switch(mState)
  {
  	case sNavigation: mStateButton.setText("Navigation"); break;
    case sEdition: mStateButton.setText("Edition"); break;
    case sPreview: mStateButton.setText("Preview"); break;
  }
  
  glTranslated(10, 10, 0);
  QColor c(128, 128, 128, 255);
  if(!iPicking)
  {
  	mStateButton.setBackgroundColor(c);
  	mStateButton.draw();
  }

	glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

//-----------------------------------------------------------------------------
void Viewer::drawSceneForPicking() const
{
	if( !isPreviewing() )
  {
		drawSprites(true);
  	drawModeButton(true);
  }
}

//-----------------------------------------------------------------------------
void Viewer::drawSprites( bool iPicking /*=false*/ ) const
{
  Texture t = mMain.mSpriteCatalog.getTexture( mMain.mTextureToken );
  vector<QString> sprites = mMain.mSpriteCatalog.getSpritesForTexture(
  	mMain.mTextureToken );
  
  for( size_t i = 0; i < sprites.size(); ++i  )
  {
  	bool isCurrentSprite = sprites[i] == mMain.mSpriteToken;
    Sprite& s = mMain.mSpriteCatalog.getSprite( sprites[i] );
    if( !s.getRect().isNull() )
    {
      QRect r = s.getRect();
      glColor3ub(0, 0, 0);
      isCurrentSprite ? glLineWidth(2.0) : glLineWidth(1.0);      
      if(!iPicking)
      {
      	//--- le rectangle qui defini le sprite
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);      
        //conversion de qt vers gl
        Point2d p( r.bottomLeft().x(), t.height() - r.bottomLeft().y() );
        Vector2d size( r.width(), r.height() );
        drawRectangle2d(p, size);        
        
        //--- les rectangles qui définissent les frames
        Point2d p2;
        for( int c = 0; c < s.getFrameGrid().x(); ++c )
        	for( int r = 0; r < s.getFrameGrid().y(); ++r )
          {
          	p2.setX( p.x() + (c * s.getFrameSize().x() ) );
            p2.setY( p.y() + (r * s.getFrameSize().y() ) );
            drawRectangle2d(p2, s.getFrameSize() );
          }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }
      
      //dessine les tites poignees sur le sprite courant
      if( isCurrentSprite )
      {
        glPointSize(4.0);
        QColor c(0, 0, 0);
        glColor4ub( c.red(), c.green(), c.blue(), c.alpha() );
        glBegin(GL_POINTS);
        if( !iPicking )
        {
          glVertex2d( r.topLeft().x(), t.height() - r.topLeft().y() + 1 );
          glVertex2d( r.topRight().x() + 1, t.height() - r.topRight().y() + 1 );
          glVertex2d( r.bottomRight().x() + 1, t.height() - r.bottomRight().y() );
          glVertex2d( r.bottomLeft().x(), t.height() - r.bottomLeft().y() );      
        }
        else 
        {
          c = idToColor(pTopLeftHandle);
          glColor4ub( c.red(), c.green(), c.blue(), c.alpha() );
          glVertex2d( r.topLeft().x(), t.height() - r.topLeft().y() + 1 );
          
          c = idToColor(pTopRightHandle);
          glColor4ub( c.red(), c.green(), c.blue(), c.alpha() );
          glVertex2d( r.topRight().x() + 1, t.height() - r.topRight().y() + 1 );
          
          c = idToColor(pBottomRightHandle);
          glColor4ub( c.red(), c.green(), c.blue(), c.alpha() );
          glVertex2d( r.bottomRight().x() + 1, t.height() - r.bottomRight().y() );
          
          c = idToColor(pBottomLeftHandle);
          glColor4ub( c.red(), c.green(), c.blue(), c.alpha() );
          glVertex2d( r.bottomLeft().x(), t.height() - r.bottomLeft().y() );      
        }
        glEnd();
      }
    }

  }  
}

//-----------------------------------------------------------------------------
//QColor Viewer::getColor( colors iC ) const
//{
//	QColor r( 0, 0, 0);
//	switch (iC) 
//  {
//    case cHover: r = QColor("blue"); break;
//    case cSelect: r = QColor("yellow"); break;
//    default: break;
//  }
//  return r;
//}

//-----------------------------------------------------------------------------
void Viewer::handleDrag()
{
  switch (mState) 
  {
    case sEdition:
    {
      const Camera& c = getCamera();
      Point3d glMousePos = c.pixelToGL( mMousePos.x(), mMousePos.y() );  
      Sprite& s = mMain.mSpriteCatalog.getSprite( mMain.mSpriteToken );
      Texture& t = mMain.mSpriteCatalog.getTexture( mMain.mTextureToken );
      QPoint p( round(glMousePos.getX()),
        t.height() - round(glMousePos.getY()) );
      QRect r = s.getRect();

      switch(mSelectedId)
      {
        case pTopLeftHandle: r.setTopLeft( p ); break;
        case pTopRightHandle:r.setTopRight( p ); break;
        case pBottomLeftHandle: r.setBottomLeft( p ); break;
        case pBottomRightHandle: r.setBottomRight( p ); break;
        default: break;
      }
      s.setRect(r);
    }
    break;
    case sNavigation:
    {
      Vector3d delta = getCamera().pixelDeltaToGLDelta( mMouseDelta.x(),
      	-mMouseDelta.y() );
      mCam.move( -delta );
    }    
    break;
    default:break;
  }
}

//-----------------------------------------------------------------------------
void Viewer::handleSelection()
{
	switch (mSelectedId) 
  {
    case pTopLeftHandle:
    case pTopRightHandle:
    case pBottomLeftHandle:
    case pBottomRightHandle: 
    	switch (mState) 
      {
        case sNavigation: mState = sEdition; break;
        default: break;
      }
    break;
    default:
      switch (mState) 
      {
        case sEdition: mState = sNavigation; break;
        default: break;
      }
    break;
  }
}

//-----------------------------------------------------------------------------
void Viewer::initializeGL()
{
	Widget3d::initializeGL();
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  
printf("frag source %s\n", kShowAlphaShader.toStdString().c_str() );
  mShowAlphaShader.addFragmentSource( kShowAlphaShader );
  //mShowAlphaShader.addVertexSource( kShowAlphaShaderV );
  mShowAlphaShader.link();
}

//-----------------------------------------------------------------------------
bool Viewer::isPreviewing() const
{ return mState == sPreview; }

//-----------------------------------------------------------------------------
void Viewer::mouseMoveEvent( QMouseEvent* e )
{
  vector<unsigned int> picked = pick(mMousePos.x(), mMousePos.y(), 5, 5);
  if( !picked.empty() )
  {
  	mHoverId = picked[0];
    switch (mHoverId) 
    {
      case pTopLeftHandle:
      case pBottomRightHandle:
		  	setCursor(QCursor(Qt::SizeFDiagCursor));
      break;
      case pTopRightHandle:
      case pBottomLeftHandle:
      	setCursor(QCursor(Qt::SizeBDiagCursor));
      break;
      default:
		    unsetCursor();
      break;
    }
  }
  else
  {
	  mHoverId = -1;
  	unsetCursor();
  }

	mMouseDelta = Vector2i(e->x() - mMousePos.x(), e->y() - mMousePos.y());
  switch (mMouseState) 
  {
    case msIdle: mMouseState = msMove; break;
    case msDown: mMouseState = msDrag; break;
    case msMove: break;
    case msDrag:
    {
      mDragDelta += mMouseDelta;
      handleDrag();
    }
    break;
    default: break;
  }
  mMousePos.set( e->x(), e->y() );  
  
  update();
  
printf( "mMouseState %d, mMousePos %d, %d, mDragDelta %d, %d"
	"hoverId %d, selectedId %d\n",
	mMouseState, mMousePos.x(), mMousePos.y(), mDragDelta.x(), mDragDelta.y(),
  mHoverId, mSelectedId );
}

//-----------------------------------------------------------------------------
void Viewer::mousePressEvent( QMouseEvent* e )
{	
  switch (mMouseState) 
  {
    case msIdle: 
      mMouseState = msDown;
      mDragDelta.set(0.0);          
      break;
    case msDown: break;
    case msMove:
      mMouseState = msDown;
      mDragDelta.set(0.0);
      break;
    case msDrag: break;
    default: break;
  }

  mSelectedId = mHoverId;
  handleSelection();
  update();
printf( "mMouseState %d, mMousePos %d, %d, mDragDelta %d, %d"
	"hoverId %d, selectedId %d\n",
	mMouseState, mMousePos.x(), mMousePos.y(), mDragDelta.x(), mDragDelta.y(),
  mHoverId, mSelectedId );
}

//-----------------------------------------------------------------------------
void Viewer::mouseReleaseEvent( QMouseEvent* e )
{
  switch (mMouseState) 
  {
    case msIdle: break;
    case msDown:
      mMouseState = msIdle;
      break;
    case msMove: break;
    case msDrag:
      mMouseState = msIdle;
      break;
    default: break;
  }

  update();
printf( "mMouseState %d, mMousePos %d, %d, mDragDelta %d, %d"
	"hoverId %d, selectedId %d\n",
	mMouseState, mMousePos.x(), mMousePos.y(), mDragDelta.x(), mDragDelta.y(),
  mHoverId, mSelectedId );
}
  
//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
	Widget3d::paintGL();
  
  if( isPreviewing() )
  {
  	const Sprite& s = mMain.mSpriteCatalog.getSprite( mMain.mSpriteToken );
    s.draw();  
  }
  else
  {
    //dessine la texture courante
    Texture t = mMain.mSpriteCatalog.getTexture( mMain.mTextureToken );  
    pushShader( mShowAlphaShader );
    mShowAlphaShader.setUniform("textureWidth", t.width());
    mShowAlphaShader.setUniform("textureHeight", t.height());
    glColor3ub(255, 255, 255);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    drawRectangle2d( t, Point2d(0.0), Vector2i(t.width(), t.height()) );
    popShader();
    
    //dessine les sprites pour la texture courante
    drawSprites();
    
    //On dessin le tit bouton pour le mode
    drawModeButton();
  }
}

//-----------------------------------------------------------------------------
void Viewer::setAsPreview(bool iP)
{ mState = iP ? sPreview : sNavigation; }

//-----------------------------------------------------------------------------
// --- MAIN WINDOW
//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QMainWindow(),
	mTextureToken(),
  mSpriteToken(),
  mSaveFileName(),
  mPreviewTimer(0)
{
  resize(1024, 600);
  move( 10, 10 );
  
  QWidget* pMainWidget = new QWidget( this );
  setCentralWidget(pMainWidget);
  
  //--- la barre de menu
  QMenuBar* pMenuBar = new QMenuBar(pMainWidget);
  QMenu* pFile = pMenuBar->addMenu("Fichier");
  pFile->addAction( QString("&Ouvrir..."), this, SLOT( openCatalogClicked() ),
  	QKeySequence(Qt::CTRL + Qt::Key_O) );
  pFile->addAction( QString("&Fermer"), this, SLOT( closeCatalogClicked() ) );
  pFile->addAction( QString("&Enregistrer"), this, SLOT( save() ),
  	QKeySequence(Qt::CTRL + Qt::Key_S) );
  pFile->addAction( QString("Enregistrer sous..."), this, SLOT( saveAs() ),
  	QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S) );
  
  setMenuBar( pMenuBar );
  
  //--- le reste du Ui
  QHBoxLayout* pLyt = new QHBoxLayout(pMainWidget);
  pLyt->setMargin(5);
  pLyt->setSpacing(5);
  
  mpViewer = new Viewer(*this, pMainWidget);
  Camera c = mpViewer->getCamera();
  c.setOrientation(Camera::XY);
  mpViewer->setCamera(c);
  
  //--- panneau de controle
  QTabWidget* pTabs = new QTabWidget(pMainWidget);
  {
  	QFrame* pTextureFrame = new QFrame();
    pTabs->insertTab(ttTexture, pTextureFrame, "Textures");
    {
    	QVBoxLayout* pVlyt = new QVBoxLayout(pTextureFrame);
      pVlyt->setSpacing(5); pVlyt->setMargin(5);
      {
      	mpTextures = new QListWidget( pTextureFrame );
        mpTextures->setAlternatingRowColors(true);
        connect( mpTextures, SIGNAL(currentRowChanged(int)),
        	this, SLOT( textureSelectionChanged(int) ) );

				QHBoxLayout* pAddRemoveLyt = new QHBoxLayout();
        {
        	mpAddTexture = new QPushButton( "Ajouter...", pTextureFrame );
          connect( mpAddTexture, SIGNAL(clicked()),
          	this, SLOT( addTextureClicked() ) );
          mpRemoveTexture = new QPushButton( "Effacer", pTextureFrame ); 
          connect( mpRemoveTexture, SIGNAL(clicked()),
          	this, SLOT( removeTextureClicked() ) );
        	pAddRemoveLyt->addStretch(1);
          pAddRemoveLyt->addWidget(mpAddTexture);
          pAddRemoveLyt->addWidget(mpRemoveTexture);
        }
        
        pVlyt->addWidget(mpTextures);        
        pVlyt->addStretch(1);
        pVlyt->addLayout( pAddRemoveLyt );
      }
    }

  	QFrame* pSpriteFrame = new QFrame();
    pTabs->insertTab(ttSprite, pSpriteFrame, "Sprites");
    {
    	QVBoxLayout* pVlyt = new QVBoxLayout(pSpriteFrame);
      pVlyt->setSpacing(5); pVlyt->setMargin(5);
      {
      	mpSprites = new QListWidget( pSpriteFrame );
        mpSprites->setAlternatingRowColors(true);
        connect( mpSprites, SIGNAL(currentRowChanged(int)),
        	this, SLOT( spriteSelectionChanged(int) ) );
          
        QVBoxLayout* pControlsLyt = new QVBoxLayout();
        {
        	QHBoxLayout* pLine1 = new QHBoxLayout();
          {
          	QLabel* l = new QLabel( "duration:", pSpriteFrame );
            QSpinBox* s = new QSpinBox(pSpriteFrame);
            s->setRange( 1, 10000 );
            connect( s, SIGNAL(valueChanged(int)),
            	this, SLOT(durationChanged(int)) );
            pLine1->addWidget(l);
            pLine1->addStretch(1);
            pLine1->addWidget(s);
          }
          
          QHBoxLayout* pLine2 = new QHBoxLayout();
          {
          	QLabel* l = new QLabel( "frame grid:", pSpriteFrame );
            QSpinBox* s = new QSpinBox(pSpriteFrame);
            s->setRange(1, 100);            
            connect( s, SIGNAL(valueChanged(int)),
            	this, SLOT(frameGridXChanged(int)) );
            QSpinBox* s2 = new QSpinBox(pSpriteFrame);
            s2->setRange(1, 100);
            connect( s2, SIGNAL(valueChanged(int)),
            	this, SLOT(frameGridYChanged(int)) );
            pLine2->addWidget(l);
            pLine2->addStretch(1);
            pLine2->addWidget(s);
            pLine2->addWidget(s2);
          }
          
          QHBoxLayout* pLine3 = new QHBoxLayout();
          {
          	QLabel* l = new QLabel( "nombres de frames:", pSpriteFrame );
            QSpinBox* s = new QSpinBox(pSpriteFrame);
            s->setRange(1, 1000);            
            connect( s, SIGNAL(valueChanged(int)),
            	this, SLOT(numberOfFramesChanged(int)) );
            pLine3->addWidget(l);
            pLine3->addStretch(1);
            pLine3->addWidget(s);
          }
          
          QHBoxLayout* pLine4 = new QHBoxLayout();
          {
          	QLabel* l = new QLabel( "looping:", pSpriteFrame );
            QCheckBox* c = new QCheckBox(pSpriteFrame);
            connect( c, SIGNAL(stateChanged(int)),
            	this, SLOT(loopingChanged(int)) );
            pLine4->addWidget(l);
            pLine4->addStretch(1);
            pLine4->addWidget(c);
          }
          
          QHBoxLayout* pLine5 = new QHBoxLayout();
          {
            mpPreviewer = new Viewer( *this, pSpriteFrame, mpViewer );
            Camera c = mpPreviewer->getCamera();
					  c.setOrientation(Camera::XY);
					  mpPreviewer->setCamera(c);
						mpPreviewer->setAsPreview(true);
            pLine5->addWidget(mpPreviewer);
          }
          
          pControlsLyt->addLayout(pLine1);
          pControlsLyt->addLayout(pLine2);
          pControlsLyt->addLayout(pLine3);
          pControlsLyt->addLayout(pLine4);
          pControlsLyt->addLayout(pLine5);
        }
        

				QHBoxLayout* pAddRemoveLyt = new QHBoxLayout();
        {
        	mpAddSprite = new QPushButton( "Ajouter...", pSpriteFrame );
          connect( mpAddSprite, SIGNAL(clicked()),
          	this, SLOT( addSpriteClicked() ) );
          mpRemoveSprite = new QPushButton( "Effacer", pSpriteFrame ); 
          connect( mpRemoveSprite, SIGNAL(clicked()),
          	this, SLOT( removeSpriteClicked() ) );
        	pAddRemoveLyt->addStretch(1);
          pAddRemoveLyt->addWidget(mpAddSprite);
          pAddRemoveLyt->addWidget(mpRemoveSprite);
        }
        
        pVlyt->addWidget(mpSprites);
        pVlyt->addLayout( pControlsLyt );
        pVlyt->addStretch(1);
        pVlyt->addLayout( pAddRemoveLyt );
      }
    }    
  }
  
  pLyt->addWidget(mpViewer, 2);
  pLyt->addWidget(pTabs, 1);
  connect(pTabs, SIGNAL(currentChanged(int)), 
	  this, SLOT(tabChanged(int)));
  tabChanged(ttTexture);
}

//-----------------------------------------------------------------------------
void MainDialog::addTextureClicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Add Texture"),
  	"/home", tr("Images (*.png *.bmp *.jpg *.jpeg)"));
  if(!fileName.isEmpty())
  {
    QImage im(fileName);
    Texture t;
    t.set(im);
    QString token = fileName.section("/", -1); //utils::getGuid();
   	mSpriteCatalog.add( token, t );
  }

  if(mTextureToken.isEmpty())
    textureSelectionChanged( 0 );
  updateTextureUi();
}


//-----------------------------------------------------------------------------
void MainDialog::addSpriteClicked()
{
	if( !mTextureToken.isEmpty() )
  {
  	Sprite s;
    Texture t = mSpriteCatalog.getTexture( mTextureToken );
    s.set( t, QRect(10, 10, 20, 30) );
    QString token = utils::getGuid();
  	mSpriteCatalog.add( token, s );
    if(mSpriteToken.isEmpty())
      mSpriteToken = token;
  }
  updateSpriteUi();
}

//-----------------------------------------------------------------------------
void MainDialog::closeCatalogClicked()
{
	mSpriteCatalog.clear();
  mSaveFileName = QString();
  updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::durationChanged(int iD) //ms
{
	Sprite& s = mSpriteCatalog.getSprite( mSpriteToken );
  s.setAnimationDuration(iD);
  updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::frameGridXChanged(int iX)
{
	Sprite& s = mSpriteCatalog.getSprite( mSpriteToken );
  s.setFrameGrid( iX, s.getFrameGrid().y() );
  updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::frameGridYChanged(int iY)
{
  Sprite& s = mSpriteCatalog.getSprite( mSpriteToken );
  s.setFrameGrid( s.getFrameGrid().x(), iY );
  updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::loopingChanged(int iS)
{
	Sprite& s = mSpriteCatalog.getSprite( mSpriteToken );
  switch (iS) 
  {
    case Qt::Checked: s.setAsLooping(true); break;
    case Qt::Unchecked: s.setAsLooping(false); break;
    default:break;
  }
  s.startAnimation();
  updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::numberOfFramesChanged(int iN)
{
  Sprite& s = mSpriteCatalog.getSprite( mSpriteToken );
  s.setNumberOfFrames(iN);
  updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::openCatalogClicked()
{
  QString c = QFileDialog::getOpenFileName(
    	this, tr("Open Catalog"),
      "/home/untitled.cat",
      tr("Sprite Catalog (*.cat)"));
	if( !c.isEmpty() )
  {
  	QByteArray ba = utils::fromFile(c);
		mSpriteCatalog.fromBinary( ba );
    mSaveFileName = c;
  }

  updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::removeSpriteClicked()
{
	int c = mpSprites->currentRow();
	mSpriteCatalog.removeSprite( mSpriteToken );
  if( mpSprites->count() > c + 1 )
  	spriteSelectionChanged(c+1);//mSpriteToken = mpSprites->item( c+1 )->text();
  else if( mpSprites->count() > 1 )
  	spriteSelectionChanged(c-1);//mSpriteToken = mpSprites->item( c-1 )->text();
  else
    mSpriteToken = QString();

	updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::removeTextureClicked()
{
	int c = mpTextures->currentRow();
	mSpriteCatalog.removeTexture( mTextureToken );
  if( mpTextures->count() > c + 1 )
  	textureSelectionChanged(c+1);//mTextureToken = mpTextures->item( c+1 )->text();
  else if( mpTextures->count() > 1 )
  	textureSelectionChanged(c-1);//mTextureToken = mpTextures->item( c-1 )->text();
  else
    mTextureToken = QString();

	updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::save()
{
	if( mSaveFileName.isEmpty() )
  { saveAs(); }
  else
  { utils::toFile( mSaveFileName, mSpriteCatalog.toBinary() ); }
  updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::saveAs()
{
	QString s;
  s = QFileDialog::getSaveFileName(
    this, tr("Save Catalog"),
    "/home/untitled.cat",
    tr("Sprite Catalog (*.cat)"));
      
  if(!s.isEmpty())
  {
  	mSaveFileName = s;
  	utils::toFile( mSaveFileName, mSpriteCatalog.toBinary() );
  }
  updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::spriteSelectionChanged(int i)
{
  mSpriteToken = mpSprites->item(i)->text();
  updateSpriteUi();
  
  const Sprite& s = mSpriteCatalog.getSprite( mSpriteToken );
  Camera c = mpPreviewer->getCamera();
  Vector2i fs = s.getFrameSize();
  c.setOrthoProjection( max( fs.x(), fs.y() ),
  	0.5, 200);
	c.setZoom(1.0);
  Matrix4d m;
  //m.setTranslation( Point3d( -fs.x()/2.0, -fs.y()/2.0, 0.0 ) );
  c.setTransformationToGlobal( m );
  c.set( Point3d(0.0, 0.0, 5.0),
  	Point3d(0.0, 0.0, 0.0),
    Vector3d(0.0, 1.0, 0.0) );
  mpPreviewer->setCamera(c, false);
  mpPreviewer->update();
  
  mpViewer->update();
}

//-----------------------------------------------------------------------------
void MainDialog::tabChanged(int i)
{
	killTimer(mPreviewTimer);
  mPreviewTimer = 0;
	switch (i) 
  {
    case ttSprite: mPreviewTimer = startTimer(30); updateSpriteUi(); break;
    case ttTexture: updateTextureUi();break;
    default: break;
  }
}

//-----------------------------------------------------------------------------
void MainDialog::timerEvent(QTimerEvent* ipE)
{
	if( ipE->timerId() == mPreviewTimer )
  	mpPreviewer->update();
}


//-----------------------------------------------------------------------------
void MainDialog::textureSelectionChanged(int i)
{
	mTextureToken = mSpriteCatalog.getTextureToken(i);
  Texture t = mSpriteCatalog.getTexture( mTextureToken );
  mSpriteToken = QString();
  
  Camera c = mpViewer->getCamera();
  c.setOrthoProjection( max( t.width(), t.height() ),
  	0.5, 200);
	c.setZoom(1.0);
  Matrix4d m;
  m.setTranslation( Point3d( t.width()/2.0, t.height()/2.0, 0.0 ) );
  c.setTransformationToGlobal( m );
  c.set( Point3d(0.0, 0.0, 5.0),
  	Point3d(0.0, 0.0, 0.0),
    Vector3d(0.0, 1.0, 0.0) );
  mpViewer->setCamera(c, false);
  
  updateTextureUi();
  mpViewer->update();
}

//-----------------------------------------------------------------------------
void MainDialog::updateSpriteUi()
{
	mpSprites->blockSignals(true);
	mpSprites->clear();

  vector<QString> sprites = mSpriteCatalog.getSpritesForTexture( mTextureToken );
  for( size_t i = 0; i < sprites.size(); ++i )
  { mpSprites->addItem( sprites[i] ); }
  
  if(mpSprites->findItems( mSpriteToken, Qt::MatchExactly ).count() > 0 )
  {
    mpSprites->setCurrentItem( mpSprites->findItems(mSpriteToken,
      Qt::MatchExactly)[0]);
  }
  mpSprites->blockSignals(false);
  
  mpRemoveSprite->setEnabled( !mSpriteToken.isEmpty() );
}

//-----------------------------------------------------------------------------
void MainDialog::updateTextureUi()
{
	mpTextures->blockSignals(true);
	mpTextures->clear();
  for(int i = 0; i < mSpriteCatalog.getNumberOfTextures(); ++i)
  {
  	mpTextures->addItem( mSpriteCatalog.getTextureToken(i) );
  }
  
  if( mpTextures->findItems(mTextureToken, Qt::MatchExactly).count() > 0 )
  {
  	mpTextures->setCurrentItem( mpTextures->findItems(mTextureToken,
  		Qt::MatchExactly)[0]);
	}
  mpTextures->blockSignals(false);
  
  mpRemoveTexture->setEnabled( !mTextureToken.isEmpty() );
}

//-----------------------------------------------------------------------------
void MainDialog::updateUi()
{
	setWindowTitle( mSaveFileName.section( "/", -1 ) );
  
	updateTextureUi();
  updateSpriteUi();
  mpViewer->update();
}
