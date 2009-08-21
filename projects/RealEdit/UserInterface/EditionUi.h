
#ifndef RealEdit_EditionUi_hh
#define RealEdit_EditionUi_hh

namespace realEdit{ class ObjectNavigator; }
#include "RealEditController.h"
#include <QMainWindow.h>
class QMenuBar;
class QPushButton;
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
  void modeChanged();
  
protected:
	
private slots:
  void doModeChange(int);
	void newProject();
	//void openProject();
	
private:
	void addMenuBar();
	void addToolPanel();
  	
	void createFileMenu( QMenuBar* ipMenuBar );
	void createEditMenu( QMenuBar* ipMenuBar );
	void createToolMenu( QMenuBar* ipMenuBar );
	
	RealEditController mController; //created and deleted within the class
	ObjectNavigator* mpObjectNavigator;
  std::vector<RealEdit3d*> mViewers;
  QPushButton* mpAssembly;
  QPushButton* mpEdition;
};

} //realEdit

#endif //Realisim_RealEdit_UserInterface_MainWindow_hh
