
#ifndef RealEdit_EditionUi_hh
#define RealEdit_EditionUi_hh

namespace realEdit{ class ObjectNavigator; }
#include "Controller.h"
#include <QMainWindow.h>
class QMenuBar;
class QPushButton;
namespace realEdit{ class RealEdit3d; }
namespace realEdit{ class Controller; }

namespace realEdit
{ 

class EditionUi : public QMainWindow
{
	Q_OBJECT
public:
	EditionUi();
	~EditionUi();
  
void changeCurrentNode();
void changeMode();
void changeTool();
  
public slots:
  virtual void update();
	
private slots:
  void doChangeTool(int);
  void doModeChange(int);
  void doUndo();
  void doRedo();
	void newProject();
	//void openProject();
	
private:
	void addMenuBar();
	void addToolPanel();
  	
	void createFileMenu( QMenuBar* ipMenuBar );
	void createEditMenu( QMenuBar* ipMenuBar );
	void createToolMenu( QMenuBar* ipMenuBar );
	
	Controller mController; //created and deleted within the class
	ObjectNavigator* mpObjectNavigator;
  std::vector<RealEdit3d*> mViewers;
  //mode
  QPushButton* mpAssembly;
  QPushButton* mpEdition;
  //tools
  QPushButton* mpSelection;
  QPushButton* mpTranslation;
  
};

} //realEdit

#endif //Realisim_RealEdit_UserInterface_MainWindow_hh
