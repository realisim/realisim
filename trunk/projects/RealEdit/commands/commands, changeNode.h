/*
 *  commands, changeNode.h
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */
 
 #ifndef realedit_commands_changeNode_hh
 #define realedit_commands_changeNode_hh

#include "Command.h"
#include "ObjectNode.h"
namespace realEdit{class EditionData;}
namespace realEdit{class EditionUi;}

namespace realEdit
{
namespace commands 
{
  using namespace std;
  class ChangeNode : public realisim::utils::Command
  {
   public:
     explicit ChangeNode(EditionData&, EditionUi&, const ObjectNode*);
     virtual ~ChangeNode();
     
     virtual void execute();
     virtual void undo();
     
  private:    
    EditionData& mEditionData;
    EditionUi& mUi;
    const ObjectNode* mpPreviousNode;
    const ObjectNode* mpNode;
  };
}
}

#endif