/*-----------------------------------------------------------------------------
RealEditController.h
-----------------------------------------------------------------------------*/


#ifndef RealEdit_RealEditController_hh
#define RealEdit_RealEditController_hh

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
	
	void newProject();
  void setCurrentNode( ObjectNode* ipNode );
  
  const EditionData& getEditionData() const{return mEditionData;}
  EditionData& getEditionData() {return mEditionData;}


protected:

private:
  EditionUi& mEditionUi;
  EditionData mEditionData;
};

#endif //RealEdit_RealEditController_hh