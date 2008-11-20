/*-----------------------------------------------------------------------------
RealEditController.h
-----------------------------------------------------------------------------*/


#ifndef RealEdit_RealEditController_hh
#define RealEdit_RealEditController_hh

#include "ObjectNode.h"

namespace RealEdit
{ 
  class RealEditController;
	class EditionUi; 
  class ObjectNode;
}

class RealEdit::RealEditController 
{
public:
	RealEditController();
	~RealEditController();
	
	void newProject();
  
  const ObjectNode& getObjectNode() const{return mObjectNode;}

protected:

private:
  ObjectNode mObjectNode;
};

#endif //RealEdit_RealEditController_hh