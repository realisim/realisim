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

namespace RealEdit
{
  class ObjectNavigator;
  class EditionData;
  class ObjectNode;
}

class RealEdit::ObjectNavigator : public QTreeWidget
{
public:
  ObjectNavigator( QWidget* ipParent, const EditionData& iEditionData );
  ~ObjectNavigator();
  
private:
  template<class TreeItem>
  void createTree( TreeItem ipItem, const ObjectNode* ipNode );
  
  const EditionData& mEditionData;
};

#endif RealEdit_ObjectNavigator_hh