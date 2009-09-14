/*
 *  ObjectNavigator.h
 */

#ifndef RealEdit_ObjectNavigator_hh
#define RealEdit_ObjectNavigator_hh

#include <QTreeWidget>
#include <map>

namespace realEdit
{
  class EditionData;
  class ObjectNavigator;
  class Controller;
  class ObjectNode;
}

class realEdit::ObjectNavigator : public QTreeWidget
{
  Q_OBJECT
public:
  ObjectNavigator( QWidget* ipParent, Controller& iC );
  ~ObjectNavigator();
  
  void changeCurrentNode();
  
private slots:
  void doItemChanged(QTreeWidgetItem* ipItem, QTreeWidgetItem* ipPreviousItem);
  
private:
  template<class TreeItem>
  void createTree( TreeItem ipItem, const ObjectNode* ipNode );
  void expandAllItems();
  
  Controller& mController;
  const EditionData& mEditionData;
  
  typedef std::map<QTreeWidgetItem*, const ObjectNode*> TreeItemToNode;
  typedef std::map<const ObjectNode*, QTreeWidgetItem*> NodeToTreeItem;
  TreeItemToNode mTreeItemToNode;
  NodeToTreeItem mNodeToTreeItem;
};

#endif RealEdit_ObjectNavigator_hh