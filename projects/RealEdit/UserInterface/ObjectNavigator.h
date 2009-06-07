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
  class RealEditController;
  class ObjectNode;
}

class realEdit::ObjectNavigator : public QTreeWidget
{
  Q_OBJECT
public:
  ObjectNavigator( QWidget* ipParent, RealEditController& iC );
  ~ObjectNavigator();
  
private slots:
  void doItemChanged(QTreeWidgetItem* ipItem, QTreeWidgetItem* ipPreviousItem);
  
private:
  template<class TreeItem>
  void createTree( TreeItem ipItem, const ObjectNode* ipNode );
  void expandAllItems();
  
  RealEditController& mController;
  const EditionData& mEditionData;
  
  typedef std::map<QTreeWidgetItem*, const ObjectNode*> TreeItemToNode;
  TreeItemToNode mTreeItemToNode;
};

#endif RealEdit_ObjectNavigator_hh