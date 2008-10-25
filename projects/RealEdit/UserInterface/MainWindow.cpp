
#include "MainWindow.h"
#include "EditionUi.h"

#include <QMainWindow>
#include <QMenuBar>
#include <QFrame>
#include <QTabWidget>
#include <QStatusBar>
#include <QVBoxLayout>

using namespace RealEdit;

MainWindow::MainWindow()
: mpMainWindow( 0 )
, mpEditionUi( 0 )
{
	mpMainWindow = new QMainWindow();
	mpMainWindow->show();

	mpMainWindow->setMinimumSize( 800, 600 );

    //Create the central widget
    QTabWidget* pCentralWidget = new QTabWidget( mpMainWindow );
    pCentralWidget->setTabPosition( QTabWidget::South );
    //set the central widget of the main window
    mpMainWindow->setCentralWidget( pCentralWidget );    

	QVBoxLayout* pMainVLyt = new QVBoxLayout( pCentralWidget );

	//Add the edition page
	mpEditionUi = new EditionUi( pCentralWidget );
	pCentralWidget->addTab( mpEditionUi, QObject::tr( "Edition" ) );


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
}

MainWindow::~MainWindow()
{
}

