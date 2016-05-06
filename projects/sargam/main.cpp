
#include <iostream>
#include "MainDialog.h"
#include <QApplication>

int startMainApp()
{
    if ( qApp )
    {
        return qApp->exec();
    }
    else
    {
        printf("ERROR: unable to start Sargam\n");
        return 0;
    }
}

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  MainDialog m;
  m.show();

  if ( startMainApp() == 0 )
  {
      //we are closing!
  }

  return 0;
}
