/*
 *  MainWindow.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 08/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "MainWindow.h"
#include "UiController.h"
#include "ProjectSelectionUi.h"

#include <QMenuBar>

MainWindow::MainWindow( UiController& iUiController ) 
: QMainWindow()
, mUiController( iUiController )
{
	show();
	addMenuBar();
}

MainWindow::~MainWindow()
{
}

void MainWindow::addMenuBar()
{
	//create a menu bar
	mpMenuBar = new QMenuBar();
	
	//add item to the menu bar and to the catalog
	QMenu* pFileMenu = mpMenuBar->addMenu( QObject::tr( "&File" ) );
	/* QMenu* pEditMenu = pMenuBar->addMenu( QObject::tr( "&Edit" ) );
	QMenu* pToolsMenu = pMenuBar->addMenu( QObject::tr( "&Tools" ) ); */
	
    //add a menu to the mainWindow
    setMenuBar( mpMenuBar );
	
	//add New Project menu item
	QAction* pNewProject = pFileMenu->addAction( "New Project", this, SLOT( newProject() ) );
	pNewProject->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_N ) );
	
	//add Open Project menu item
	QAction* pOpenProject = pFileMenu->addAction( "Open Project", this, SLOT( openProject() ) );
	pOpenProject->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_O ) );
}

void
MainWindow::newProject()
{
	mUiController.newProject();
}

void
MainWindow::openProject()
{
	mUiController.openProject();
}