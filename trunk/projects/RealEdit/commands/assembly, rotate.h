/*
 *  assembly, rotate.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 09-08-09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
 
#include "Command.h"
namespace realEdit {class EditionData;}
 
namespace realEdit
{
namespace commands
{
namespace assembly
{
  class Rotate : public realisim::utils::Command
  {
    public:
     explicit Rotate(EditionData&);
     virtual ~Rotate();
     
     virtual void execute();
     virtual void undo();
     
  private:
    realEdit::EditionData& mEditionData;
  }; 
}
}
}
