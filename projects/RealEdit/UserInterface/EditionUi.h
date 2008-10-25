
#ifndef RealEdit_EditionUi_hh
#define RealEdit_EditionUi_hh

namespace RealEdit{ class EditionUi; }
namespace Realisim{ class Widget3d; }

#include "WorkspaceUi.h"

class QListWidget;

class RealEdit::EditionUi : public RealEdit::WorkspaceUi
{
public:
	EditionUi( QWidget* ipParent, RealEditController& iController, Qt::WindowFlags iFlags = 0 );
	~EditionUi();

protected:
private:
	void addObjectNavigator();

	Realisim::Widget3d* mpWidget3d_1;
	Realisim::Widget3d* mpWidget3d_2;

	QListWidget* mpObjectNavigator;
};

#endif //Realisim_RealEdit_UserInterface_MainWindow_hh
