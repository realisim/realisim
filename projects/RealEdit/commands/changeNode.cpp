/*
 *  commands, changeNode.cpp
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */

#include "commands/changeNode.h"
#include "DataModel/EditionData.h"
#include "DataModel/ObjectNode.h"
#include "UserInterface/MainWindow.h"
#include "UserInterface/ProjectWindow.h"

using namespace realisim;
using namespace realEdit;
  using namespace commands;
  
ChangeNode::ChangeNode(ProjectWindow& iPw, EditionData& iEd, const ObjectNode* ipNode) :
  Command(),
  mProjectWindow(iPw),
  mEditionData(iEd),
  mpPreviousNode(mEditionData.getCurrentNode()),
  mpNode(ipNode)
{}

ChangeNode::~ChangeNode()
{}

//------------------------------------------------------------------------------
void ChangeNode::execute()
{
  mEditionData.setCurrentNode(mpNode);
  mProjectWindow.changeCurrentNode();
  ((MainWindow*)mProjectWindow.parentWidget())->updateUi();
}

//------------------------------------------------------------------------------
void ChangeNode::undo()
{
  mEditionData.setCurrentNode(mpPreviousNode);
  mProjectWindow.changeCurrentNode();
  ((MainWindow*)mProjectWindow.parentWidget())->updateUi();
}

