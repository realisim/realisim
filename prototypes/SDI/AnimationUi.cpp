
#include "AnimationUi.h"
#include "Widget3d.h"
#include "UiController.h"

#include <QMenuBar>
#include <QDockWidget>
#include <QLayout>
#include <QListWidget>

using namespace realisim;
using namespace treeD;
using namespace RealEdit;

AnimationUi::AnimationUi( UiController& iUiController)
: MainWindow(iUiController)
, mpWidget3d_1( 0 )
, mpWidget3d_2( 0 )
{
	QFrame* pMainFrame = new QFrame( this );
	setCentralWidget( pMainFrame );
	
	addMenuBar();
	
	QVBoxLayout* pVLyt = new QVBoxLayout( pMainFrame );	

	mpWidget3d_1 = new Widget3d( this, mpWidget3d_1 );
	mpWidget3d_2 = new Widget3d( this, mpWidget3d_2 );

	pVLyt->addWidget( mpWidget3d_1 );
	pVLyt->addWidget( mpWidget3d_2 );

	//add the dock widget that holds the animation navigator
	addAnimationNavigator();

}

AnimationUi::~AnimationUi()
{
}

void
AnimationUi::addAnimationNavigator()
{
	//add the dock widget via the controller, because the 
	//controller manage the show/hide of the dock widget
	//when a different workspace is shown by the central tab widget
	QDockWidget* pDockWidget = new QDockWidget( QObject::tr( "Animation Navigation" ), this );
	addDockWidget( Qt::LeftDockWidgetArea, pDockWidget );
	pDockWidget->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );

	//add the QlistWidget
	QFrame* pFrame = new QFrame( pDockWidget );
	pDockWidget->setWidget( pFrame );
	QVBoxLayout* vLyt = new QVBoxLayout( pFrame );
	{
		mpAnimationNavigator = new QListWidget( pFrame );

		QFontMetrics dockWidgetFontMetric( pDockWidget->font() );
		int a = dockWidgetFontMetric.width( pDockWidget->windowTitle() );

		mpAnimationNavigator->setMinimumWidth( a );
	}
	vLyt->addWidget( mpAnimationNavigator );
}

void
AnimationUi::addMenuBar()
{
	
	//add item to the menu bar and to the catalog
	//QMenu* pFileMenu = pMenuBar->addMenu( QObject::tr( "&File" ) );
    mpMenuBar->addMenu( QObject::tr( "&Edit" ) );
    QMenu* pToolsMenu = mpMenuBar->addMenu( QObject::tr( "&Tools" ) );
	
	QAction* pTools1 = pToolsMenu->addAction( "tool 3" );
	QAction* pTools2 = pToolsMenu->addAction( "tool 4" );
	QAction* pTools3 = pToolsMenu->addAction( "tool 5" );
	
}


