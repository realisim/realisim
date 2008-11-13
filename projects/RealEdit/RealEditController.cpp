
#include "RealEditController.h"

#include "EditionUi.h"

using namespace RealEdit;

RealEditController::RealEditController()
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


