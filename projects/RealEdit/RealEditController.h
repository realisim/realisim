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

namespace realEdit
{ 

class RealEditController 
{
public:
  enum MODE{mAssembly, mEdition};
  
	RealEditController( EditionUi& ipEditionUi );
	~RealEditController();

  void createCube();
  void createSphere (unsigned int iLevel = 0);
  void createTetrahedron();
  DisplayData& getDisplayData() {return mDisplayData;}
  const EditionData& getEditionData() const;
  MODE getMode() const {return mMode;}
  void newProject();
  void setCurrentNode (const ObjectNode* ipNode);
  void setMode(MODE iM);
  
void translate();
void rotate();

protected:
  EditionData& getEditionData();
  void subdivideIsocahedron(const std::vector<RealEditPoint>&, long);
  
private:
  realisim::utils::CommandStack mCommandStack;
  DisplayData mDisplayData;
  EditionUi& mEditionUi;
  EditionData mEditionData;
  MODE mMode;
  
  typedef std::map<std::pair<unsigned int, unsigned int>, RealEditPoint> IsocahedronSubdivision;
  std::map<std::pair<unsigned int, unsigned int>, RealEditPoint> mIsocahedronSubdivision;
};

} //realEdit

#endif //RealEdit_RealEditController_hh