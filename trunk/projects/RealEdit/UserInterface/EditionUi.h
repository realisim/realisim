
#ifndef RealEdit_EditionUi_hh
#define RealEdit_EditionUi_hh

namespace RealEdit{ class EditionUi; }
namespace Realisim{ class Widget3d; }

#include <QMainWindow.h>

class QMenuBar;

namespace RealEdit{ class RealEditController; }

class QListWidget;

class RealEdit::EditionUi : public QMainWindow
{
	Q_OBJECT
public:
	EditionUi();
	~EditionUi();
	

	
protected:
	
private slots:
	void newProject();
	//void openProject();
	
private:
	void addMenuBar();
	void addObjectNavigator();
	
	void createFileMenu( QMenuBar* ipMenuBar );
	void createEditMenu( QMenuBar* ipMenuBar );
	void createToolMenu( QMenuBar* ipMenuBar );
	
	RealEditController* mpController; //created and deleted within the class
	
	Realisim::Widget3d* mpWidget3d_1;
	Realisim::Widget3d* mpWidget3d_2;
	
	QListWidget* mpObjectNavigator;
};

#endif //Realisim_RealEdit_UserInterface_MainWindow_hh
