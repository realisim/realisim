
#include "utils/Command.h"
#include "Controller.h"
#include "dataModel/DataModel.h"
#include "math/MathUtils.h"
#include "math/PlatonicSolid.h"
#include "UserInterface/MainWindow.h"
#include "UserInterface/ProjectWindow.h"

using namespace realisim;
using namespace realEdit;
using namespace std;

Controller::Controller(ProjectWindow& iProjectWindow) :
  mProjectWindow(iProjectWindow),
  mCommandStack(), 
  mDisplayData(),
  mEditionData(),
  mMode(mEdition),
  mTool(tSelect),
  mCanChangeTool(true)
{
  ObjectNode* pRootNode = mEditionData.getCurrentNode();
  createPlatonicSolid(PlatonicSolid::tTetrahedron);

  ObjectNode* pBouetteNode =  pRootNode->addNode( "étron" );
  pBouetteNode->translate( Point3d( -8.0, 0.0, 0.0 ) );
  pBouetteNode->rotate( PI/2.0,
    Vector3d( 0.0, 0.0, 1.0 ) );
  mEditionData.setCurrentNode( pBouetteNode );
  createPlatonicSolid(PlatonicSolid::tCube, 2);
  
  ObjectNode* pNode = pBouetteNode->addNode( "bâche" );
  pNode->rotate( PI/4.0,
    Vector3d( 0.0, 0.0, 1.0 ) );
  pNode->translate( Vector3d( -800.0, 0.0, 0.0 ) );
  mEditionData.setCurrentNode( pNode );
  createPlatonicSolid(PlatonicSolid::tIsocahedron, 3);
}

Controller::~Controller()
{}

//------------------------------------------------------------------------------
void Controller::addCommand(utils::Command* ipC)
{ mCommandStack.add(ipC); }

//------------------------------------------------------------------------------
void Controller::createPlatonicSolid(PlatonicSolid::type iType, 
  int iLevel /*= 0*/)
{
  PlatonicSolid ps(iType, iLevel);
  RealEditModel m = getEditionData().getCurrentModel();
  
  vector<RealEditPoint> vPoints;
  vector<RealEditPoint> vFace;
  for (uint i = 0; i < ps.getVertex().size(); ++i) 
  {    
    RealEditPoint p(ps.getVertex()[i]);
    m.addPoint (p);
    vPoints.push_back(p);
  }
  
  for (uint i = 0; i < ps.getFaces().size(); ++i)
  {
    vFace.clear();
    vFace.push_back(vPoints[ps.getFaces()[i].index1]);
    vFace.push_back(vPoints[ps.getFaces()[i].index2]);
    vFace.push_back(vPoints[ps.getFaces()[i].index3]);
    m.addPolygon(vFace);
  }
}

//------------------------------------------------------------------------------
const EditionData& Controller::getEditionData() const
{ return mEditionData; }

//------------------------------------------------------------------------------
EditionData& Controller::getEditionData()
{ return mEditionData; }

//------------------------------------------------------------------------------
MainWindow& Controller::getMainWindow()
{ return *((MainWindow*)mProjectWindow.parentWidget()); }

//------------------------------------------------------------------------------
ProjectWindow& Controller::getProjectWindow()
{ return mProjectWindow; }

//------------------------------------------------------------------------------
void Controller::redo()
{ getCommandStack().redo(); }

//------------------------------------------------------------------------------
void Controller::setMode(mode iMode)
{
  if(mMode != iMode)
    mMode = iMode;
}

//------------------------------------------------------------------------------
void Controller::setTool(tool iTool)
{
  if(mTool != iTool)
    mTool = iTool;
}

//------------------------------------------------------------------------------
void Controller::undo()
{ getCommandStack().undo(); }
