/*-----------------------------------------------------------------------------
Controller.h
  Le controlleur est la classe centrale de l'application. Il possède les données
  pour un projet. Un projet est réprésenté dans une fenêtre et peu contenir
  autant d'objets qu'on le désire. Le controlleur possède les données d'édition
  (EditionData, i.e: les noeuds, modèle, polygones, lignes et point ainsi que
  les textures, shaders etc...), le mode courant (edition, assemblage) ainsi que
  l'outil courant et le plus important la pile de commande (voir
  utils::commandStack). Il est le cerveau de l'opération. Il a aussi accès à
  toutes les parties de l'interface usager. Le controlleur commande 
  l'application.
  
  note sur la pile de commande:
  Lorsque cette dernière est modifiée, ajout d'une commande ou appel à undo, 
  un dirty flag est activer afin de notifier l'usager que son travail est 
  modifié et qu'une sauvegarder est requise.
  
  Notes d'architecture:
  Le controlleur devrait être accessible de partout, de chaque parties de 
  l'interface, de chaque commandes. C'est grâce à lui qu'on peut faire le pont 
  entre les différentes parties de l'application. Généralement, une action
  usager devrait être une commande (voir utils::command) qui reçoit le 
  controlleur en paramêtre, modifie les données à sa guise et ensuite notifie
  les reste de l'application si besoin ait.
  
  Les données sont accessibles de partout mais règle générale, elles devraient
  être modifier dans les commandes afin de supporter les undo/redo. Les autres
  endroit devraient utiliser les version const des méthode get.
  
  membres:
  mProjectWindow: La fenetre de projet, qui contient les visualeurs 3d.
  mCommandStack: la pile de commande qui permet les undo, redo.
  mDisplayData: les liste d'affichage partagée.
  mEditionData: les données d'édition pour le projet courant
  mMode: le mode courant (edition, assemblage)
  mTool: l'outil courant.
-----------------------------------------------------------------------------*/


#ifndef RealEdit_controller_hh
#define RealEdit_controller_hh


#include "DataModel/DataModel.h"
#include "DataModel/DisplayData.h"
#include "DataModel/EditionData.h"
#include "math/PlatonicSolid.h"
namespace realEdit{class ProjectWindow;}
namespace realEdit{class MainWindow;}
namespace realisim{namespace utils{class Command;}}
#include "utils/CommandStack.h"

namespace realEdit
{ 

class Controller
{
public:
  enum mode{mAssembly, mEdition};
  enum tool{tSelect, tTranslate, tExtrude};
  
	Controller(ProjectWindow&);
	virtual ~Controller();
  
  virtual void addCommand(utils::Command*);
  virtual bool canChangeTool() const {return mCanChangeTool;}
  virtual DisplayData& getDisplayData() {return mDisplayData;}
  virtual const EditionData& getEditionData() const;
  virtual EditionData& getEditionData();
  virtual MainWindow& getMainWindow();
  virtual mode getMode() const {return mMode;}
  virtual ProjectWindow& getProjectWindow();
  virtual tool getTool() const {return mTool;}
  virtual void redo();
  virtual void undo();
  virtual void setCanChangeTool(bool b) {mCanChangeTool = b;}
  virtual void setMode(mode iM);
  virtual void setTool(tool);

protected:
  virtual void createPlatonicSolid(PlatonicSolid::type, int = 0);
  virtual realisim::utils::CommandStack& getCommandStack() {return mCommandStack;}
  
private:
  ProjectWindow& mProjectWindow;
  realisim::utils::CommandStack mCommandStack;
  DisplayData mDisplayData;
  EditionData mEditionData;
  mode mMode;
  tool mTool;
  bool mCanChangeTool;
//bool mNeedToSave;
};

} //realEdit

#endif //RealEdit_Controller_hh