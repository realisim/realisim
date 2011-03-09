/*
 *  commands, changeNode.h
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */
 
 #ifndef realedit_commands_changeNode_hh
 #define realedit_commands_changeNode_hh

#include "utils/Command.h"
#include "DataModel/ObjectNode.h"
namespace realEdit{class EditionData;}
namespace realEdit{class Controller;}

namespace realEdit
{
namespace commands 
{
  using namespace std;
  class ChangeNode : public realisim::utils::Command
  {
    public:
      explicit ChangeNode(Controller&, unsigned int);
      virtual ~ChangeNode();

      virtual void execute();
      virtual void undo();
    
    private:
      Controller& mController;
      unsigned int mPreviousNodeId;
      unsigned int mNodeId;
  };
}
}

#endif