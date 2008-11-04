
#include "EditionUi.h"
#include "Widget3d.h"

#include <QMenuBar>
#include <QDockWidget>
#include <QFrame>
#include <QLayout>
#include <QListWidget>

using namespace Realisim;
using namespace RealEdit;

EditionUi::EditionUi()
: QMainWindow()
, mpWidget3d_1( 0 )
, mpWidget3d_2( 0 )
, mpObjectNavigator( 0 )
{
	QFrame* pMainFrame = new QFrame( this );
	setCentralWidget( pMainFrame );
	
	addMenuBar();
	
	QVBoxLayout* pVLyt = new QVBoxLayout( pMainFrame );	

	mpWidget3d_1 = new Widget3d( this, mpWidget3d_1 );
	mpWidget3d_2 = new Widget3d( this, mpWidget3d_2 );

	pVLyt->addWidget( mpWidget3d_1 );
	pVLyt->addWidget( mpWidget3d_2 );

	//add the Object Navigator
	addObjectNavigator();

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
		mpObjectNavigator = new QListWidget( pFrame );

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
	
	//add item to the menu bar and to the catalog
	QMenu* pFileMenu = pMenuBar->addMenu( QObject::tr( "&File" ) );
    QMenu* pEditMenu = pMenuBar->addMenu( QObject::tr( "&Edit" ) );
    QMenu* pToolsMenu = pMenuBar->addMenu( QObject::tr( "&Tools" ) );
	
    //add a menu to the mainWindow
    setMenuBar( pMenuBar );
	
	//add New File menu item
	QAction* pNewProject = pFileMenu->addAction( "New Project", this, SLOT( handleNewProject() ) );
	pNewProject->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_N ) );
	
	QAction* pTools1 = pToolsMenu->addAction( "tool 1" );
	QAction* pTools2 = pToolsMenu->addAction( "tool 2" );
	QAction* pTools3 = pToolsMenu->addAction( "tool 3" );
	
}
