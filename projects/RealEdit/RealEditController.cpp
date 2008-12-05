
#include "RealEditController.h"

#include "EditionUi.h"
#include "Point.h"

using namespace Realisim;
using namespace RealEdit;

RealEditController::RealEditController() : mEditionData()
{
  mEditionData.setCurrentModel( 
    mEditionData.getScene().getObjectNode()->getModel() );
  
  mEditionData.addPoint(Point3f( -2.0, 0.0, 0.0 ) );
  mEditionData.addPoint(Point3f( 2.0, 0.0, 0.0 ) );
  mEditionData.addPoint(Point3f( 0.0, 2.0, 0.0 ) );
  
  mEditionData.addNode( mEditionData.getScene().getObjectNode(), "bouette" );
  mEditionData.addNode( mEditionData.getScene().getObjectNode(), "bouette1" );
  ObjectNode* pNode = mEditionData.addNode( mEditionData.getScene().getObjectNode(), "bouette3" );
  
  mEditionData.setCurrentModel( pNode->getModel() );
  mEditionData.addPoint(Point3f( -7.0, 0.0, 0.0 ) );
  mEditionData.addPoint(Point3f( -5.0, 0.0, 0.0 ) );
  mEditionData.addPoint(Point3f( -6.0, 2.0, 0.0 ) );
  
  mEditionData.addNode( pNode, "caca" );
  mEditionData.addNode( pNode, "caca2" );
  mEditionData.addNode( pNode, "caca3" );
}

RealEditController::~RealEditController()
{
}

void
RealEditController::newProject()
{
	new EditionUi();
}


