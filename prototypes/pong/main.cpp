
#include <iostream>
#include "GameWindow.h"
#include <QApplication>
#include <QIcon>
#include <QMainWindow>


int startMainApp()
{
    if ( qApp )
    {
        return qApp->exec();
    }
    else
    {
        printf("ERROR: unable to start Pong\n");
        return 0;
    }
}


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QMainWindow* mw = new QMainWindow();
    Pong::GameWindow* gw = new Pong::GameWindow(mw);
    mw->setCentralWidget(gw);
    
    mw->show();
    mw->move(10, 40);
    mw->resize(800, 600);
    
    gw->showOptions();
	
    if ( startMainApp() == 0 )
    {
        //we are closing!
        //clean up the game and close server/connections
        //gw->terminateGame();
    }
	
    return 0;
}
