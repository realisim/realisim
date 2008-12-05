//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef OBJECTNODE_H
#define OBJECTNODE_H

#include <cassert>
#include <string>
#include <vector>

namespace RealEdit
{
  class ObjectNode;
  class RealEditModel;
}

class RealEdit::ObjectNode
{
public:
  ObjectNode( const std::string& iName );
  ~ObjectNode();
  
  ObjectNode( const ObjectNode& iObjectNode ){ assert(0); }
  const ObjectNode& operator=( const ObjectNode& iObjectNode ){ assert(0); }
  
  const RealEditModel* getModel() const;
  RealEditModel* getModel();
  
  unsigned int getChildCount() const;
  const ObjectNode* getChild( int iChildNumber ) const;
  const std::string& getName() const;
  
  ObjectNode* addNode( ObjectNode* ipParent, const std::string& iName );
  
private:
  RealEditModel* mpModel;
  ObjectNode* mpParentNode;
  std::vector<ObjectNode*> mChilds;
  
  std::string mName;
};

#endif  //OBJECTNODE_H
