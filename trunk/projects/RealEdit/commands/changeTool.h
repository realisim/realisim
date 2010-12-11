#ifndef realedit_commands_ChangeTool_hh
#define realedit_commands_ChangeTool_hh

#include "utils/Command.h"
namespace realEdit{class EditionData;}
#include "Controller.h"

namespace realEdit
{
namespace commands 
{
  using namespace std;
  class ChangeTool : public realisim::utils::Command
  {
   public:
     explicit ChangeTool(Controller&,
       Controller::tool);
     virtual ~ChangeTool();
     
     virtual void execute();
     
  protected:
    virtual void undo();
        
    Controller& mController;
    Controller::tool mPreviousTool;
    Controller::tool mTool;
  };
}
}

#endif
