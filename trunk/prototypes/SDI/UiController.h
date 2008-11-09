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

#include <QObject>

class UiController : public QObject
{
	Q_OBJECT
public:
	UiController();
	~UiController();
	
	void newProject();
	void openProject();
	
protected:
		
private:
	
};

#endif