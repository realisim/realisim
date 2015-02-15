
#include <iostream>
#include "MainDialog.h"
#include <QApplication>
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
//    QTextCodec::setCodecForCStrings (QTextCodec::codecForName("UTF-8"));
    MainDialog m;
    m.show();
	
    if ( startMainApp() == 0 )
    {
        //we are closing!
    }
	
    return 0;
}
