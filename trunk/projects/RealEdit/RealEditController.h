/*-----------------------------------------------------------------------------
RealEditController.h
-----------------------------------------------------------------------------*/


#ifndef RealEdit_RealEditController_hh
#define RealEdit_RealEditController_hh

#include "DataModel.h"
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

  void createCube();
  void createSphere (unsigned int iLevel = 0);
  void createTetrahedron();
  DisplayData& getDisplayData() {return mDisplayData;}
  const EditionData& getEditionData() const;
  void newProject();
  void setCurrentNode (const ObjectNode* ipNode);

protected:
  EditionData& getEditionData();
  void subdivideIsocahedron(const std::vector<RealEditPoint>&, long);
private:
  DisplayData mDisplayData;
  EditionUi& mEditionUi;
  EditionData mEditionData;
  
  typedef std::map<std::pair<unsigned int, unsigned int>, RealEditPoint> IsocahedronSubdivision;
  std::map<std::pair<unsigned int, unsigned int>, RealEditPoint> mIsocahedronSubdivision;
};

#endif //RealEdit_RealEditController_hh