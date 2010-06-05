/*
 *  MainWindow.h
 *  Realisim
 *
 *  Created by Pierre-Olivier Beaudoin on 10-05-29.
 */
 

#ifndef RealEdit_MainWindow_hh
#define RealEdit_MainWindow_hh


#include <QMainWindow.h>
class QMenuBar;
class QPushButton;
namespace realEdit{ class Controller; }
namespace realEdit{ class ObjectNavigator; }
namespace realEdit{ class ProjectWindow; }
namespace realEdit{ class RealEdit3d; }

namespace realEdit
{ 

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow();
  
  void updateUi();
    
private slots:
  void doUndo();
  void doRedo();
  void handleFocusChanged(QWidget*, QWidget*);
	void newProject();
  
	//void openProject();
  
	
private:
	void addMenuBar();
	//void addToolPanel();
	void createFileMenu(QMenuBar*);
	void createEditMenu(QMenuBar*);
	void createToolMenu(QMenuBar*);
  void setActiveProjectWindow(ProjectWindow*);
  void setController(Controller&);
	
  Controller* mpController;
	ObjectNavigator* mpObjectNavigator;
  ProjectWindow* mpActiveProjectWindow;
  std::vector<ProjectWindow*> mProjectWindows;
  //mode
  QPushButton* mpAssembly;
  QPushButton* mpEdition;
  //tools
  QPushButton* mpSelection;
  QPushButton* mpTranslation;
  
};

} //realEdit

#endif //Realisim_RealEdit_UserInterface_MainWindow_hh


