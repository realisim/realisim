/*
 *  commands, changeNode.cpp
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */

#include "commands, changeNode.h"
#include "EditionData.h"
#include "EditionUi.h"
#include "ObjectNode.h"

using namespace realisim;
using namespace realEdit;
  using namespace commands;
  
ChangeNode::ChangeNode(EditionData& iEd, EditionUi& iUi, const ObjectNode* ipNode) :
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
  mUi.currentNodeChanged();
}

//------------------------------------------------------------------------------
void ChangeNode::undo()
{
  mEditionData.setCurrentNode(mpPreviousNode);
  mUi.currentNodeChanged();
}

