/*
 *  changeTool.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 09-09-07.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "commands, changeTool.h"
#include "EditionData.h"
#include "EditionUi.h"

using namespace realisim;
using namespace realEdit;
  using namespace commands;
  
ChangeTool::ChangeTool(Controller& iC, EditionUi& iUi,
  Controller::tool iTool) :
  Command(),
  mController(iC),
  mUi(iUi),
  mPreviousTool(iC.getTool()),
  mTool(iTool)
{}

ChangeTool::~ChangeTool()
{}

//------------------------------------------------------------------------------
void ChangeTool::execute()
{
  mController.mTool = mTool;
  mUi.changeTool();
}

//------------------------------------------------------------------------------
void ChangeTool::undo()
{
  mController.mTool = mPreviousTool;
  mUi.changeTool();
}

