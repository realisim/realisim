/*
 *  main.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 31/10/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ProjectSeletionUi_hh
#define ProjectSeletionUi_hh

#include "MainWindow.h"

#include <map>

class QWidget;
class QListWidgetItem;

class ProjectSelectionUi : public MainWindow
{
	Q_OBJECT
public:
	ProjectSelectionUi( UiController& iUiController );
	~ProjectSelectionUi();
	
	enum ProjectType
	{
		EDITION = 0,
		ANIMATION
	};

	
protected:
	
private slots:
	void projectSelected( QListWidgetItem* ipSelectedItem );
	
private:
	void addMenuBar();
	
	typedef std::map< QListWidgetItem*, ProjectType > ItemToProjectType;
	ItemToProjectType mItemToProjectType;
};

#endif