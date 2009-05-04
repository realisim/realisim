
#include "EditionUi.h"

#include <iostream>
#include <QApplication>
#include <QMainWindow>
#include <QIcon>


using namespace RealEdit;

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
	Q_INIT_RESOURCE(images);

 	app.setWindowIcon( QIcon( ":/images/RealEdit_icon.png" ) );

	EditionUi* p = new EditionUi(); 
  if ( startMainApp() == 0 )
  {
      //we are closing!
      delete p;
  }

    return 0;
}
