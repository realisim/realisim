
#include "UserInterface/MainWindow.h"

#include <iostream>
#include <QApplication>
#include <QTextCodec>
#include <QIcon>


using namespace realEdit;

int startMainApp()
{
  if ( qApp )
  {
    return qApp->exec();
  }
  else
  {
    printf("ERROR: unable to start realEdit\n");
    return 0;
  }
}


int main(int argc, char** argv)
{

  QApplication app(argc, argv);
  Q_INIT_RESOURCE(images);
  app.setWindowIcon( QIcon( ":/images/RealEdit_icon.png" ) );
  QTextCodec::setCodecForCStrings (QTextCodec::codecForName("UTF-8"));

  MainWindow m;
  m.show();
  
  if ( startMainApp() == 0 )
  {
      //On ferme, mais ya rien a faire pour l'instant!
  }

  return 0;
}
