/*-----------------------------------------------------------------------------
Controller.h

  Les commandes (commands, ...) sont une extension du controller et facilite
  grandement les undos/redos. Elles reçoivents généralement en paramêtres soit
  les données d'édition, soit le Ui (pour des fin des rafrîchissement) soit 
  Controller lui même. Les commandes reçoivent Controller dans
  le cas où l'action à accomplir se fait sur les membre de Controller 
  (la méthode setMode() par exemple). Dans ces cas, les commandes impliquées 
  doivent être friend avec Controller afin de leur permettre de modifier
  les membres de ce dernier.
  De plus, certaines commandes comme selection ou translate ont des états
  intermédiaires. i.e: Lors du drag de la souris, on execute la commande mais on
  ne veut pas stocker le résultat dans le CommandStack parce qu'on ne veut pas
  que ça fasse partie du undo/redo. Dans ce cas, le Controller possède un
  pointeur sur la commande et gère manuellement le moment ou celle-ci est 
  ajouté au CommandStack.

  invariants:
  mpEditionUi != 0
-----------------------------------------------------------------------------*/


#ifndef RealEdit_controller_hh
#define RealEdit_controller_hh

#include "CommandStack.h"
#include "commands, selection.h"
#include "DataModel.h"
#include "DisplayData.h"
#include "EditionData.h"
namespace realEdit{class EditionUi;}
namespace realEdit{namespace commands{class ChangeMode;}}
namespace realEdit{namespace commands{class ChangeTool;}}
namespace realEdit{namespace commands{class Translate;}}
#include "PlatonicSolid.h"

namespace realEdit
{ 
  using namespace commands;

class Controller 
{
public:
  friend class commands::ChangeMode;
  friend class commands::ChangeTool;

  enum mode{mAssembly, mEdition};
  enum tool{tSelection, tTranslation};
  
	Controller( EditionUi& ipEditionUi );
	~Controller();
  DisplayData& getDisplayData() {return mDisplayData;}
  const EditionData& getEditionData() const;
  mode getMode() const {return mMode;}
  tool getTool() const {return mTool;}
  void newProject();
  void redo();
  void undo();
  
  void select(const uint, Selection::mode);
  void selectEnd();
  void setCurrentNode (const ObjectNode* ipNode);
  void setMode(mode iM);
  void setTool(tool);
  void translate(const Vector3d&);
  void translateEnd();

protected:
  void createPlatonicSolid(PlatonicSolid::type, int = 0);
  realisim::utils::CommandStack& getCommandStack() {return mCommandStack;}
  EditionData& getEditionData();
  EditionUi& getUi() {return mEditionUi;}
  
private:
  realisim::utils::CommandStack mCommandStack;
  DisplayData mDisplayData;
  EditionUi& mEditionUi;
  EditionData mEditionData;
  mode mMode;
  tool mTool;
  
  //--- Commands
  commands::Selection* mpSelection;
  commands::Translate* mpTranslate;
};

} //realEdit

#endif //RealEdit_Controller_hh