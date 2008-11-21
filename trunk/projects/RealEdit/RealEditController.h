/*-----------------------------------------------------------------------------
RealEditController.h
-----------------------------------------------------------------------------*/


#ifndef RealEdit_RealEditController_hh
#define RealEdit_RealEditController_hh

#include "EditionData.h"

namespace RealEdit
{ 
  class RealEditController;
	class EditionUi; 
}

class RealEdit::RealEditController 
{
public:
	RealEditController();
	~RealEditController();
	
	void newProject();
  
  const EditionData& getEditionData() const{return mEditionData;}

protected:

private:
  EditionData mEditionData;
};

#endif //RealEdit_RealEditController_hh