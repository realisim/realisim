/*-----------------------------------------------------------------------------
RealEditController.h
-----------------------------------------------------------------------------*/


#ifndef RealEdit_RealEditController_hh
#define RealEdit_RealEditController_hh

#include "DisplayData.h"
#include "EditionData.h"

/* 
  invariants:
  mpEditionUi != 0
*/

namespace RealEdit
{ 
  class RealEditController;
	class EditionUi; 
}

class RealEdit::RealEditController 
{
public:
	RealEditController( EditionUi& ipEditionUi );
	~RealEditController();

  const DisplayData& getDisplayData() const {return mDisplayData;}
  DisplayData& getDisplayData() {return mDisplayData;}
  const EditionData& getEditionData() const{return mEditionData;}
  EditionData& getEditionData() {return mEditionData;}
  void newProject();
  void setCurrentNode( ObjectNode* ipNode );


protected:

private:
  DisplayData mDisplayData;
  EditionUi& mEditionUi;
  EditionData mEditionData;
};

#endif //RealEdit_RealEditController_hh