
#include "UiController.h"

#include <iostream>
#include <QApplication>
#include <QMainWindow>
#include <QIcon>


int startMainApp()
{
    if ( qApp )
    {
        return qApp->exec();
    }
    else
    {
        printf("ERROR: unable to start RealEdit\n");
        return 0;
    }
}


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
		
	UiController uiController;
	
    if ( startMainApp() == 0 )
    {
        //we are closing!
    }
	
    return 0;
}
