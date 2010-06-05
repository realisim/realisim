/*
 *  MainWindow.cpp
 *  Realisim
 *
 *  Created by Pierre-Olivier Beaudoin on 10-05-29.
 */

#include "Controller.h"
#include "MainWindow.h"
#include "UserInterface/ObjectNavigator.h"
#include "UserInterface/ProjectWindow.h"

#include <QApplication>
#include <QButtonGroup>
#include <QDockWidget>
#include <QFrame>
#include <QLayout>
#include <QListWidget>
#include <QMenuBar>
#include <QPushButton>

using namespace realisim;
using namespace realEdit;

MainWindow::MainWindow() 
  : QMainWindow(),
  mpController(0),
  mpObjectNavigator( 0 ),
  mpActiveProjectWindow(0),
  mProjectWindows(),
  mpAssembly(0),
  mpEdition(0),
  mpSelection(0),
  mpTranslation(0)
{	
  move(0, 25);
	resize(200, 400);
	QFrame* pMainFrame = new QFrame( this );
	setCentralWidget( pMainFrame );
	
	addMenuBar();
	
	QGridLayout* pGLyt = new QGridLayout( pMainFrame );
  pGLyt->setContentsMargins(1, 1, 1, 1);
  pGLyt->setSpacing( 1 );
  
  //add the Object Navigator
  mpObjectNavigator = new ObjectNavigator(pMainFrame);
	
  int row = 0;
  pGLyt->addWidget(mpObjectNavigator, 0, 0);
  ++row;

  connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)),
    this, SLOT(handleFocusChanged(QWidget*, QWidget*)));
    
  //--- initialisation du UI a partir de mEditionData
  //changeCurrentNode();
  //changeMode();
  //changeTool();
}

MainWindow::~MainWindow()
{
  mProjectWindows.clear();
}

//------------------------------------------------------------------------------
void
MainWindow::addMenuBar()
{
	//create a menu bar
	QMenuBar* pMenuBar = new QMenuBar();
	//add a menu to the mainWindow
  setMenuBar( pMenuBar );
	
	createFileMenu( pMenuBar );
	createEditMenu( pMenuBar );
	createToolMenu( pMenuBar );
}

//------------------------------------------------------------------------------
//void
//MainWindow::addToolPanel()
//{
//	QDockWidget* pDockWidget = new QDockWidget( "Tools", this );
//	addDockWidget( Qt::LeftDockWidgetArea, pDockWidget );
//	pDockWidget->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
//  pDockWidget->setFixedWidth(200);
//	
//	QFrame* pFrame = new QFrame( pDockWidget );
//	pDockWidget->setWidget( pFrame );
//	QVBoxLayout* vLyt = new QVBoxLayout( pFrame );
//	{
//    //--- Bouton de modes
//    QHBoxLayout* pModeLyt = new QHBoxLayout();
//    {
//      mpAssembly = new QPushButton("Assembly", pFrame);
//      mpAssembly->setCheckable(true);
//      mpAssembly->setFixedHeight(25);
//      mpEdition = new QPushButton("Edition", pFrame);
//      mpEdition->setCheckable(true);
//      mpEdition->setFixedHeight(25);
//
//      QButtonGroup* pButtonGroup = new QButtonGroup(pFrame);
//      pButtonGroup->setExclusive(true);
//      pButtonGroup->addButton(mpAssembly, 0);
//      pButtonGroup->addButton(mpEdition, 1);
//      
//      connect(pButtonGroup, SIGNAL(buttonClicked(int)), 
//        this, SLOT(doModeChange(int)));
//
//      pModeLyt->addWidget(mpAssembly);
//      pModeLyt->addWidget(mpEdition);
//      pModeLyt->addStretch(1);
//    }    
//    
//		mpObjectNavigator = new ObjectNavigator( pFrame, mController );
//    
//    //--- Bouton des outils
//    QHBoxLayout* pToolLyt = new QHBoxLayout();
//    {
//      //--- selection
//      mpSelection = new QPushButton("s", pFrame);
//      mpSelection->setCheckable(true);
//      mpSelection->setFixedHeight(25);
//      //--- translation
//      mpTranslation = new QPushButton("t", pFrame);
//      mpTranslation->setCheckable(true);
//      mpTranslation->setFixedHeight(25);
//
//      QButtonGroup* pButtonGroup = new QButtonGroup(pFrame);
//      pButtonGroup->setExclusive(true);
//      pButtonGroup->addButton(mpSelection, 0);
//      pButtonGroup->addButton(mpTranslation, 1);
//      
//      connect(pButtonGroup, SIGNAL(buttonClicked(int)), 
//        this, SLOT(doChangeTool(int)));
//
//      pToolLyt->addWidget(mpSelection);
//      pToolLyt->addWidget(mpTranslation);
//      pToolLyt->addStretch(1);
//    }
//    
//    vLyt->addLayout(pModeLyt);
//    vLyt->addWidget( mpObjectNavigator );
//    vLyt->addLayout( pToolLyt );
//    vLyt->addStretch(1);
//	}
//}

