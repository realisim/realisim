/*
 *  ObjectNavigator.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 25/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef RealEdit_ObjectNavigator_hh
#define RealEdit_ObjectNavigator_hh

#include <QTreeWidget>
#include <map>

namespace RealEdit
{
  class ObjectNavigator;
  class RealEditController;
  class ObjectNode;
}

class RealEdit::ObjectNavigator : public QTreeWidget
{
  Q_OBJECT
public:
  ObjectNavigator( QWidget* ipParent, RealEditController& iC );
  ~ObjectNavigator();
  
private slots:
  void doItemChanged(QTreeWidgetItem* ipItem, QTreeWidgetItem* ipPreviousItem);
  
private:
  template<class TreeItem>
  void createTree( TreeItem ipItem, ObjectNode* ipNode );
  
  RealEditController& mController;
  
  typedef std::map<QTreeWidgetItem*, ObjectNode*> TreeItemToNode;
  TreeItemToNode mTreeItemToNode;
};

#endif RealEdit_ObjectNavigator_hh