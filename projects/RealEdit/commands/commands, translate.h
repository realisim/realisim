/*
 *  commands, translate.h
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */
 
 #ifndef realedit_commands_translate_hh
 #define realedit_commands_translate_hh

namespace realEdit{class EditionData;}
#include "Command.h"

namespace realEdit
{
namespace commands 
{
 class Translate : public realisim::utils::Command
 {
   public:
     explicit Translate(EditionData&);
     virtual ~Translate();
     
     virtual void execute();
     virtual void undo();
     
  private:
    realEdit::EditionData& mEditionData;
 };
}
}

#endif