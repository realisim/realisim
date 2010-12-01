/*
 *  commands, changeNode.cpp
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */

#include "Controller.h"
#include "commands/changeNode.h"
#include "DataModel/EditionData.h"
#include "DataModel/ObjectNode.h"
#include "UserInterface/MainWindow.h"
#include "UserInterface/ProjectWindow.h"

using namespace realisim;
using namespace realEdit;
  using namespace commands;
  
ChangeNode::ChangeNode(Controller& iC, unsigned int iId) :
  Command(),
  mController(iC),
  mPreviousNodeId(mController.getEditionData().getCurrentNode()->getId()),
  mNodeId(iId)
{}

ChangeNode::~ChangeNode()
{}

//------------------------------------------------------------------------------
void ChangeNode::execute()
{
  EditionData& e = mController.getEditionData();
  ObjectNode* n = e.getNode(mNodeId);
  e.setCurrentNode(n);
  mController.getProjectWindow().changeCurrentNode();
  mController.getMainWindow().updateUi();
}

//------------------------------------------------------------------------------
void ChangeNode::undo()
{
  EditionData& e = mController.getEditionData();
  ObjectNode* n = e.getNode(mPreviousNodeId);
  e.setCurrentNode(n);
  mController.getProjectWindow().changeCurrentNode();
  mController.getMainWindow().updateUi();
}

