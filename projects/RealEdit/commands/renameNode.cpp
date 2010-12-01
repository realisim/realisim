/*
 *  commands, translate.cpp
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */

#include "commands/renameNode.h"
#include "DataModel/EditionData.h"
#include "UserInterface/MainWindow.h"

using namespace realEdit;
  using namespace commands;
using namespace std;
  
RenameNode::RenameNode(Controller& iC, unsigned int iId, QString iNewName) :
  Command(),
  mC(iC),
  mId(iId),
  mOldName(mC.getEditionData().getNode(mId)->getName()),
  mNewName(iNewName)
{
}

RenameNode::~RenameNode()
{}

//------------------------------------------------------------------------------
void RenameNode::execute()
{
  EditionData& e = mC.getEditionData();
  ObjectNode* n = e.getNode(mId);
  n->setName(mNewName);
  mC.getMainWindow().renameNode(mId);
}

//------------------------------------------------------------------------------
void RenameNode::undo()
{
  EditionData& e = mC.getEditionData();
  ObjectNode* n = e.getNode(mId);
  n->setName(mOldName);
  mC.getMainWindow().renameNode(mId);
}

