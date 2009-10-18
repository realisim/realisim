
#include "commands, changeMode.h"
#include "commands, changeNode.h"
#include "commands, changeTool.h"
#include "DataModel.h"
#include "EditionUi.h"
#include "MathUtils.h"
#include "PlatonicSolid.h"
#include "Controller.h"

using namespace realisim;
using namespace realEdit;
using namespace std;

Controller::Controller(EditionUi& iEditionUi) :
  mCommandStack(), 
  mDisplayData(),
  mEditionUi(iEditionUi),
  mEditionData(),
  mMode(mEdition),
  mTool(tSelection),
  mpSelectionCommand(0)
{
//  ObjectNode* pRootNode = getEditionData().getCurrentNode();
//  vector<RealEditPoint> vPoints;
//  RealEditPoint p = 
//      getEditionData().addPoint (Point3d(-1.0, 0.0, 0.0));
//    vPoints.push_back(p);
//  RealEditPoint p1 = 
//      getEditionData().addPoint (Point3d(1.0, 0.0, 0.0));
//    vPoints.push_back(p1);
//  RealEditPoint p2 = 
//      getEditionData().addPoint (Point3d(0.0, 1.0, 0.0));
//    vPoints.push_back(p2);
//  
//  getEditionData().addPolygon(vPoints);

  ObjectNode* pRootNode = getEditionData().getCurrentNode();
  createPlatonicSolid(PlatonicSolid::tTetrahedron);

  ObjectNode* pBouetteNode = getEditionData().addNode( "étron" );
  pBouetteNode->translate( Point3d( -8.0, 0.0, 0.0 ) );
  pBouetteNode->rotate( PI/2.0,
    Vector3d( 0.0, 0.0, 1.0 ) );
  getEditionData().setCurrentNode( pBouetteNode );
  createPlatonicSolid(PlatonicSolid::tCube, 2);
  
  ObjectNode* pNode = getEditionData().addNode( "bâche" );
  pNode->rotate( PI/4.0,
    Vector3d( 0.0, 0.0, 1.0 ) );
  pNode->translate( Vector3d( -8.0, 0.0, 0.0 ) );
  getEditionData().setCurrentNode( pNode );
  createPlatonicSolid(PlatonicSolid::tIsocahedron, 3);
}

Controller::~Controller()
{
  //s'il y a des commandes qui n'ont pas été assigné au commandStack, il faut
  //les deleter manuellement
  if(mpSelectionCommand)
    {delete mpSelectionCommand; mpSelectionCommand = 0;}
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
      getEditionData().addPoint (ps.getVertex()[i]);
    vPoints.push_back(p);
  }
  
  for (uint i = 0; i < ps.getFaces().size(); ++i)
  {
    vFace.clear();
    vFace.push_back(vPoints[ps.getFaces()[i].index1]);
    vFace.push_back(vPoints[ps.getFaces()[i].index2]);
    vFace.push_back(vPoints[ps.getFaces()[i].index3]);
    getEditionData().addPolygon(vFace);
  }
}

//------------------------------------------------------------------------------
/*Cette méthode est protégé afin d'être utilisée seulement par 
Controller dans le but d'obliger les utilisateurs à passé par 
Controller afin de modifier les données.*/
EditionData& Controller::getEditionData()
{return mEditionData;}

//------------------------------------------------------------------------------
/*Cette méthode sert a offrir les données en lecture seulement a quiconque.*/
const EditionData& Controller::getEditionData() const
{return mEditionData;}

//------------------------------------------------------------------------------
void
Controller::newProject()
{
  EditionUi* p = new EditionUi();
  //pas besoin de deleter la fenetre, Qt le fera lorsque la fenêtre sera fermé
  p->setAttribute(Qt::WA_DeleteOnClose, true);
}

//------------------------------------------------------------------------------
void Controller::redo()
{ getCommandStack().redo(); getUi().update(); }

//------------------------------------------------------------------------------
//select all ids from the vector.
void Controller::select(const uint iS, commands::Selection::mode iMode)
{
  if(mpSelectionCommand == 0)
    mpSelectionCommand = new commands::Selection(getEditionData(), iS, iMode);
  else
    mpSelectionCommand->update(iS, iMode);
    
  mpSelectionCommand->execute();
  getUi().update();
}

//------------------------------------------------------------------------------
void Controller::selectEnd()
{
  if(mpSelectionCommand)
    mCommandStack.add(mpSelectionCommand);
  mpSelectionCommand = 0;
}

//------------------------------------------------------------------------------
/*Place le noeud courant et notifie l'interface, qui placera ensuite les caméras
*/
void Controller::setCurrentNode (const ObjectNode* ipNode)
{
  if(getEditionData().getCurrentNode() != ipNode)
  {
    commands::ChangeNode* c = new commands::ChangeNode(getEditionData(),
      getUi(), ipNode);
    c->execute();
    getCommandStack().add(c);
  }
}

//------------------------------------------------------------------------------
void Controller::setMode(mode iMode)
{
  if(mMode != iMode)
  {
    commands::ChangeMode* c = new commands::ChangeMode(*this, getUi(), iMode);
    c->execute();
    getCommandStack().add(c);
  }
}

//------------------------------------------------------------------------------
void Controller::setTool(tool iTool)
{
  if(mTool != iTool)
  {
    commands::ChangeTool* c = new commands::ChangeTool(*this, getUi(), iTool);
    c->execute();
    getCommandStack().add(c);
  }
}

//------------------------------------------------------------------------------
void Controller::translate(const Vector3d& iDelta)
{
  if(getEditionData().hasSelection())
  {
    if(getMode() == mEdition)
    {
      for(uint i = 0; i < getEditionData().getSelectedPoints().size(); ++i)
      {
        RealEditPoint p = getEditionData().getSelectedPoints()[i];
        p.set(p.pos() + iDelta);      
      }

      getEditionData().getCurrentModel().updateNormals();
      getEditionData().getCurrentModel().updateBoundingBox();
    }
    else
    {
      getEditionData().getCurrentNode()->translate(iDelta);
    }
    
    getUi().update();
  }
}

//------------------------------------------------------------------------------
void Controller::undo()
{ getCommandStack().undo(); getUi().update(); }
