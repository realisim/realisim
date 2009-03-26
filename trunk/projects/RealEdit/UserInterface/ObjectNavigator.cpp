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

using namespace RealEdit;
using namespace std;

ObjectNavigator::ObjectNavigator( QWidget* ipParent, RealEditController& iC ) :
QTreeWidget( ipParent ),
mController( iC ),
mTreeItemToNode()
{
  header()->hide();
  
  //create the object tree
  createTree( this, mController.getEditionData().getScene().getObjectNode() );
  
  connect( this, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*) ),
           this, SLOT(doItemChanged(QTreeWidgetItem*, QTreeWidgetItem*) ) );
}

ObjectNavigator::~ObjectNavigator()
{
}

//------------------------------------------------------------------------------
template<class TreeItem>
void
ObjectNavigator::createTree( TreeItem ipItem,
                             ObjectNode* ipNode )
{
  //create a new tree item
  QTreeWidgetItem* pItem = new QTreeWidgetItem( ipItem );
  //add the tree item and its corresponding ObjectNode to the map.
  mTreeItemToNode.insert( 
    make_pair<QTreeWidgetItem*, ObjectNode*>(pItem, ipNode) );
  pItem->setText( 0, ipNode->getName().c_str() );
  
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
    mController.setCurrentNode( it->second );
  }
}



