
#include "3d/Camera.h"
#include "3d/Utilities.h"
#include "MainDialog.h"
#include "math/MathUtils.h"
#include "math/PlatonicSolid.h"
#include "QColorDialog"
#include "QComboBox"
#include "QFrame"
#include "QLabel"
#include "QLayout"
#include "QLineEdit"
#include "QListWidget"
#include "QPushButton"
#include "QSlider"

using namespace realisim;
  using namespace math;
  using namespace treeD;
  
static const int kIterationInterval = 30; //ms

realisim::treeD::Particules MainDialog::mDummyParticules;

MainDialog::MainDialog() : QMainWindow(),
  mpViewer(0)
{
	createUi();
  updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::addClicked()
{
	Particules p;
  p.setNumberOfParticules( 200 );
  p.setImage( QImage( ":./images/particule.png" ) );
  mParticules.push_back( p );
  updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::createUi()
{
  resize(1200, 600);
  
  QFrame* pMainFrame = new QFrame(this);
  setCentralWidget(pMainFrame);
  
  QHBoxLayout* pLyt = new QHBoxLayout(pMainFrame);
  pLyt->setMargin(2);
  pLyt->setSpacing(5);
  
  //--- on bricole le panneau de gauche
  QFrame* pLeftPanel = new QFrame(pMainFrame);
  QVBoxLayout* pLeftPanelLyt = new QVBoxLayout( pLeftPanel );  
  pLeftPanelLyt->setMargin( 2 );
  pLeftPanelLyt->setSpacing( 5 );
  {
    //la liste de source
  	mpParticules = new QListWidget( pLeftPanel );
    connect( mpParticules, SIGNAL( itemClicked ( QListWidgetItem*) ),
    	this, SLOT( updateUi() ) );
    QHBoxLayout* pAddRemoveLyt = new QHBoxLayout();
    {
    	QPushButton* pAdd = new QPushButton( "add", pLeftPanel );
      connect( pAdd, SIGNAL( clicked() ), this, SLOT( addClicked() ) );
      QPushButton* pRemove = new QPushButton( "remove", pLeftPanel );
      connect( pRemove, SIGNAL( clicked() ), this, SLOT( removeClicked() ) );
      
      pAddRemoveLyt->addStretch( 1 );
      pAddRemoveLyt->addWidget( pAdd );
      pAddRemoveLyt->addWidget( pRemove );
    }
    
    //--- les parametres de la source choisi
    //-- couleur
    QHBoxLayout* pColorLyt = new QHBoxLayout();
    {
    	mpColorButton = new QPushButton( pLeftPanel );
      connect( mpColorButton, SIGNAL( clicked() ), 
      	this, SLOT( colorClicked() ) );
      pColorLyt->addWidget( mpColorButton );
    }
    
    //-- type
    mpType = new QComboBox( pLeftPanel );
    connect( mpType, SIGNAL( activated( int ) ),
    	this, SLOT( typeChanged( int ) ) );
    mpType->insertItem( Particules::tOmniDirectional, "omnidirectional" );
    mpType->insertItem( Particules::tDirectional, "directional" );
    mpType->insertItem( Particules::tCone, "cone" );
    mpType->insertItem( Particules::tDisk, "disk" );
    
    //-- nombre de particule
    QHBoxLayout* pNumPartsLyt = new QHBoxLayout();
    {
    	QLabel* pLabel = new QLabel( "num. particules:", pLeftPanel );
      
      mpNumberOfParticules = new QLineEdit( pLeftPanel );
      QIntValidator* pVal1 = new QIntValidator( mpNumberOfParticules );
      pVal1->setBottom( 0 ); pVal1->setTop( 500000 );
      mpNumberOfParticules->setValidator( pVal1 );
      connect( mpNumberOfParticules, SIGNAL( textChanged( const QString& ) ),
        this, SLOT( numberOfParticulesChanged( const QString& ) ) );
      
      pNumPartsLyt->addWidget( pLabel );
      pNumPartsLyt->addWidget( mpNumberOfParticules );
    }
    
    //-- life range
    QHBoxLayout* pLifeLyt = new QHBoxLayout();
    {
    	QLabel* pLabel = new QLabel( "life range (ms):", pLeftPanel );
      
      mpLifeLowerBound = new QLineEdit( pLeftPanel );
      QDoubleValidator* pVal1 = new QDoubleValidator( mpLifeLowerBound );
      pVal1->setBottom( 0 ); pVal1->setTop( 500000 );
      mpLifeLowerBound->setValidator( pVal1 );
      connect( mpLifeLowerBound, SIGNAL( textChanged( const QString& ) ),
        this, SLOT( lifeLowerRangeChanged( const QString& ) ) );
      
      mpLifeUpperBound = new QLineEdit( pLeftPanel );
      QDoubleValidator* pVal2 = new QDoubleValidator( mpLifeUpperBound );
      pVal2->setBottom( 0 ); pVal2->setTop( 500000 );
      mpLifeUpperBound->setValidator( pVal2 );
      connect( mpLifeUpperBound, SIGNAL( textChanged( const QString& ) ),
        this, SLOT( lifeUpperRangeChanged( const QString& ) ) );
      
      pLifeLyt->addWidget( pLabel );
      pLifeLyt->addWidget( mpLifeLowerBound );
      pLifeLyt->addWidget( mpLifeUpperBound );
    }

		//-- velocity range
    QHBoxLayout* pVelocityLyt = new QHBoxLayout();
    {
    	QLabel* pLabel = new QLabel( "vel. range (m/s):", pLeftPanel );
      
      mpVelocityLowerBound = new QLineEdit( pLeftPanel );
      QDoubleValidator* pVal1 = new QDoubleValidator( mpVelocityLowerBound );
      pVal1->setBottom( 0 ); pVal1->setTop( 500000 );
      mpVelocityLowerBound->setValidator( pVal1 );
      connect( mpVelocityLowerBound, SIGNAL( textChanged( const QString& ) ),
        this, SLOT( velocityLowerRangeChanged( const QString& ) ) );
      
      mpVelocityUpperBound = new QLineEdit( pLeftPanel );
      QDoubleValidator* pVal2 = new QDoubleValidator( mpVelocityUpperBound );
      pVal2->setBottom( 0 ); pVal2->setTop( 500000 );
      mpVelocityUpperBound->setValidator( pVal2 );
      connect( mpVelocityUpperBound, SIGNAL( textChanged( const QString& ) ),
        this, SLOT( velocityUpperRangeChanged( const QString& ) ) );
      
      pVelocityLyt->addWidget( pLabel );
      pVelocityLyt->addWidget( mpVelocityLowerBound );
      pVelocityLyt->addWidget( mpVelocityUpperBound );
    }

    //-- rayon
    QHBoxLayout* pRadiusLyt = new QHBoxLayout();
    {
    	QLabel* pLabel = new QLabel( "radius:", pLeftPanel );
      
      mpRadiusSlider = new QSlider( Qt::Horizontal );
      mpRadiusSlider->setMinimum( 0 ); mpRadiusSlider->setMaximum( 100 );
      connect( mpRadiusSlider, SIGNAL( valueChanged( int ) ),
        this, SLOT( radiusChanged( int ) ) );
      
      pRadiusLyt->addWidget( pLabel );
      pRadiusLyt->addWidget( mpRadiusSlider );
    }
    
    //-- rate
    QHBoxLayout* pRateLyt = new QHBoxLayout();
    {
    	QLabel* pLabel = new QLabel( "rate:", pLeftPanel );
      
      mpRateSlider = new QSlider( Qt::Horizontal );
      mpRateSlider->setMinimum( 0 ); mpRateSlider->setMaximum( 10000 );
      connect( mpRateSlider, SIGNAL( valueChanged( int ) ),
        this, SLOT( rateChanged( int ) ) );
      
      pRateLyt->addWidget( pLabel );
      pRateLyt->addWidget( mpRateSlider );
    }
    
    //-- size
    QHBoxLayout* pSizeLyt = new QHBoxLayout();
    {
    	QLabel* pLabel = new QLabel( "size:", pLeftPanel );
      
      mpSizeSlider = new QSlider( Qt::Horizontal );
      mpSizeSlider->setMinimum( 0 ); mpSizeSlider->setMaximum( 40 );
      connect( mpSizeSlider, SIGNAL( valueChanged( int ) ),
        this, SLOT( sizeChanged( int ) ) );
      
      pRateLyt->addWidget( pLabel );
      pRateLyt->addWidget( mpSizeSlider );
    }
    
    pLeftPanelLyt->addWidget( mpParticules );
    pLeftPanelLyt->addLayout( pAddRemoveLyt );
    pLeftPanelLyt->addWidget( mpType );
    pLeftPanelLyt->addLayout( pColorLyt );
    pLeftPanelLyt->addLayout( pNumPartsLyt );
    pLeftPanelLyt->addLayout( pLifeLyt );
    pLeftPanelLyt->addLayout( pVelocityLyt );
    pLeftPanelLyt->addLayout( pRadiusLyt );
    pLeftPanelLyt->addLayout( pRateLyt );
    pLeftPanelLyt->addLayout( pSizeLyt );
  }
  
  //ajoute le left panel au layout principale
  pLyt->addWidget(pLeftPanel, 1);
  
  //--- on ajoute le viewer
  Particules p;
  p.setColor( 128, 236, 15, 65 );
	p.setNumberOfParticules( 2000 );
  p.setVelocityRange( 5, 5 );
  p.setLifeRange( 500, 4000 );
  p.enableDecay( true );
  mParticules.push_back( p );
	mpParticules->setCurrentRow( -1 );

  mpViewer = new Viewer(pMainFrame, mParticules);
  //mpViewer->setCameraOrientation(Camera::FREE);
  Camera c = mpViewer->getCamera();
  c.setWindowSize(200, 200);
  c.setProjection(200, 0.5, 200);
  mpViewer->setCamera( c );
  pLyt->addWidget(mpViewer, 4);
}
//-----------------------------------------------------------------------------
void MainDialog::colorClicked()
{
  QColor c = QColorDialog::getColor( getSelectedSource().getColor(), this,
    "Choose color", QColorDialog::ShowAlphaChannel );
  getSelectedSource().setColor( c.red(), c.green(), c.blue(), c.alpha() );
	updateUi();
}
//-----------------------------------------------------------------------------
Particules& MainDialog::getSelectedSource()
{
	Particules* r = &mDummyParticules;
  QList<QListWidgetItem *> selected = mpParticules->selectedItems();
  if( !selected.empty() )
  { r = &mParticules[ mpParticules->row( selected[0] ) ]; }
  return *r;
}
//-----------------------------------------------------------------------------
void MainDialog::lifeLowerRangeChanged( const QString& iText )
{
  double ur = getSelectedSource().getLifeRange().second;
  getSelectedSource().setLifeRange( iText.toDouble(), ur );
  updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::lifeUpperRangeChanged( const QString& iText )
{
  double lr = getSelectedSource().getLifeRange().first;
  getSelectedSource().setLifeRange( lr, iText.toDouble() );
}
//-----------------------------------------------------------------------------
void MainDialog::numberOfParticulesChanged( const QString& iText )
{
  getSelectedSource().setNumberOfParticules( iText.toInt() );
  updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::radiusChanged( int iV )
{
	getSelectedSource().setRadius( iV );
  updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::rateChanged( int iV )
{
  getSelectedSource().setRate( iV );
  updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::removeClicked()
{
  QList<QListWidgetItem *> selected = mpParticules->selectedItems();
  if( !selected.empty() )
  { 
  	int i = mpParticules->row( selected[0] );
    mParticules.erase( mParticules.begin() + i );
  }
	updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::sizeChanged( int iV )
{
  getSelectedSource().setSize( iV );
  updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::typeChanged( int iType )
{
  getSelectedSource().setType( (Particules::type)iType );
	updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::updateUi()
{
	//liste des particules
	QString s;
	if( (int)mParticules.size() != mpParticules->count() )
  {
  	mpParticules->clear();
    for( int i = 0; i < (int)mParticules.size(); ++i )
    {
      s = "source " + QString::number( i );
      mpParticules->insertItem( i, s );
    }
  }
	
  //parametre
  Particules& source = getSelectedSource();
  QImage imageColor( 32, 32, QImage::Format_RGB32 );
  imageColor.fill( source.getColor().rgb() );
  QPixmap pix;
  pix.convertFromImage( imageColor );
  mpColorButton->setIcon( pix );
  mpNumberOfParticules->setText( QString::number( source.getNumberOfParticules() ) );
  mpType->setCurrentIndex( source.getType() );
  mpLifeLowerBound->setText( QString::number( source.getLifeRange().first ) );
  mpLifeUpperBound->setText( QString::number( source.getLifeRange().second ) );
  mpVelocityLowerBound->setText( QString::number( source.getVelocityRange().first ) );
  mpVelocityUpperBound->setText( QString::number( source.getVelocityRange().second ) );
  mpRadiusSlider->setValue( source.getRadius() );
  mpRateSlider->setValue( source.getRate() );
  mpSizeSlider->setValue( source.getSize() );
  
	mpViewer->update();
}
//------------------------------------------------------------------------------
void MainDialog::velocityLowerRangeChanged( const QString& iT )
{
  double ur = getSelectedSource().getVelocityRange().second;
  getSelectedSource().setVelocityRange( iT.toDouble(), ur );
  updateUi();
}
//------------------------------------------------------------------------------
void MainDialog::velocityUpperRangeChanged( const QString& iT )
{
  double lr = getSelectedSource().getVelocityRange().first;
  getSelectedSource().setVelocityRange( lr, iT.toDouble() );
  updateUi();
}
//------------------------------------------------------------------------------
//--- Viewer
//------------------------------------------------------------------------------
Viewer::Viewer( QWidget* ipW,
	const std::vector< realisim::treeD::Particules >& iP ) :
	Widget3d( ipW ),
  mParticules( iP ),
  mTimerId(0)  
{
  mTimerId = startTimer(kIterationInterval);  
}

Viewer::~Viewer()
{}
//------------------------------------------------------------------------------
void Viewer::paintGL()
{
	Widget3d::paintGL();

	for( int i = 0; i < (int)mParticules.size(); ++i )
  {
  	mParticules[i].draw();
  }
}
//------------------------------------------------------------------------------
void Viewer::initializeGL()
{
	Widget3d::initializeGL();
}
//------------------------------------------------------------------------------
void Viewer::timerEvent(QTimerEvent* ipEvent)
{ update(); }
