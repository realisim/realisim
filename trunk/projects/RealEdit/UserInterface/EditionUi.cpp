
#include "EditionUi.h"
#include "Widget3d.h"

#include <QLayout>

using namespace Realisim;
using namespace RealEdit;

EditionUi::EditionUi( QWidget* ipParent /* = 0 */, Qt::WindowFlags iFlags/* = 0*/ )
: QFrame( ipParent, iFlags )
, mpWidget3d_1( 0 )
, mpWidget3d_2( 0 )
{
	QVBoxLayout* pVLyt = new QVBoxLayout( this );
	mpWidget3d_1 = new Widget3d( this, mpWidget3d_1 );
	mpWidget3d_2 = new Widget3d( this, mpWidget3d_2 );

	pVLyt->addWidget( mpWidget3d_1 );
	pVLyt->addWidget( mpWidget3d_2 );

}

EditionUi::~EditionUi()
{
}

