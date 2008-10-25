
#include "RealEditController.h"
#include "MainWindow.h"


using namespace RealEdit;

RealEditController::RealEditController()
: mpMainWindow( 0 )
{
	mpMainWindow = new MainWindow();
}

RealEditController::~RealEditController()
{
}