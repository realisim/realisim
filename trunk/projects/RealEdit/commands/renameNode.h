/*
 *  commands, translate.h
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */
 
 #ifndef realedit_commands_renameNode_hh
 #define realedit_commands_renameNode_hh

#include "QString"
namespace realEdit{class Controller;}
#include "utils/Command.h"

namespace realEdit
{
namespace commands 
{
  class RenameNode : public realisim::utils::Command
  {
   public:
     
     explicit RenameNode(Controller&, unsigned int, QString);
     virtual ~RenameNode();
     
     virtual void execute();
     
  protected:
    virtual void undo();
  
    Controller& mC;
    unsigned int mId;
    QString mOldName;
    QString mNewName;
  };
}
}

#endif