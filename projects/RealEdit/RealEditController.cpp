
#include "RealEditController.h"

#include "AnimationUi.h"
#include "EditionUi.h"

#include <QDockWidget>
#include <QFrame>
#include <QMainWindow>
#include <QMenuBar>
#include <QTabWidget>
#include <QStatusBar>
#include <QVBoxLayout>

using namespace RealEdit;

RealEditController::RealEditController()
: QObject()
,mpMainWindow( 0 )
, mpEditionUi( 0 )
, mpAnimationUi( 0 )
{
	mpMainWindow = new QMainWindow();
	mpMainWindow->show();
	mpMainWindow->setAnimated( true );
	mpMainWindow->setMinimumSize( 800, 600 );

    //Create the central widget
	{
		QTabWidget* pCentralWidget = new QTabWidget( mpMainWindow );
		pCentralWidget->setTabPosition( QTabWidget::South );
		//set the central widget of the main window
		mpMainWindow->setCentralWidget( pCentralWidget );    

		QVBoxLayout* pMainVLyt = new QVBoxLayout( pCentralWidget );

		QObject::connect( pCentralWidget, SIGNAL( currentChanged ( int ) ),
						  this, SLOT( handleCurrentTabChanged( int ) ) );

		//Add the edition page
		addEditionUi( pCentralWidget );

		//Add the animation page
		addAnimationUi( pCentralWidget );
	}

    //create a menu bar
    QMenuBar* pMenuBar = new QMenuBar();

    //add item to the menu bar and to the catalog
	pMenuBar->addMenu( QObject::tr( "&File" ) );
    pMenuBar->addMenu( QObject::tr( "&Edit" ) );
    pMenuBar->addMenu( QObject::tr( "&Tools" ) );

    //add a menu to the mainWindow
    mpMainWindow->setMenuBar( pMenuBar );

    //add a tool bar

    //add a statusBar
    QStatusBar* statusBar = new QStatusBar( mpMainWindow );
    mpMainWindow->setStatusBar( statusBar );

	hideAllWorkspaceUiWidgets();
	showWorkspaceUiWidgets( mpEditionUi );
}

RealEditController::~RealEditController()
{
}

void
RealEditController::addEditionUi( QTabWidget* ipCentralWidget )
{
	//Add the edition page
	mpEditionUi = new EditionUi( ipCentralWidget, *this );
	ipCentralWidget->insertTab( EditionTabIndex, mpEditionUi, QObject::tr( "Edition" ) );
}

void
RealEditController::addAnimationUi( QTabWidget* ipCentralWidget )
{
	//Add the animation page
	mpAnimationUi = new AnimationUi( ipCentralWidget, *this );
	ipCentralWidget->insertTab( AnimationTabIndex, mpAnimationUi, QObject::tr( "Animation" ) );
}

QDockWidget*
RealEditController::addDockWidget( WorkspaceUi* ipRequester )
{
	QDockWidget* pDockWidget = new QDockWidget( mpMainWindow );
	mpMainWindow->addDockWidget( Qt::LeftDockWidgetArea, pDockWidget );
	
	mWorkspaceUiToWidgetMap.insert( std::make_pair( ipRequester, pDockWidget ) );

	return pDockWidget;
}

void
RealEditController::hideAllWorkspaceUiWidgets()
{
	std::map<WorkspaceUi*, QWidget*>::iterator it = mWorkspaceUiToWidgetMap.begin();
	while( it != mWorkspaceUiToWidgetMap.end() )
	{
		it->second->hide();
		it++;
	}
}

void
RealEditController::showWorkspaceUiWidgets( WorkspaceUi* ipWorkspace )
{
	std::pair<WorkspaceUiToWidgetMap::iterator, WorkspaceUiToWidgetMap::iterator> range =
		mWorkspaceUiToWidgetMap.equal_range( ipWorkspace );

	while( range.first != range.second )
	{
		range.first->second->show();
		range.first++;
	}
}

void
RealEditController::hideWorkspaceUiWidgets( WorkspaceUi* ipWorkspace )
{
		std::pair<WorkspaceUiToWidgetMap::iterator, WorkspaceUiToWidgetMap::iterator> range =
		mWorkspaceUiToWidgetMap.equal_range( ipWorkspace );

	while( range.first != range.second )
	{
		range.first->second->hide();
		range.first++;
	}
}

void
RealEditController::handleCurrentTabChanged( int iIndex )
{
	switch( iIndex )
	{
	case EditionTabIndex:
		hideAllWorkspaceUiWidgets();
		showWorkspaceUiWidgets( mpEditionUi );
		break;
	case AnimationTabIndex:
		hideAllWorkspaceUiWidgets();
		showWorkspaceUiWidgets( mpAnimationUi );
		break;
	}
}