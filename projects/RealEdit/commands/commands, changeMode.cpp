/*
 *  commands, changeMode.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 09-09-06.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "commands, changeMode.h"
#include "EditionData.h"
#include "EditionUi.h"

using namespace realisim;
using namespace realEdit;
  using namespace commands;
  
ChangeMode::ChangeMode(RealEditController& iC, EditionUi& iUi,
  RealEditController::mode iMode) :
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
  mUi.modeChanged();
}

//------------------------------------------------------------------------------
void ChangeMode::undo()
{
  mController.mMode = mPreviousMode;
  mUi.modeChanged();
}
