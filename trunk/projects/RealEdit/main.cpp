
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

  /*On active l'anti aliasing pour les fenêtre GL. Pour 
    l'instant, il est désactiver parce que ca entre en comflit avec
    le picking. Il suffit de glDisable(GL_MULTISAMPLE) dans le drawForPicking
    */
//  QGLFormat fmt;
//  fmt.setSampleBuffers(true);
//  fmt.setSamples(8);
//  QGLFormat::setDefaultFormat(fmt);

  MainWindow m;
  m.show();
  
  if ( startMainApp() == 0 )
  {
      //On ferme, mais ya rien a faire pour l'instant!
  }

  return 0;
}
