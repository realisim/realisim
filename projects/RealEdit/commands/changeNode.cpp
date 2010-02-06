/*
 *  commands, changeNode.cpp
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */

#include "commands/changeNode.h"
#include "DataModel/EditionData.h"
#include "UserInterface/EditionUi.h"
#include "DataModel/ObjectNode.h"

using namespace realisim;
using namespace realEdit;
  using namespace commands;
  
ChangeNode::ChangeNode(EditionData& iEd, EditionUi& iUi, const ObjectNode* ipNode) :
  Command(),
  mEditionData(iEd),
  mUi(iUi),
  mpPreviousNode(mEditionData.getCurrentNode()),
  mpNode(ipNode)
{}

ChangeNode::~ChangeNode()
{}

//------------------------------------------------------------------------------
void ChangeNode::execute()
{
  mEditionData.setCurrentNode(mpNode);
  mUi.changeCurrentNode();
}

//------------------------------------------------------------------------------
void ChangeNode::undo()
{
  mEditionData.setCurrentNode(mpPreviousNode);
  mUi.changeCurrentNode();
}

