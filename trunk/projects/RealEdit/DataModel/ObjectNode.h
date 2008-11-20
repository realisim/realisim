//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef OBJECTNODE_H
#define OBJECTNODE_H

#include "DataModel.h"

#include <vector>

namespace RealEdit
{
  class ObjectNode;
  class RealEditModel;
}

class RealEdit::ObjectNode
{
public:
  
private:
  RealEditModel* mpModel;
  ObjectNode* mParentNode;
  std::vector<ObjectNode*> mChilds;
};

#endif  //OBJECTNODE_H
