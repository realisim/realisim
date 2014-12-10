
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
#include "QMouseEvent"
#include "QPushButton"
#include "QSlider"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace realisim;
  using namespace math;
  using namespace treeD;
  

MainDialog::MainDialog() : QMainWindow(),
  mpViewer(0)
{
	createUi();
  updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::cameraChanged(int iIndex) 
{
	mpViewer->setCamera( mCameras[iIndex] );
  updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::cameraControlChanged(int iControl)
{ 
	mpViewer->setControlType( (Widget3d::controlType)iControl );
  updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::cameraProjectionChanged( int iType )
{
	Camera c = mpViewer->getCamera();
  
  switch ( iType ) 
  {
    case Camera::Projection::tOrthogonal :
    	c.setOrthoProjection( 100, 0.01, 10000 );
      break;
    case Camera::Projection::tPerspective :
      c.setPerspectiveProjection(60, 1, 0.5, 10000.0, true);
      break;
    default: break;
  }
  mpViewer->setCamera(c);
  updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::createUi()
{
  resize(800, 600);
  Camera c1, c2, c3, c4, c5;
  c1.setViewportSize( 800, 600 );
  c1.setPerspectiveProjection( 60, 1, 0.5, 10000, true );
  c1.set( Point3d( 50 ),
  	Point3d( 0.0 ),
    Vector3d( 0.0, 1.0, 0.0 ) );
  c2 = c1;
  c2.set( Point3d( 50, 0.0, 0.0 ),
  	Point3d( 0.0 ),
    Vector3d( 0.0, 1.0, 0.0 ) );
  c3 = c1;
  c3.set( Point3d( 0.0, 0.0, 50 ),
  	Point3d( 0.0 ),
    Vector3d( 0.0, 1.0, 0.0 ) );
  c4 = c1;
  c4.set( Point3d( 0.0, 50, 0.0 ),
  	Point3d( 0.0 ),
    Vector3d( 0.0, 0.0, -1.0 ) );
  c5 = c1;
  c5.setPerspectiveProjection( 120, 1, 0.5, 10000, true );

  mCameras.push_back( c1 );
  mCameras.push_back( c2 );
  mCameras.push_back( c3 );
  mCameras.push_back( c4 );
  mCameras.push_back( c5 );
  
  QFrame* pMainFrame = new QFrame(this);
  setCentralWidget(pMainFrame);
  
  QHBoxLayout* pLyt = new QHBoxLayout(pMainFrame);
  pLyt->setMargin(2);
  pLyt->setSpacing(5);
  
  mpViewer = new Viewer(pMainFrame);
  
  //--- on bricole le panneau de gauche
  QFrame* pLeftPanel = new QFrame(pMainFrame);
  QVBoxLayout* pLeftPanelLyt = new QVBoxLayout( pLeftPanel );  
  pLeftPanelLyt->setMargin( 2 );
  pLeftPanelLyt->setSpacing( 5 );
  {
  	QHBoxLayout* pL0 = new QHBoxLayout();
    pL0->setMargin( 2 ); pL0->setSpacing( 5 );
    {
    	QLabel* pl = new QLabel( pMainFrame );
      pl->setText( "camera" );
      
      mpCameras = new QComboBox( pMainFrame );
      connect( mpCameras, SIGNAL( currentIndexChanged(int) ), 
        this, SLOT( cameraChanged(int) ) );
      
      pL0->addWidget( pl );
      pL0->addWidget( mpCameras );
    }
    
  	QHBoxLayout* pL1 = new QHBoxLayout();
    pL1->setMargin( 2 ); pL1->setSpacing( 5 );
    {
    	QLabel* pl = new QLabel( pMainFrame );
      pl->setText( "type de projection" );
      
      mpCameraProjection = new QComboBox( pMainFrame );
      connect( mpCameraProjection, SIGNAL( currentIndexChanged(int) ), 
        this, SLOT( cameraProjectionChanged(int) ) );
      
      pL1->addWidget( pl );
      pL1->addWidget( mpCameraProjection );
    }
  	
  	QHBoxLayout* pL2 = new QHBoxLayout();
    pL2->setMargin( 2 ); pL2->setSpacing( 5 );
    {
    	QLabel* pl = new QLabel( pMainFrame );
      pl->setText( "type de controle" );
      
      mpCameraControl = new QComboBox( pMainFrame );
      connect( mpCameraControl, SIGNAL( currentIndexChanged(int) ), 
        this, SLOT( cameraControlChanged(int) ) );
      
      pL2->addWidget( pl );
      pL2->addWidget( mpCameraControl );
    }
    
    QHBoxLayout* pL3 = new QHBoxLayout();
    pL3->setMargin( 2 ); pL3->setSpacing( 5 );
    {
    	QLabel* pl = new QLabel( pMainFrame );
      pl->setText( "zoom" );
      
      mpZoom = new QLineEdit( pMainFrame );
			QPushButton* pApply = new QPushButton( "appliquer", pMainFrame );
      connect( pApply, SIGNAL( clicked() ), this, SLOT( zoomChanged() ) );
      
      pL3->addWidget( pl );
      pL3->addWidget( mpZoom );
      pL3->addWidget( pApply );
    }
  
	  pLeftPanelLyt->addLayout(pL0);
	  pLeftPanelLyt->addLayout(pL1);
  	pLeftPanelLyt->addLayout(pL2);
    pLeftPanelLyt->addLayout(pL3);
    pLeftPanelLyt->addStretch(1);
  }
  
  //ajoute le left panel au layout principale
  pLyt->addWidget(pLeftPanel, 1);
  pLyt->addWidget(mpViewer, 4);
  
  
  mpViewer->setControlType( Widget3d::ctRotateAround );
  updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::updateUi()
{
	if( mpCameras->count() == 0 )
  {
  	mpCameras->blockSignals( true );
  	for( uint i = 0; i < mCameras.size(); ++i )
    { mpCameras->insertItem( i, "camera " + QString::number( i ) ); }
    mpCameras->blockSignals( false );
  }
	if( mpCameraProjection->count() == 0 )
  {
  		mpCameraProjection->blockSignals( true );
      mpCameraProjection->insertItem( Camera::Projection::tOrthogonal, "Othogonal" );
      mpCameraProjection->insertItem( Camera::Projection::tPerspective, "Perspective" );
      mpCameraProjection->blockSignals( false );
  }
  if( mpCameraControl->count() == 0 )
  {
		  mpCameraControl->blockSignals( true );
      mpCameraControl->insertItem( Widget3d::ctNone, "Pas de controle" );
      mpCameraControl->insertItem( Widget3d::ctPan, "Pan" );
      mpCameraControl->insertItem( Widget3d::ctRotateAround, "RotateAround" );
      mpCameraControl->insertItem( Widget3d::ctFree, "ctFree" );
      mpCameraControl->blockSignals( false );
  }

	mpCameraProjection->setCurrentIndex( mpViewer->getCamera().getProjection().mType );
	mpCameraControl->setCurrentIndex( mpViewer->getControlType() );
  mpZoom->setText( QString::number( mpViewer->getCamera().zoom(), 'g', 2 ) );

	mpViewer->update();
}
//------------------------------------------------------------------------------
void MainDialog::zoomChanged()
{
	double z = mpZoom->text().toDouble( 0 );
  Camera c = mpViewer->getCamera();
  c.setZoom( z );
  mpViewer->setCamera(c);
  updateUi();
}
//------------------------------------------------------------------------------
//--- Viewer
//------------------------------------------------------------------------------
Viewer::Viewer( QWidget* ipW ) :
	Widget3d( ipW )
{
	setMouseTracking( true );
  
  srand( time(NULL) );
  for( int i = 0; i < 200; ++i )
  {
  	mObjectPositions.push_back( (Point3d( (rand() / (double)RAND_MAX - 0.5) * 400 ,
      (rand() / (double)RAND_MAX - 0.5) * 400,
      (rand() / (double)RAND_MAX - 0.5) * 400 ) ) );
  }
}

Viewer::~Viewer()
{}
//------------------------------------------------------------------------------
void Viewer::draw() const
{	
	for( uint i = 0; i < mObjectPositions.size(); ++i )
  {
  	glPushMatrix();
    const Point3d& p = mObjectPositions[i];
    glTranslated(p.x(), p.y(), p.z());
  	drawRectangularPrism( Point3d(-10), Point3d(10) );
    glPopMatrix();
  }  
}

//------------------------------------------------------------------------------
void Viewer::initializeGL()
{ Widget3d::initializeGL(); }

//-----------------------------------------------------------------------------
void Viewer::keyPressEvent(QKeyEvent* ipE)
{
  Widget3d::keyPressEvent( ipE );
	switch ( ipE->key() ) 
  {
    case Qt::Key_Space :
    {
    	Camera c = getCamera();
      c.set( Point3d( 0.0, 0.0, 100.0 ), 
      	Point3d(0.0),
        Vector3d( 0.0, 1.0, 0.0 ) );
      setCamera( c );
    } break;
    default: break;
  }
}

//------------------------------------------------------------------------------
void Viewer::paintGL()
{ Widget3d::paintGL(); }

//------------------------------------------------------------------------------
void Viewer::timerEvent(QTimerEvent* ipEvent)
{ 
	Widget3d::timerEvent( ipEvent );
//	ipEvent->ignore();
	update();
}
