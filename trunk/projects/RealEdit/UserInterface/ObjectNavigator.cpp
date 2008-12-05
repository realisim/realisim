/*
 *  ObjectNavigator.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 25/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "ObjectNavigator.h"

#include "EditionData.h"
#include "ObjectNode.h"
#include "DataModel.h"

#include <QHeaderView>
#include <QTreeWidgetItem>

using namespace RealEdit;

ObjectNavigator::ObjectNavigator( QWidget* ipParent, const EditionData& iEditionData ) :
QTreeWidget( ipParent ),
mEditionData( iEditionData )
{
  header()->hide();
  
  //create the object tree
  createTree( this, mEditionData.getScene().getObjectNode() );
}

ObjectNavigator::~ObjectNavigator()
{
}

template<class TreeItem>
void
ObjectNavigator::createTree( TreeItem ipItem,
                             const ObjectNode* ipNode )
{
  QTreeWidgetItem* pItem = new QTreeWidgetItem( ipItem );
  pItem->setText( 0, ipNode->getName().c_str() );
  
  for( unsigned int i = 0; i < ipNode->getChildCount(); ++i )
  {
    createTree( pItem, ipNode->getChild( i ) );
  }
}



