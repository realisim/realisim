
#ifndef RealEdit_EditionUi_hh
#define RealEdit_EditionUi_hh

namespace RealEdit{ class EditionUi; }
namespace Realisim{ class Widget3d; }

#include <QMainWindow>

class QListWidget;

class RealEdit::EditionUi : public QMainWindow
{
public:
	EditionUi();
	~EditionUi();
	
	void addMenuBar();

protected:
private:
	void addObjectNavigator();

	Realisim::Widget3d* mpWidget3d_1;
	Realisim::Widget3d* mpWidget3d_2;

	QListWidget* mpObjectNavigator;
};

#endif //Realisim_RealEdit_UserInterface_MainWindow_hh
