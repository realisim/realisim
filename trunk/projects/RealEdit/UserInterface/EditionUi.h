
#ifndef RealEdit_EditionUi_hh
#define RealEdit_EditionUi_hh

namespace RealEdit{ class EditionUi; }
namespace Realisim{ class Widget3d; }

#include <QFrame>

class RealEdit::EditionUi : public QFrame
{
public:
	EditionUi( QWidget* ipParent = 0, Qt::WindowFlags iFlags = 0 );
	~EditionUi();

protected:
private:

	Realisim::Widget3d* mpWidget3d_1;
	Realisim::Widget3d* mpWidget3d_2;
};

#endif //Realisim_RealEdit_UserInterface_MainWindow_hh
