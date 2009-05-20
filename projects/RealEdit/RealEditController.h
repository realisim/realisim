/*-----------------------------------------------------------------------------
RealEditController.h
-----------------------------------------------------------------------------*/


#ifndef RealEdit_RealEditController_hh
#define RealEdit_RealEditController_hh

#include "DisplayData.h"
#include "EditionData.h"
#include "Vect.h"

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

  void createCube();
  void createSphere();
  const DisplayData& getDisplayData() const {return mDisplayData;}
  DisplayData& getDisplayData() {return mDisplayData;}
  const EditionData& getEditionData() const{return mEditionData;}
  EditionData& getEditionData() {return mEditionData;}
  void newProject();
  void setCurrentNode( ObjectNode* ipNode );

protected:
  void subdivideIsocahedron(const std::vector<unsigned int>&, long);
private:
  DisplayData mDisplayData;
  EditionUi& mEditionUi;
  EditionData mEditionData;
  
  typedef std::map<std::pair<unsigned int, unsigned int>, unsigned int> IsocahedronSubdivision;
  std::map<std::pair<unsigned int, unsigned int>, unsigned int> mIsocahedronSubdivision;
};

#endif //RealEdit_RealEditController_hh