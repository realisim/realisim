
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
}

RealEditController::~RealEditController()
{
}

void
RealEditController::newProject()
{
	new EditionUi();
}