//------------------------------------------------------------------------------
//void MainWindow::changeMode()
//{
//  /*On s'assure que les boutons de mode sont synchronisé avec le mode courant
//  et on rafraichît les viewers*/
//  if(mController.getMode() == Controller::mAssembly)
//    mpAssembly->setChecked(true);
//  else
//    mpEdition->setChecked(true);
//  
//  for(unsigned int i = 0; i < mViewers.size(); ++i)
//    mViewers[i]->update();
//}

//------------------------------------------------------------------------------
//void MainWindow::changeTool()
//{
//  switch (mController.getTool()) 
//  {
//    case Controller::tSelection :
//      if(!mpSelection->isDown())
//        mpSelection->toggle();
//      break;
//    case Controller::tTranslation :
//      if(!mpTranslation->isDown())
//        mpTranslation->toggle();
//      break;
//    default:
//      break;
//  }
//}

//------------------------------------------------------------------------------
void
MainWindow::createFileMenu( QMenuBar* ipMenuBar )
{
	//add item to the menu bar and to the catalog
	QMenu* pFileMenu = ipMenuBar->addMenu( QObject::tr( "&File" ) );
	
	//add New Project menu item
	QAction* pNewProject = pFileMenu->addAction( "New Project", this, SLOT( newProject() ) );
	pNewProject->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_N ) );
	
	//add Open Project menu item
//	QAction* pOpenProject = pFileMenu->addAction( "Open Project", this, SLOT( openProject() ) );
//	pOpenProject->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_O ) );
}

//------------------------------------------------------------------------------
void
MainWindow::createEditMenu( QMenuBar* ipMenuBar )
{
	QMenu* pEditMenu = ipMenuBar->addMenu( QObject::tr( "&Edit" ) );
  QAction* pUndo = pEditMenu->addAction("&Undo");
  pUndo->setShortcut(Qt::CTRL + Qt::Key_Z);
  QAction* pRedo = pEditMenu->addAction("&Redo");
  pRedo->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_Z);
  
  connect(pUndo, SIGNAL(triggered()), this, SLOT(doUndo()));
  connect(pRedo, SIGNAL(triggered()), this, SLOT(doRedo()));
}

//------------------------------------------------------------------------------
void
MainWindow::createToolMenu( QMenuBar* ipMenuBar )
{
	QMenu* pToolsMenu = ipMenuBar->addMenu( QObject::tr( "&Tools" ) );
	
	QAction* pTools1 = pToolsMenu->addAction( "tool 1" );
	QAction* pTools2 = pToolsMenu->addAction( "tool 2" );
	QAction* pTools3 = pToolsMenu->addAction( "tool 3" );
}

//------------------------------------------------------------------------------
//void MainWindow::doChangeTool(int iButtonId)
//{
//  switch (iButtonId) {
//    case 0:
//      mController.setTool(Controller::tSelection);
//      break;
//    case 1:
//      mController.setTool(Controller::tTranslation);
//      break;
//    default:
//      break;
//  }
//}

//------------------------------------------------------------------------------
//void MainWindow::doModeChange(int iButtonId)
//{
//  if(iButtonId == 0)
//    mController.setMode(Controller::mAssembly);
//  else
//    mController.setMode(Controller::mEdition);
//}

//------------------------------------------------------------------------------
void MainWindow::doUndo()
{
  if(mpController)
    mpController->undo();
}

//------------------------------------------------------------------------------
void MainWindow::doRedo()
{
  if(mpController)
    mpController->redo();
}


//------------------------------------------------------------------------------
void MainWindow::handleFocusChanged(QWidget* ipOld, QWidget* ipNew)
{
  std::vector<ProjectWindow*>::iterator it = 
    std::find(mProjectWindows.begin(), mProjectWindows.end(), ipNew);
  if(it != mProjectWindows.end())
    setActiveProjectWindow((ProjectWindow*)ipNew);
}

//------------------------------------------------------------------------------
void MainWindow::newProject()
{
  ProjectWindow* pW = new ProjectWindow(this);
  //addDockWidget(Qt::RightDockWidgetArea, pW);
  //pW->setFeatures(QDockWidget::DockWidgetMovable);
  //pW->setAllowedAreas(Qt::NoDockWidgetArea);
  //pW->setFloating(true);
  pW->move(205, 25);
  pW->show();
  
  setActiveProjectWindow(pW);
  mProjectWindows.push_back(pW);
}

void MainWindow::setActiveProjectWindow(ProjectWindow* ipW)
{
  mpActiveProjectWindow = ipW;
  setController(mpActiveProjectWindow->getController());
}

//------------------------------------------------------------------------------
void MainWindow::setController(Controller& iC)
{
  mpController = &iC;
  mpObjectNavigator->setController(iC);
}

//------------------------------------------------------------------------------
void MainWindow::updateUi()
{
	mpObjectNavigator->updateUi();
	mpActiveProjectWindow->updateUi();
}

