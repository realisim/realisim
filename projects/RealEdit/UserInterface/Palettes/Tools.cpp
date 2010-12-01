/*
 *  Vierwer.h
 *  Realisim
 *
 *  Created by Pierre-Olivier Beaudoin on 
 */

#include "Controller.h"
#include "Palettes/Tools.h"

using namespace realEdit;
  using namespace palette;
  
Tools::Tools(QWidget* ipParent /* = 0*/) : QWidget(ipParent),
  mpController(0)
{
  setWindowFlags(windowFlags() | Qt::Tool);
}

Tools::~Tools()
{}

//------------------------------------------------------------------------------
void Tools::setController(Controller& iC)
{ mpController = &iC; }