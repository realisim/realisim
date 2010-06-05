/*
 *  changeTool.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 09-09-07.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

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
     virtual void undo();
     
  private:    
    Controller& mController;
    Controller::tool mPreviousTool;
    Controller::tool mTool;
  };
}
}

#endif