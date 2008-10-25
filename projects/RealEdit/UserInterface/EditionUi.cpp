
#include "EditionUi.h"
#include "RealEditController.h"
#include "Widget3d.h"

#include <QDockWidget>
#include <QFrame>
#include <QLayout>
#include <QListWidget>

using namespace Realisim;
using namespace RealEdit;

EditionUi::EditionUi( QWidget* ipParent,
					  RealEditController& iController,
					  Qt::WindowFlags iFlags/* = 0*/ )
: WorkspaceUi( ipParent, iController, iFlags )
, mpWidget3d_1( 0 )
, mpWidget3d_2( 0 )
, mpObjectNavigator( 0 )
{
	QVBoxLayout* pVLyt = new QVBoxLayout( this );	

	mpWidget3d_1 = new Widget3d( this, mpWidget3d_1 );
	mpWidget3d_2 = new Widget3d( this, mpWidget3d_2 );

	pVLyt->addWidget( mpWidget3d_1 );
	pVLyt->addWidget( mpWidget3d_2 );

	//add the Object Navigator
	addObjectNavigator();

}

EditionUi::~EditionUi()
{
}

void
EditionUi::addObjectNavigator()
{
	//add the dock widget via the controller, because the 
	//controller manage the show/hide of the dock widget
	//when a different workspace is shown by the central tab widget
	QDockWidget* pDockWidget = mController.addDockWidget( this );
	pDockWidget->setWindowTitle( QObject::tr( "Object Navigation" ) );
	pDockWidget->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );

	//add the QlistWidget
	QFrame* pFrame = new QFrame( pDockWidget );
	pDockWidget->setWidget( pFrame );
	QVBoxLayout* vLyt = new QVBoxLayout( pFrame );
	{
		mpObjectNavigator = new QListWidget( pFrame );

		QFontMetrics dockWidgetFontMetric( pDockWidget->font() );
		int a = dockWidgetFontMetric.width( pDockWidget->windowTitle() );

		mpObjectNavigator->setMinimumWidth( a );
	}
	vLyt->addWidget( mpObjectNavigator );
}
