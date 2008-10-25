
#include "WorkspaceUi.h"

#include "RealEditController.h"

using namespace RealEdit;

WorkspaceUi::WorkspaceUi( QWidget* ipParent,
						  RealEditController& iController,
						  Qt::WindowFlags iFlags /* = 0 */ )
: QFrame( ipParent, iFlags )
, mController( iController )
{
}

WorkspaceUi::~WorkspaceUi()
{
}

