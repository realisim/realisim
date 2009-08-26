/*-----------------------------------------------------------------------------
RealEditController.h
 
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
#include "PlatonicSolid.h"

namespace realEdit
{ 

class RealEditController 
{
public:
  enum mode{mAssembly, mEdition};
  
	RealEditController( EditionUi& ipEditionUi );
	~RealEditController();

  DisplayData& getDisplayData() {return mDisplayData;}
  const EditionData& getEditionData() const;
  mode getMode() const {return mMode;}
  void newProject();
  void setCurrentNode (const ObjectNode* ipNode);
  void setMode(mode iM);
  
void translate();
void rotate();

protected:
  void createPlatonicSolid(PlatonicSolid::type, int = 0);
  EditionData& getEditionData();
  
private:
  realisim::utils::CommandStack mCommandStack;
  DisplayData mDisplayData;
  EditionUi& mEditionUi;
  EditionData mEditionData;
  mode mMode;
};

} //realEdit

#endif //RealEdit_RealEditController_hh