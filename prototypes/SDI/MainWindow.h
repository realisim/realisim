/*
 *  MainWindow.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 08/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MainWindow_hh
#define MainWindow_hh

#include <QMainWindow>

class UiController;
class QMenuBar;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow( UiController& iUiController );
	~MainWindow();
				
protected:
	UiController& mUiController;
	QMenuBar* mpMenuBar;
	
private slots:
	void newProject();
	void openProject();
	
private:
	void addMenuBar();
};

#endif
