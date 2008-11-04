/*
 *  main.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 31/10/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef UiController_hh
#define UiController_hh

#include <QMainWindow>

#include <map>

class QWidget;
class QListWidgetItem;

class UiController : public QMainWindow
{
	Q_OBJECT
public:
	UiController( QWidget* ipParent = 0 );
	~UiController();
	
	enum ProjectType
	{
		EDITION = 0,
		ANIMATION
	};
	
public slots:
	void handleNewProject();
	
protected:
	
private slots:
	void projectSelected( QListWidgetItem* ipSelectedItem );
	
private:
	void addMenuBar();
	
	typedef std::map< QListWidgetItem*, ProjectType > ItemToProjectType;
	ItemToProjectType mItemToProjectType;
};

#endif