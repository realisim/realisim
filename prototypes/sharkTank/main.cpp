// (c) Copyright, Resonant Medical Inc., 2008


#include "mainWindow.h"

#include "qapplication.h"
#include "qmainwindow.h"
#include "qtextcodec.h"
#include <windows.h>

int main ( int argc, char* argv[] )
{
   if ( AllocConsole() )
   {
      freopen( "conin$", "r", stdin );
      freopen( "conout$", "w", stdout );
      freopen( "conout$", "w", stderr );
   }

   QApplication a( argc , argv );
   mainWindow w;
   a.setMainWidget(&w);

   QTextCodec::setCodecForCStrings( QTextCodec::codecForName( "utf8" ) );
   QTextCodec::setCodecForTr( QTextCodec::codecForName( "utf8" ) );

   w.show();
   a.exec();
   return 0 ;
}
