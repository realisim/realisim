//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef OBJECTNODE_H
#define OBJECTNODE_H

#include <cassert>
#include <vector>

namespace RealEdit
{
  class ObjectNode;
  class RealEditModel;
}

class RealEdit::ObjectNode
{
public:
  ObjectNode();
  ~ObjectNode();
  
  ObjectNode( const ObjectNode& iObjectNode ){ assert(0); }
  const ObjectNode& operator=( const ObjectNode& iObjectNode ){ assert(0); }
  
  const RealEditModel* getModel() const;
  RealEditModel* getModel();
  
  unsigned int getNumChild() const;
  const ObjectNode* getChild( int iChildNumber ) const;
  
private:
  RealEditModel* mpModel;
  ObjectNode* mpParentNode;
  std::vector<ObjectNode*> mChilds;
};

#endif  //OBJECTNODE_H
