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

  invariants:
  mpEditionUi != 0
-----------------------------------------------------------------------------*/


#ifndef RealEdit_controller_hh
#define RealEdit_controller_hh

#include "CommandStack.h"
#include "DataModel.h"
#include "DisplayData.h"
#include "EditionData.h"
namespace realEdit{class EditionUi;}
namespace realEdit{namespace commands{class ChangeMode;}}
namespace realEdit{namespace commands{class ChangeTool;}}
#include "PlatonicSolid.h"

namespace realEdit
{ 

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
  void select(const vector<uint>&);
  void setCurrentNode (const ObjectNode* ipNode);
  void setMode(mode iM);
  void setTool(tool);
  void translate(const Vector3d&);

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
};

} //realEdit

#endif //RealEdit_Controller_hh