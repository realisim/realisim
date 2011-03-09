/*
 *  commands, translate.h
 *  Created by Pierre-Olivier Beaudoin on 27-11-10.
 */
 
#ifndef realedit_commands_removeNode_hh
#define realedit_commands_removeNode_hh

#include "DataModel/ObjectNode.h"
namespace realEdit{class Controller;}
#include "utils/Command.h"

/*Enleve le noeud indentifié de son parent.
	
  note sur les membres:
  Ici, on fait une copie (via le constructeur copie) du noeud (ainsi que tous
  ces enfants). Cette copie tient en compte les noms, transformation et modèles
  de chaque noeuds. Il faut aussi noté que le modèle utilise le partage
  explicite, donc cette copie de noeud ne fait qu'augmenter un compte de 
  référence sur le modèle et empêche que ce dernier soit complètement détruit
  lors d'un appel a delete. La copie locale à la commande permettra donc de
  faire undo/redo autant de fois que désiré. 
*/
namespace realEdit
{
namespace commands 
{
  class RemoveNode : public realisim::utils::Command
  {
  public:
     explicit RemoveNode(Controller&, unsigned int);
     virtual ~RemoveNode();
     
     virtual void execute();
     virtual unsigned int getNodeId() const;
     virtual void undo();
   
		private:
     Controller& mController;
     ObjectNode mNode;
     unsigned int mParentId;
  };
}
}

#endif