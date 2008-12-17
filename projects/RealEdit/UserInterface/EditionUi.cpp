
#include "EditionUi.h"
#include "RealEdit3d.h"
#include "ObjectNavigator.h"

#include <QMenuBar>
#include <QDockWidget>
#include <QFrame>
#include <QLayout>
#include <QListWidget>

using namespace Realisim;
using namespace RealEdit;

EditionUi::EditionUi() 
: QMainWindow()
, mController()
, mpWidget3d_1( 0 )
, mpWidget3d_2( 0 )
, mpWidget3d_3( 0 )
, mpObjectNavigator( 0 )
{	
	resize(800, 600);
	QFrame* pMainFrame = new QFrame( this );
	setCentralWidget( pMainFrame );
	
	addMenuBar();
	
	QVBoxLayout* pVLyt = new QVBoxLayout( pMainFrame );
  pVLyt->setSpacing( 2 );
	
	mpWidget3d_1 = new RealEdit3d( this, mpWidget3d_1, mController.getEditionData() );
  mpWidget3d_1->setCameraMode( Camera::ORTHOGONAL );
  Camera cam = mpWidget3d_1->getCamera();
  cam.set( Point3d( 0, 0, 10 ),
           Point3d( 0, 0, 0 ),
           Vector3d( 0, 1, 0 ) );
  mpWidget3d_1->setCamera( cam );
  
  mpWidget3d_2 = new RealEdit3d( this, mpWidget3d_1, mController.getEditionData() );
  mpWidget3d_2->setCameraMode( Camera::ORTHOGONAL );
  Camera cam2 = mpWidget3d_2->getCamera();
  cam2.set( Point3d( 0, 10, 0 ),
           Point3d( 0, 0, 0 ),
           Vector3d( 1, 0, 0 ) );
  mpWidget3d_2->setCamera( cam2 );
  
	mpWidget3d_3 = new RealEdit3d( this, mpWidget3d_1, mController.getEditionData() );
  Camera cam3 = mpWidget3d_3->getCamera();
  cam3.set( Point3d( 10, 0, 0 ),
            Point3d( 0, 0, 0 ),
            Vector3d( 0, 1, 0 ) );
  mpWidget3d_3->setCamera( cam3 );
	
	pVLyt->addWidget( mpWidget3d_1 );
	pVLyt->addWidget( mpWidget3d_2 );
	pVLyt->addWidget( mpWidget3d_3 );
	
	//add the Object Navigator
	addObjectNavigator();
	
	show();
	
}

EditionUi::~EditionUi()
{
}

void
EditionUi::addObjectNavigator()
{
	//add the dock widget via the controller, because the 
	//controller manage the show/hide of the dock widget
	//when a different workspace is shown by the central tab widget
	QDockWidget* pDockWidget = new QDockWidget( "Object Navigation", this );
	addDockWidget( Qt::LeftDockWidgetArea, pDockWidget );
	pDockWidget->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
	
	//add the QlistWidget
	QFrame* pFrame = new QFrame( pDockWidget );
	pDockWidget->setWidget( pFrame );
	QVBoxLayout* vLyt = new QVBoxLayout( pFrame );
	{
		mpObjectNavigator = new ObjectNavigator( pFrame, mController.getEditionData() );
		
		QFontMetrics dockWidgetFontMetric( pDockWidget->font() );
		int a = dockWidgetFontMetric.width( pDockWidget->windowTitle() );
		
		mpObjectNavigator->setMinimumWidth( a );
	}
	vLyt->addWidget( mpObjectNavigator );
}

void
EditionUi::addMenuBar()
{
	//create a menu bar
	QMenuBar* pMenuBar = new QMenuBar();
	//add a menu to the mainWindow
  setMenuBar( pMenuBar );
	
	createFileMenu( pMenuBar );
	createEditMenu( pMenuBar );
	createToolMenu( pMenuBar );
	
}

void
EditionUi::createFileMenu( QMenuBar* ipMenuBar )
{
	//add item to the menu bar and to the catalog
	QMenu* pFileMenu = ipMenuBar->addMenu( QObject::tr( "&File" ) );
	
	//add New Project menu item
	QAction* pNewProject = pFileMenu->addAction( "New Project", this, SLOT( newProject() ) );
	pNewProject->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_N ) );
	
	//add Open Project menu item
//	QAction* pOpenProject = pFileMenu->addAction( "Open Project", this, SLOT( openProject() ) );
//	pOpenProject->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_O ) );
}

void
EditionUi::createEditMenu( QMenuBar* ipMenuBar )
{
	QMenu* pEditMenu = ipMenuBar->addMenu( QObject::tr( "&Edit" ) );
}

void
EditionUi::createToolMenu( QMenuBar* ipMenuBar )
{
	QMenu* pToolsMenu = ipMenuBar->addMenu( QObject::tr( "&Tools" ) );
	
	QAction* pTools1 = pToolsMenu->addAction( "tool 1" );
	QAction* pTools2 = pToolsMenu->addAction( "tool 2" );
	QAction* pTools3 = pToolsMenu->addAction( "tool 3" );
}

void
EditionUi::newProject()
{
	mController.newProject();
}
