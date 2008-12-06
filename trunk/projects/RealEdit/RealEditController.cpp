
#include "RealEditController.h"

#include "EditionUi.h"
#include "Point.h"

using namespace Realisim;
using namespace RealEdit;

RealEditController::RealEditController() : mEditionData()
{
  mEditionData.addPoint( Point3f( -2.0, 0.0, 0.0 ) );
  mEditionData.addPoint( Point3f( 2.0, 0.0, 0.0 ) );
  mEditionData.addPoint( Point3f( 0.0, 2.0, 0.0 ) );
  
  mEditionData.addNode( "bouette" );
  mEditionData.addNode( "bouette1" );
  ObjectNode* pNode = mEditionData.addNode( "bouette3" );
  
  mEditionData.setCurrentNode( pNode );
  mEditionData.addPoint(Point3f( -7.0, 0.0, 0.0 ) );
  mEditionData.addPoint(Point3f( -5.0, 0.0, 0.0 ) );
  mEditionData.addPoint(Point3f( -6.0, 2.0, 0.0 ) );
  
  mEditionData.addNode( "caca" );
  mEditionData.addNode( "caca2" );
  mEditionData.addNode( "caca3" );
}

RealEditController::~RealEditController()
{
}

void
RealEditController::newProject()
{
	new EditionUi();
}


