/*
 *  Command.h
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */

#ifndef realisim_utils_command_hh
#define realisim_utils_command_hh

/*Classe de base pour les commandes du système de undo/redo.*/

namespace realisim 
{
namespace utils 
{
  class Command
  {
    public:
      Command(){};
      virtual ~Command(){};
      
      virtual void execute() = 0;
      virtual void undo() = 0;
  };
}
}
#endif