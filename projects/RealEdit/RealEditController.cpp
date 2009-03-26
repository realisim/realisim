
#include "RealEditController.h"

#include "EditionUi.h"
#include "Point.h"

using namespace Realisim;
using namespace RealEdit;

RealEditController::RealEditController(EditionUi& iEditionUi) : 
  mEditionUi(iEditionUi),
  mEditionData()
{
  ObjectNode* pRootNode = mEditionData.getCurrentNode();
//  pRootNode->rotate( PI/4.0,
//                    Vector3d( 0.0, 0.0, 1.0 ) );
  
  mEditionData.addPoint( Point3f( -2.0, 0.0, 0.0 ) );
  mEditionData.addPoint( Point3f( 2.0, 0.0, 0.0 ) );
  mEditionData.addPoint( Point3f( 0.0, 2.0, 0.0 ) );
  
  ObjectNode* pBouetteNode = mEditionData.addNode( "bouette" );
  pBouetteNode->rotate( 3*PI/4.0,
                       Vector3d( 1.0, 0.0, 0.0 ) );
  pBouetteNode->translate( Point3d( 8.0, 0.0, 0.0 ) );
//  pBouetteNode->rotate( PI/4.0,
//                       Vector3d( 0.0, 0.0, 1.0 ) );
  mEditionData.setCurrentNode( pBouetteNode );
  mEditionData.addPoint( Point3f( -2.0, 0.0, 0.0 ) );
  mEditionData.addPoint( Point3f( 2.0, 0.0, 0.0 ) );
  
  mEditionData.setCurrentNode(pRootNode);
  mEditionData.addNode( "bouette1" );
  
  ObjectNode* pNode = mEditionData.addNode( "bouette3" );
  pNode->translate( Point3d( -8.0, 0.0, 0.0 ) );
  pNode->rotate( PI/4.0,
    Vector3d( 0.0, 0.0, 1.0 ),
    Point3d( 0.0, 0.0, 0.0 ) );
  pNode->rotate( PI/4.0,
                Vector3d( 0.0, 0.0, 1.0 ),
                Point3d( 0.0, 0.0, 0.0 ) );
          
  
  mEditionData.setCurrentNode( pNode );
  mEditionData.addPoint( Point3f( -2.0, 0.0, 0.0 ) );
  mEditionData.addPoint( Point3f( 2.0, 0.0, 0.0 ) );
  mEditionData.addPoint( Point3f( 0.0, 2.0, 0.0 ) );
  
  mEditionData.addNode( "caca" );
  
  mEditionData.addNode( "caca2" );
  mEditionData.addNode( "caca3" );
  
}

RealEditController::~RealEditController()
{
}

//------------------------------------------------------------------------------
void
RealEditController::newProject()
{
	new EditionUi();
}

//------------------------------------------------------------------------------
void RealEditController::setCurrentNode( ObjectNode* ipNode )
{
  mEditionData.setCurrentNode(ipNode);
  mEditionUi.currentNodeChanged();
}



