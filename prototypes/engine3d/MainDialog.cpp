/*
 */

#include "MainDialog.h"
#include <QLayout>

//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QMainWindow()
{
  //--- create Ui
  resize(800, 600);
  
  setCentralWidget(new QFrame(this));
}