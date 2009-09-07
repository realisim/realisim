
#include "commands, changeMode.h"
#include "commands, changeNode.h"
#include "commands, selection.h"
#include "DataModel.h"
#include "EditionUi.h"
#include "MathUtils.h"
#include "PlatonicSolid.h"
#include "RealEditController.h"

using namespace realisim;
using namespace realEdit;
using namespace std;

RealEditController::RealEditController(EditionUi& iEditionUi) :
  mCommandStack(), 
  mDisplayData(),
  mEditionUi(iEditionUi),
  mEditionData(),
  mMode(mEdition)
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
  createPlatonicSolid(PlatonicSolid::tIsocahedron, 1);
}

RealEditController::~RealEditController()
{
}

//------------------------------------------------------------------------------
void RealEditController::createPlatonicSolid(PlatonicSolid::type iType, 
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
RealEditController dans le but d'obliger les utilisateurs à passé par 
RealEditController afin de modifier les données.*/
EditionData& RealEditController::getEditionData()
{return mEditionData;}

//------------------------------------------------------------------------------
/*Cette méthode sert a offrir les données en lecture seulement a quiconque.*/
const EditionData& RealEditController::getEditionData() const
{return mEditionData;}

//------------------------------------------------------------------------------
void
RealEditController::newProject()
{
  EditionUi* p = new EditionUi();
  //pas besoin de deleter la fenetre, Qt le fera lorsque la fenêtre sera fermé
  p->setAttribute(Qt::WA_DeleteOnClose, true);
}

//------------------------------------------------------------------------------
void RealEditController::redo()
{ getCommandStack().redo(); getUi().update(); }

//------------------------------------------------------------------------------
//select all ids from the vector.
void RealEditController::select(vector<uint> iS)
{
  commands::Selection* c = new commands::Selection(getEditionData(), iS);
  getCommandStack().add(c);
  getUi().update();
}

//------------------------------------------------------------------------------
/*Place le noeud courant et notifie l'interface, qui placera ensuite les caméras
*/
void RealEditController::setCurrentNode (const ObjectNode* ipNode)
{
  if(getEditionData().getCurrentNode() != ipNode)
  {
    commands::ChangeNode* c = new commands::ChangeNode(getEditionData(),
      getUi(), ipNode);
    getCommandStack().add(c);
  }
}

//------------------------------------------------------------------------------
void RealEditController::setMode(mode iMode)
{
  if(mMode != iMode)
  {
    commands::ChangeMode* c = new commands::ChangeMode(*this, getUi(), iMode);
    getCommandStack().add(c);
  }
}

//------------------------------------------------------------------------------
void RealEditController::undo()
{ getCommandStack().undo(); getUi().update(); }
