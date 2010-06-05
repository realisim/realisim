/*
 *  ObjectNavigator.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 25/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "UserInterface/ObjectNavigator.h"

#include "Controller.h"
#include "DataModel/ObjectNode.h"
#include "DataModel/DataModel.h"

#include <QHeaderView>
#include <QTreeWidgetItem>

using namespace realEdit;
using namespace std;

ObjectNavigator::ObjectNavigator(QWidget* ipParent) :
  QTreeWidget( ipParent ),
  mpController(0),
  mTreeItemToNode(),
  mNodeToTreeItem()
{
  header()->hide();
  setAlternatingRowColors(true);

  connect( this, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*) ),
         this, SLOT(doItemChanged(QTreeWidgetItem*, QTreeWidgetItem*) ) );
}

ObjectNavigator::~ObjectNavigator()
{}

//------------------------------------------------------------------------------
template<class TreeItem>
void
ObjectNavigator::createTree (TreeItem ipItem,
                            const ObjectNode* ipNode)
{
  //create a new tree item
  QTreeWidgetItem* pItem = new QTreeWidgetItem( ipItem );
  //add the tree item and its corresponding ObjectNode to the map.
  mTreeItemToNode.insert( 
    make_pair<QTreeWidgetItem*, const ObjectNode*>(pItem, ipNode) );
  pItem->setText( 0, ipNode->getName() );
  
  mNodeToTreeItem.insert(
    make_pair<const ObjectNode*, QTreeWidgetItem*>(ipNode, pItem) );
  
  for( unsigned int i = 0; i < ipNode->getChildCount(); ++i )
  {
    createTree( pItem, ipNode->getChild( i ) );
  }
}

//------------------------------------------------------------------------------
void ObjectNavigator::doItemChanged(QTreeWidgetItem* ipItem,
  QTreeWidgetItem* ipPreviousItem)
{
  TreeItemToNode::const_iterator it = mTreeItemToNode.find( ipItem );
  if( it != mTreeItemToNode.end() )
  {
    mpController->setCurrentNode (it->second);
  }
}

//------------------------------------------------------------------------------
void ObjectNavigator::expandAllItems()
{
  TreeItemToNode::const_iterator it = mTreeItemToNode.begin();
  while(it != mTreeItemToNode.end())
  {
    expandItem(it->first);
    ++it;
  }
}

//------------------------------------------------------------------------------
void ObjectNavigator::setController(Controller& iController)
{
  clear();
  mNodeToTreeItem.clear();
  mTreeItemToNode.clear();
  mpController = &iController;
  const EditionData& e = mpController->getEditionData();
  createTree( this, e.getScene().getObjectNode() );
	updateUi();
}

//------------------------------------------------------------------------------
void ObjectNavigator::updateUi()
{
  NodeToTreeItem::iterator it = 
    mNodeToTreeItem.find(mpController->getEditionData().getCurrentNode());
  if(it != mNodeToTreeItem.end())
  {
    setCurrentItem(it->second);
    scrollToItem(it->second);
  }
}



