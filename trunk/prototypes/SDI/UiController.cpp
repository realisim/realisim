/*
 *  main.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 31/10/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "UiController.h"
#include "EditionUi.h"
#include "AnimationUi.h"

#include <QAction>
#include <qmenubar.h>
#include <QMenu>
#include <QStatusBar>
#include <QFrame>
#include <QListWidget>
#include <QLayout>
#include <QKeySequence>
#include <QPoint>

using namespace RealEdit;

UiController::UiController( QWidget* ipParent /* = 0*/ )
: QMainWindow( ipParent )
, mItemToProjectType()
{
	show();
	
	addMenuBar();
	
    //add a tool bar
	
    //add a statusBar
    QStatusBar* statusBar = new QStatusBar( this );
    setStatusBar( statusBar );
	
	
	//Create the central widget
	{
		QFrame* pCentralWidget = new QFrame( this );

		//set the central widget of the main window
		setCentralWidget( pCentralWidget );    
				
		QVBoxLayout* pLyt = new QVBoxLayout( pCentralWidget );
		
		QListWidget* pListWidget = new QListWidget( pCentralWidget );
		pListWidget->setAlternatingRowColors(true);
		
		connect( pListWidget, SIGNAL( itemActivated ( QListWidgetItem* ) ),
				 this, SLOT( projectSelected( QListWidgetItem* ) ) );
		
		pLyt->addWidget( pListWidget );
		
		QListWidgetItem* pEditionItem = new QListWidgetItem( "Edition", pListWidget );
		mItemToProjectType.insert( std::pair<QListWidgetItem*, ProjectType>( pEditionItem, EDITION ) );
		
		QListWidgetItem* pAnimationItem = new QListWidgetItem( "Animation", pListWidget );
		mItemToProjectType.insert( std::pair<QListWidgetItem*, ProjectType>( pAnimationItem, ANIMATION ) );
	}
		
}


UiController::~UiController()
{
}

void
UiController::addMenuBar()
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
}

void
UiController::handleNewProject()
{
	UiController* pUiController = new UiController();
	
	pUiController->move( this->pos() + QPoint(20, 20) );
	
	pUiController->show();
}

void 
UiController::projectSelected( QListWidgetItem* ipSelectedItem )
{
	ItemToProjectType::iterator it = mItemToProjectType.find( ipSelectedItem );
	
	if( it != mItemToProjectType.end() )
	{
		switch (it->second) 
		{
			case EDITION:
				EditionUi* pEditionUi = new EditionUi();
				pEditionUi->show();
				break;
			case ANIMATION:
				AnimationUi* pAnimationUi = new AnimationUi();
				pAnimationUi->show();
				break;
			default:
				break;
		}
	}
}

