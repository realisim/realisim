
#include "RealEditController.h"

#include "EditionUi.h"

using namespace RealEdit;

RealEditController::RealEditController() : mEditionData()
{
  
}

RealEditController::~RealEditController()
{
}

void
RealEditController::newProject()
{
	new EditionUi();
}


