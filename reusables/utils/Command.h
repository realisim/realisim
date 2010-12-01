/*
 *  Command.h
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */

#ifndef realisim_utils_command_hh
#define realisim_utils_command_hh

/*Classe de base pour les commandes du système de undo/redo. Il suffit de la
  sous classer et de l'ajouter a utils::CommandStack. Ensuite, via le 
  CommandStack, on peut appeler CommandStack::undo qui appelle le Command::undo
  ou CommandStack::redo qui appelle Command::redo.
  
  notes sur les méthodes:
  execute(): sert à executer la commande une première fois. L'utilisateur de la
    classe doit explicitement appeler cette méthode. 
  undo(): sert à défaire ce que la commande à fait.
  redo(): sert a refaire la commande. Règle générale, redo appel execute()
    donc pas besoin de surcharger cette méthode. Par contre, dans certain cas
    execute() et redo() doivent être légèrement différent. Par exemple,
    execute() calculera des conditions initiales qui pourront être utilisées par
    redo().
  */
namespace realisim 
{
namespace utils 
{
  class Command
  {
    friend class CommandStack;
    
    public:
      Command(){};
      virtual ~Command(){};
      
      virtual void execute() = 0;
      
    protected:
      virtual void undo() = 0;
      virtual void redo() {execute();}
  };
}
}
#endif