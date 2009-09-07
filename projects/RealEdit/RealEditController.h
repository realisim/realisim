/*-----------------------------------------------------------------------------
RealEditController.h

  Les commandes (commands, ...) sont une extension du controller et facilite
  grandement les undos/redos. Elles reçoivents généralement en paramêtres soit
  les données d'édition, soit le Ui (pour des fin des rafrîchissement) soit 
  RealEditController lui même. Les commandes reçoivent RealEditController dans
  le cas où l'action à accomplir se fait sur les membre de RealEditController 
  (la méthode setMode() par exemple). Dans ces cas, les commandes impliquées 
  doivent être friend avec RealEditController afin de leur permettre de modifier
  les membres de ce dernier.

  invariants:
  mpEditionUi != 0
-----------------------------------------------------------------------------*/


#ifndef RealEdit_RealEditController_hh
#define RealEdit_RealEditController_hh

#include "CommandStack.h"
#include "DataModel.h"
#include "DisplayData.h"
#include "EditionData.h"
namespace realEdit{class EditionUi;}
namespace realEdit{namespace commands{class ChangeMode;}}
#include "PlatonicSolid.h"

namespace realEdit
{ 

class RealEditController 
{
public:
  friend class commands::ChangeMode;

  enum mode{mAssembly, mEdition};
  
	RealEditController( EditionUi& ipEditionUi );
	~RealEditController();
  DisplayData& getDisplayData() {return mDisplayData;}
  const EditionData& getEditionData() const;
  mode getMode() const {return mMode;}
  void newProject();
  void redo();
  void undo();
  void select(vector<uint>);
  void setCurrentNode (const ObjectNode* ipNode);
  void setMode(mode iM);

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
};

} //realEdit

#endif //RealEdit_RealEditController_hh