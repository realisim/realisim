/*
 *  commands, translate.cpp
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */

#include "commands/addNode.h"
#include "Controller.h"
#include "DataModel/ObjectNode.h"
#include "UserInterface/MainWindow.h"

using namespace realEdit;
  using namespace commands;
using namespace std;
  
AddNode::AddNode(Controller& iC, unsigned int iParentId) :
  Command(),
  mController(iC),
  mNodeId(0),
  mParentId(iParentId)
{
}

AddNode::~AddNode()
{}

//------------------------------------------------------------------------------
void AddNode::execute()
{
	EditionData& e = mController.getEditionData();
  ObjectNode* parent = e.getNode(mParentId);
  const ObjectNode* newNode = parent->addNode("Node", mNodeId);
  mNodeId = newNode->getId();
  mController.getMainWindow().addNode(mNodeId);
}

//------------------------------------------------------------------------------
unsigned int AddNode::getNodeId() const
{ return mNodeId; }

//------------------------------------------------------------------------------
void AddNode::undo()
{
  EditionData& e = mController.getEditionData();
  ObjectNode* parent = e.getNode(mParentId);
  ObjectNode* n = e.getNode(mNodeId);
  parent->removeChild(n);
  mController.getMainWindow().removeNode(mNodeId);
}

