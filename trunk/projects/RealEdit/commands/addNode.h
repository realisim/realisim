/*
 *  commands, translate.h
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */
 
#ifndef realedit_commands_addNode_hh
#define realedit_commands_addNode_hh

/*Ajoute un noeud enfant au noeud identifié par iParentId*/
namespace realEdit{class Controller;}
#include "utils/Command.h"

namespace realEdit
{
namespace commands 
{
  class AddNode : public realisim::utils::Command
  {
   public:
     
     explicit AddNode(Controller&, unsigned int iParentId);
     virtual ~AddNode();
     
     virtual void execute();
     virtual unsigned int getNodeId() const;
     
  protected:
    virtual void undo();
    
    Controller& mController;
    unsigned int mNodeId;
    unsigned int mParentId;
  };
}
}

#endif