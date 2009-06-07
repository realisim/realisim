
#include "EditionUi.h"
#include "RealEdit3d.h"
#include "ObjectNavigator.h"

#include <QMenuBar>
#include <QDockWidget>
#include <QFrame>
#include <QLayout>
#include <QListWidget>

using namespace realisim;
using namespace realisim::treeD;
using namespace realEdit;

EditionUi::EditionUi() 
: QMainWindow()
, mController( *this )
, mpWidget3d_1( 0 )
, mpWidget3d_2( 0 )
, mpWidget3d_3( 0 )
, mpWidget3d_4( 0 )
, mpObjectNavigator( 0 )
{	
	resize(800, 600);
	QFrame* pMainFrame = new QFrame( this );
	setCentralWidget( pMainFrame );
	
	addMenuBar();
	
	QGridLayout* pGLyt = new QGridLayout( pMainFrame );
  pGLyt->setSpacing( 1 );
	
	mpWidget3d_1 = new RealEdit3d( this, mpWidget3d_1, mController);
  mpWidget3d_1->setCameraMode( Camera::ORTHOGONAL );
  mpWidget3d_1->setCameraOrientation( Camera::XY );
    
  mpWidget3d_2 = new RealEdit3d( this, mpWidget3d_1, mController);
  mpWidget3d_2->setCameraMode( Camera::ORTHOGONAL );
  mpWidget3d_2->setCameraOrientation( Camera::ZY );
  
	mpWidget3d_3 = new RealEdit3d (this, mpWidget3d_1, mController);
  mpWidget3d_3->setCameraMode( Camera::ORTHOGONAL );
  mpWidget3d_3->setCameraOrientation( Camera::XZ );

	mpWidget3d_4 = new RealEdit3d (this, mpWidget3d_1, mController);
  mpWidget3d_4->setCameraOrientation( Camera::FREE );

  pGLyt->addWidget(mpWidget3d_4, 0, 0, 1, 3);
  pGLyt->setRowStretch(0, 2);
  pGLyt->addWidget(mpWidget3d_1, 1, 0 );
  pGLyt->addWidget(mpWidget3d_2, 1, 1 );
  pGLyt->addWidget(mpWidget3d_3, 1, 2 ); 
	
	//add the Object Navigator
	addObjectNavigator();
	
	show();
	
}

EditionUi::~EditionUi()
{
}

//------------------------------------------------------------------------------
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
		mpObjectNavigator = new ObjectNavigator( pFrame, mController );
		
		QFontMetrics dockWidgetFontMetric( pDockWidget->font() );
		int a = dockWidgetFontMetric.width( pDockWidget->windowTitle() );
		
		mpObjectNavigator->setMinimumWidth( a );
	}
	vLyt->addWidget( mpObjectNavigator );
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
void
EditionUi::createEditMenu( QMenuBar* ipMenuBar )
{
	QMenu* pEditMenu = ipMenuBar->addMenu( QObject::tr( "&Edit" ) );
}

//------------------------------------------------------------------------------
void
EditionUi::createToolMenu( QMenuBar* ipMenuBar )
{
	QMenu* pToolsMenu = ipMenuBar->addMenu( QObject::tr( "&Tools" ) );
	
	QAction* pTools1 = pToolsMenu->addAction( "tool 1" );
	QAction* pTools2 = pToolsMenu->addAction( "tool 2" );
	QAction* pTools3 = pToolsMenu->addAction( "tool 3" );
}

//------------------------------------------------------------------------------
void EditionUi::currentNodeChanged()
{
  mpWidget3d_1->currentNodeChanged();
  mpWidget3d_2->currentNodeChanged();
  mpWidget3d_3->currentNodeChanged();
  mpWidget3d_4->currentNodeChanged();
}

//------------------------------------------------------------------------------
void
EditionUi::newProject()
{
	mController.newProject();
}
