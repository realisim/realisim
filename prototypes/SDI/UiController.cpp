
#include "UiController.h"
#include "ProjectSelectionUi.h"

UiController::UiController()
{
	newProject();
}

UiController::~UiController()
{}

void UiController::newProject()
{
	ProjectSelectionUi* pProjectWindow = new ProjectSelectionUi( *this );
}

void UiController::openProject()
{
}
