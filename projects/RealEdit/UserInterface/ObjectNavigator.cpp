/*
 *  ObjectNavigator.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 25/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "ObjectNavigator.h"

#include "RealEditController.h"
#include "ObjectNode.h"
#include "DataModel.h"

#include <QHeaderView>
#include <QTreeWidgetItem>

using namespace realEdit;
using namespace std;

ObjectNavigator::ObjectNavigator( QWidget* ipParent, RealEditController& iC ) :
QTreeWidget( ipParent ),
mController( iC ),
mEditionData (const_cast<const RealEditController&> (iC).getEditionData ()),
mTreeItemToNode()
{
  header()->hide();
  setAlternatingRowColors(true);
  setAnimated(true);
  
  //create the object tree
  createTree( this, mEditionData.getScene().getObjectNode() );
  expandAllItems();
  
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
    mController.setCurrentNode (it->second);
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



