
#include <iostream>
#include <QApplication>
#include <QIcon>
#include <QMainWindow>
#include "Widget.h"


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
    QMainWindow mw;
    realisim::prototypes::Widget w(&mw);
    mw.setCentralWidget(&w);
    
    mw.move(10, 40);
    mw.resize(480, 320);
    mw.show();
    	
    if ( startMainApp() == 0 )
    {
        //we are closing!
    }
    return 0;
}
