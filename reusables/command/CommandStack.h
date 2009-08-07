/*
 *  CommandStack.h
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */
 
 #ifndef realisim_command_commandManager_hh
 #define realisim_command_commandManager_hh
 
/*Cette classe gère les besoins de undo/redo. Il suffit de sous classer 
realisim::command::Command afin de pouvoir ajouter les commandes et le 
CommandManager gère la file de commandes.

Chaque nouvelle commande ajoutée est placée a la fin de la file. Lorsque l'index
n'est pas a la fin de la pile (un ou plusieurs undo ont été effectué) la 
prochaine commande sera insérée après l'index et toutes les commandes qui
suivaient l'index sont effacées.

mCommands: la liste des commandes
mIndex: identifie l'emplacement actuel dans la pile de commandes.
*/

namespace realisim {namespace command {class Command;}} 
 #include <vector>
 
 namespace realisim
 {
 namespace command 
 {
   class CommandStack
   {
     public:
       CommandStack();
       virtual ~CommandStack();
       
       void add(Command*);
       void redo();
       void undo();
       
     protected:       
       std::vector<Command*> mCommands;
       int mIndex;
   };
 }
 }

#endif