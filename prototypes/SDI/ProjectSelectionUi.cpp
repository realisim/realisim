/*
 *  main.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 31/10/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "ProjectSelectionUi.h"
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

ProjectSelectionUi::ProjectSelectionUi( UiController& iUiController )
: MainWindow( iUiController )
, mItemToProjectType()
{	
	addMenuBar();
	
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


ProjectSelectionUi::~ProjectSelectionUi()
{
}

void
ProjectSelectionUi::addMenuBar()
{
}


void 
ProjectSelectionUi::projectSelected( QListWidgetItem* ipSelectedItem )
{
	ItemToProjectType::iterator it = mItemToProjectType.find( ipSelectedItem );
	
	if( it != mItemToProjectType.end() )
	{
		switch (it->second) 
		{
			case EDITION:
				EditionUi* pEditionUi = new EditionUi( mUiController );
				pEditionUi->show();
				close();
				break;
			case ANIMATION:
				AnimationUi* pAnimationUi = new AnimationUi( mUiController );
				pAnimationUi->show();
				close();
				break;
			default:
				break;
		}
	}
}

