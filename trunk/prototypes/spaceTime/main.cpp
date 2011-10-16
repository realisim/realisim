
#include <iostream>
#include "MainWindow.h"
#include <QApplication>
#include <QIcon>
#include <QTextCodec>


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
    
    QTextCodec::setCodecForCStrings (QTextCodec::codecForName("UTF-8"));
    
    /*On active l'anti aliasing pour les fenêtre GL*/
    QGLFormat fmt;
    fmt.setSampleBuffers(true);
    fmt.setSamples(8);
    QGLFormat::setDefaultFormat(fmt);
    
    MainWindow m;
    m.show();
	
    if ( startMainApp() == 0 )
    {
        //we are closing!
    }
	
    return 0;
}
