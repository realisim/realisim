/*
 *  commands, translate.cpp
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */

#include "commands/changeNode.h"
#include "commands/removeNode.h"
#include "Controller.h"
#include "DataModel/ObjectNode.h"
#include "UserInterface/MainWindow.h"

using namespace realEdit;
  using namespace commands;
using namespace std;
  
RemoveNode::RemoveNode(Controller& iC, unsigned int iId) :
  Command(),
  mController(iC),
  mNode(*(iC.getEditionData().getNode(iId))),
  mParentId(iC.getEditionData().getNode(iId)->getParentNode()->getId())
{
}

RemoveNode::~RemoveNode()
{}

//------------------------------------------------------------------------------
void RemoveNode::execute()
{
  EditionData& e = mController.getEditionData();
  ObjectNode* n = e.getNode(mNode.getId());
  ObjectNode* parent = n->getParentNode();
  
  //juste avant d'enlever le noeud, nous allons selectionner le parent
  //de ce noeud afin de ne pas se retrouver sans selection
  ChangeNode c(mController, mParentId);
  c.execute();
  
  parent->removeChild(n);
  mController.getMainWindow().removeNode(mNode.getId());	
}

//------------------------------------------------------------------------------
unsigned int RemoveNode::getNodeId() const
{ return mNode.getId(); }

//------------------------------------------------------------------------------
/*Ici, on instancie une nouvelle copie du noeud mNode et on l'ajoute à l'arbre.
  Il est important de faire new, parce que si on ajoutait directement mNode dans
  l'arbre et que la pile de commande devait être détruite, le modele associé aux
  noeuds de mNode seraient probablement aussi détruit. Donc c'est pour garantir
  que la nouvelle copie qui sera inséré dans l'arbre est bien propriétaire
  des modèles.*/
void RemoveNode::undo()
{
  EditionData& e = mController.getEditionData();
  ObjectNode* parent = e.getNode(mParentId);
  ObjectNode* n = new ObjectNode(mNode);
  parent->addNode(n);
  mController.getMainWindow().addNode(mNode.getId());
  
  //on reselectionne le noeud juste apres son l'ajout
  ChangeNode c(mController, n->getId());
  c.execute();
}

