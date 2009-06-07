
#include "EditionUi.h"

#include <iostream>
#include <QApplication>
#include <QTextCodec>
#include <QMainWindow>
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

  EditionUi* p = new EditionUi();
  //pas besoin de deleter p, Qt le fera lorsque la fenêtre sera fermée
  p->setAttribute(Qt::WA_DeleteOnClose, true);
  
  if ( startMainApp() == 0 )
  {
      //On ferme, mais ya rien a faire pour l'instant!
  }

  return 0;
}
