
#ifndef RealEdit_EditionUi_hh
#define RealEdit_EditionUi_hh

namespace realEdit{ class ObjectNavigator; }
#include "RealEditController.h"
#include <QMainWindow.h>
class QMenuBar;
namespace realEdit{ class RealEdit3d; }
namespace realEdit{ class RealEditController; }

namespace realEdit
{ 

class EditionUi : public QMainWindow
{
	Q_OBJECT
public:
	EditionUi();
	~EditionUi();
  
  void currentNodeChanged();

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
  RealEdit3d* mpWidget3d_3;
  RealEdit3d* mpWidget3d_4;

	ObjectNavigator* mpObjectNavigator;
};

} //realEdit

#endif //Realisim_RealEdit_UserInterface_MainWindow_hh
