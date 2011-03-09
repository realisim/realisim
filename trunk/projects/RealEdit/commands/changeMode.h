/*
 *  commands, changeMode.h
 *  Project
 *  Created by Pierre-Olivier Beaudoin on 09-09-06.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 */
 
 #ifndef realedit_commands_ChangeMode_hh
 #define realedit_commands_ChangeMode_hh

#include "utils/Command.h"
namespace realEdit{class EditionData;}
#include "Controller.h"

namespace realEdit
{
namespace commands 
{
  using namespace std;
  class ChangeMode : public realisim::utils::Command
  {
  	public:
    	explicit ChangeMode(Controller&, Controller::mode);
     	virtual ~ChangeMode();
     
     	virtual void execute();
      virtual void undo();
      
    private:
      Controller& mController;
      Controller::mode mPreviousMode;
      Controller::mode mMode;
  };
}
}

#endif

