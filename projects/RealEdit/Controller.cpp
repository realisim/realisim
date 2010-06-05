
#include "commands/changeMode.h"
#include "commands/changeNode.h"
#include "commands/changeTool.h"
#include "commands/translate.h"
#include "Controller.h"
#include "dataModel/DataModel.h"
#include "math/MathUtils.h"
#include "math/PlatonicSolid.h"
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
  mTool(tSelection),
  mpSelection(0),
  mpTranslate(0)
{
//  ObjectNode* pRootNode = getEditionDataPriv().getCurrentNode();
//  vector<RealEditPoint> vPoints;
//  RealEditPoint p = 
//      getEditionDataPriv().addPoint (Point3d(-1.0, 0.0, 0.0));
//    vPoints.push_back(p);
//  RealEditPoint p1 = 
//      getEditionDataPriv().addPoint (Point3d(1.0, 0.0, 0.0));
//    vPoints.push_back(p1);
//  RealEditPoint p2 = 
//      getEditionDataPriv().addPoint (Point3d(0.0, 1.0, 0.0));
//    vPoints.push_back(p2);
//  
//  getEditionDataPriv().addPolygon(vPoints);

  ObjectNode* pRootNode = getEditionDataPriv().getCurrentNode();
  createPlatonicSolid(PlatonicSolid::tTetrahedron);

  ObjectNode* pBouetteNode = getEditionDataPriv().addNode( "étron" );
  pBouetteNode->translate( Point3d( -8.0, 0.0, 0.0 ) );
  pBouetteNode->rotate( PI/2.0,
    Vector3d( 0.0, 0.0, 1.0 ) );
  getEditionDataPriv().setCurrentNode( pBouetteNode );
  createPlatonicSolid(PlatonicSolid::tCube, 2);
  
  ObjectNode* pNode = getEditionDataPriv().addNode( "bâche" );
  pNode->rotate( PI/4.0,
    Vector3d( 0.0, 0.0, 1.0 ) );
  pNode->translate( Vector3d( -800.0, 0.0, 0.0 ) );
  getEditionDataPriv().setCurrentNode( pNode );
  createPlatonicSolid(PlatonicSolid::tIsocahedron, 3);
}

Controller::~Controller()
{
  //s'il y a des commandes qui n'ont pas été assigné au commandStack, il faut
  //les deleter manuellement
  if(mpSelection)
    {delete mpSelection; mpSelection = 0;}
  if(mpTranslate)
    {delete mpTranslate; mpTranslate = 0;}

}

//------------------------------------------------------------------------------
void Controller::createPlatonicSolid(PlatonicSolid::type iType, 
  int iLevel /*= 0*/)
{
  PlatonicSolid ps(iType, iLevel);
  
  vector<RealEditPoint> vPoints;
  vector<RealEditPoint> vFace;
  for (uint i = 0; i < ps.getVertex().size(); ++i) 
  {    
    RealEditPoint p = 
      getEditionDataPriv().addPoint (ps.getVertex()[i]);
    vPoints.push_back(p);
  }
  
  for (uint i = 0; i < ps.getFaces().size(); ++i)
  {
    vFace.clear();
    vFace.push_back(vPoints[ps.getFaces()[i].index1]);
    vFace.push_back(vPoints[ps.getFaces()[i].index2]);
    vFace.push_back(vPoints[ps.getFaces()[i].index3]);
    getEditionDataPriv().addPolygon(vFace);
  }
}

//------------------------------------------------------------------------------
/*Cette méthode est protégé afin d'être utilisée seulement par 
Controller dans le but d'obliger les utilisateurs à passé par 
Controller afin de modifier les données.*/
EditionData& Controller::getEditionDataPriv()
{ return mEditionData; }

//------------------------------------------------------------------------------
/*Cette méthode sert a offrir les données en lecture seulement a quiconque.*/
const EditionData& Controller::getEditionData() const
{ return mEditionData; }


//------------------------------------------------------------------------------
void Controller::redo()
{
  getCommandStack().redo();
  getProjectWindow().updateUi();
}

//------------------------------------------------------------------------------
//select all ids from the vector.
void Controller::select(const uint iS, commands::Selection::mode iMode)
{
  if(mpSelection == 0)
    mpSelection = new commands::Selection(getEditionDataPriv(), iS, iMode);
  else
    mpSelection->update(iS, iMode);
    
  mpSelection->execute();
  getProjectWindow().updateUi();
}

//------------------------------------------------------------------------------
void Controller::selectEnd()
{
  if(mpSelection)
    mCommandStack.add(mpSelection);
  mpSelection = 0;
}

//------------------------------------------------------------------------------
/*Place le noeud courant et notifie l'interface, qui placera ensuite les caméras
*/
void Controller::setCurrentNode (const ObjectNode* ipNode)
{
  if(getEditionDataPriv().getCurrentNode() != ipNode)
  {
    commands::ChangeNode* c = new commands::ChangeNode(
      getProjectWindow(), getEditionDataPriv(), ipNode);
    c->execute();
    getCommandStack().add(c);
		getProjectWindow().updateUi();
  }
}

//------------------------------------------------------------------------------
void Controller::setMode(mode iMode)
{
  if(mMode != iMode)
  {
    commands::ChangeMode* c = new commands::ChangeMode(*this, iMode);
    c->execute();
    getCommandStack().add(c);
    getProjectWindow().updateUi();
  }
}

//------------------------------------------------------------------------------
void Controller::setTool(tool iTool)
{
  if(mTool != iTool)
  {
    commands::ChangeTool* c = new commands::ChangeTool(*this, iTool);
    c->execute();
    getCommandStack().add(c);
    getProjectWindow().updateUi();
  }
}

//------------------------------------------------------------------------------
void Controller::translate(const Vector3d& iDelta)
{
  if(mpTranslate == 0)
    mpTranslate = new commands::Translate(getEditionDataPriv(), iDelta, getMode());
  else
    mpTranslate->update(iDelta);
    
  mpTranslate->execute();
  getProjectWindow().updateUi();
}

//------------------------------------------------------------------------------
void Controller::translateEnd()
{
  if(mpTranslate)
    getCommandStack().add(mpTranslate);
  mpTranslate = 0;
}


//------------------------------------------------------------------------------
void Controller::undo()
{ 
  getCommandStack().undo();
  getProjectWindow().updateUi();
}
