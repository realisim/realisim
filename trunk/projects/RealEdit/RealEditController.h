/*-----------------------------------------------------------------------------
RealEditController.h
 
  invariants:
  mpEditionUi != 0
-----------------------------------------------------------------------------*/


#ifndef RealEdit_RealEditController_hh
#define RealEdit_RealEditController_hh

#include "DataModel.h"
#include "DisplayData.h"
#include "EditionData.h"
namespace realEdit{class EditionUi;}

namespace realEdit
{ 

class RealEditController 
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

} //realEdit

#endif //RealEdit_RealEditController_hh