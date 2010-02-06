/*
 *  commands, changeMode.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 09-09-06.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "commands/changeMode.h"
#include "DataModel/EditionData.h"
#include "UserInterface/EditionUi.h"

using namespace realisim;
using namespace realEdit;
  using namespace commands;
  
ChangeMode::ChangeMode(Controller& iC, EditionUi& iUi,
  Controller::mode iMode) :
  Command(),
  mController(iC),
  mUi(iUi),
  mPreviousMode(iC.getMode()),
  mMode(iMode)
{}

ChangeMode::~ChangeMode()
{}

//------------------------------------------------------------------------------
void ChangeMode::execute()
{
  mController.mMode = mMode;
  mUi.changeMode();
}

//------------------------------------------------------------------------------
void ChangeMode::undo()
{
  mController.mMode = mPreviousMode;
  mUi.changeMode();
}
