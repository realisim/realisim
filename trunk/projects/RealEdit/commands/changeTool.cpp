/*
 *  changeTool.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 09-09-07.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "commands/changeTool.h"
#include "DataModel/EditionData.h"

using namespace realisim;
using namespace realEdit;
  using namespace commands;
  
ChangeTool::ChangeTool(Controller& iC,
  Controller::tool iTool) :
  Command(),
  mController(iC),
  mPreviousTool(iC.getTool()),
  mTool(iTool)
{}

ChangeTool::~ChangeTool()
{}

//------------------------------------------------------------------------------
void ChangeTool::execute()
{
  mController.mTool = mTool;
}

//------------------------------------------------------------------------------
void ChangeTool::undo()
{
  mController.mTool = mPreviousTool;
}

