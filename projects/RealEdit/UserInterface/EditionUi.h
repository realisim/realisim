
#ifndef RealEdit_EditionUi_hh
#define RealEdit_EditionUi_hh


#include "RealEditController.h"

#include <QMainWindow.h>

class QMenuBar;

namespace RealEdit{ class RealEditController; }
//namespace Realisim{ class Widget3d; }


namespace RealEdit
{ 
  class EditionUi; 
  class RealEdit3d;

}

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
	
	RealEditController mController; //created and deleted within the class
	
  RealEdit3d* mpWidget3d_1;
  RealEdit3d* mpWidget3d_2;
	
	QListWidget* mpObjectNavigator;
};

#endif //Realisim_RealEdit_UserInterface_MainWindow_hh
