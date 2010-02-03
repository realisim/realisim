
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
		Q_INIT_RESOURCE(images);
    QApplication app(argc, argv);
    QMainWindow mw;
    Pong::GameWindow gw(&mw);
    mw.setCentralWidget(&gw);
    
    mw.move(10, 40);
    mw.resize(800, 600);
    mw.show();
    
    gw.showOptions();
	
    if ( startMainApp() == 0 )
    {
        //we are closing!
        //clean up the game and close server/connections
        //gw->terminateGame();
    }
    return 0;
}
