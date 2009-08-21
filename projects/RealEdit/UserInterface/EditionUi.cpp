
#include "EditionUi.h"
#include "RealEdit3d.h"
#include "ObjectNavigator.h"

#include <QButtonGroup>
#include <QDockWidget>
#include <QFrame>
#include <QLayout>
#include <QListWidget>
#include <QMenuBar>
#include <QPushButton>

using namespace realisim;
using namespace realisim::treeD;
using namespace realEdit;

EditionUi::EditionUi() 
  : QMainWindow(),
  mController( *this ),
  mpObjectNavigator( 0 ),
  mViewers(),
  mpAssembly(0),
  mpEdition(0)
{	
	resize(800, 600);
	QFrame* pMainFrame = new QFrame( this );
	setCentralWidget( pMainFrame );
	
	addMenuBar();
	
	QGridLayout* pGLyt = new QGridLayout( pMainFrame );
  pGLyt->setContentsMargins(1, 1, 1, 1);
  pGLyt->setSpacing( 1 );
	
	RealEdit3d* pV1 = new RealEdit3d( this, 0, mController);
  pV1->setCameraMode( Camera::ORTHOGONAL );
  pV1->setCameraOrientation( Camera::XY );
    
  RealEdit3d* pV2 = new RealEdit3d( this, pV1, mController);
  pV2->setCameraMode( Camera::ORTHOGONAL );
  pV2->setCameraOrientation( Camera::ZY );
  
	RealEdit3d* pV3 = new RealEdit3d (this, pV1, mController);
  pV3->setCameraMode( Camera::ORTHOGONAL );
  pV3->setCameraOrientation( Camera::XZ );

	RealEdit3d* pV4 = new RealEdit3d (this, pV1, mController);
  pV4->setCameraOrientation( Camera::FREE );

  mViewers.push_back(pV4);
  mViewers.push_back(pV1);
  mViewers.push_back(pV2);
  mViewers.push_back(pV3);
  
  pGLyt->addWidget(pV4, 0, 0, 1, 3);
  pGLyt->setRowStretch(0, 2);
  pGLyt->addWidget(pV1, 1, 0 );
  pGLyt->addWidget(pV2, 1, 1 );
  pGLyt->addWidget(pV3, 1, 2 ); 
	
	//add the Object Navigator
	addToolPanel();

  //--- initialisation du UI a partir de mEditionData
  modeChanged();
  currentNodeChanged();
  
	show();
}

EditionUi::~EditionUi()
{
}

//------------------------------------------------------------------------------
void
EditionUi::addMenuBar()
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
void
EditionUi::addToolPanel()
{
	QDockWidget* pDockWidget = new QDockWidget( "Tools", this );
	addDockWidget( Qt::LeftDockWidgetArea, pDockWidget );
	pDockWidget->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
	
	QFrame* pFrame = new QFrame( pDockWidget );
	pDockWidget->setWidget( pFrame );
	QVBoxLayout* vLyt = new QVBoxLayout( pFrame );
	{
    //--- Bouton de modes
    QHBoxLayout* pModeLyt = new QHBoxLayout(pFrame);
    {
      mpAssembly = new QPushButton("Assembly", pFrame);
      mpAssembly->setCheckable(true);
      mpEdition = new QPushButton("Edition", pFrame);
      mpEdition->setCheckable(true);

      QButtonGroup* pButtonGroup = new QButtonGroup(pFrame);
      pButtonGroup->setExclusive(true);
      pButtonGroup->addButton(mpAssembly, 0);
      pButtonGroup->addButton(mpEdition, 1);
      
      connect(pButtonGroup, SIGNAL(buttonClicked(int)), 
        this, SLOT(doModeChange(int)));

      pModeLyt->addWidget(mpAssembly);
      pModeLyt->addWidget(mpEdition);
      pModeLyt->addStretch(1);
    }    
    
		mpObjectNavigator = new ObjectNavigator( pFrame, mController );
    
    vLyt->addLayout(pModeLyt);
    vLyt->addWidget( mpObjectNavigator );
    vLyt->addStretch(1);
	}
}

//------------------------------------------------------------------------------
void
EditionUi::createFileMenu( QMenuBar* ipMenuBar )
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
EditionUi::createEditMenu( QMenuBar* ipMenuBar )
{
	QMenu* pEditMenu = ipMenuBar->addMenu( QObject::tr( "&Edit" ) );
}

//------------------------------------------------------------------------------
void
EditionUi::createToolMenu( QMenuBar* ipMenuBar )
{
	QMenu* pToolsMenu = ipMenuBar->addMenu( QObject::tr( "&Tools" ) );
	
	QAction* pTools1 = pToolsMenu->addAction( "tool 1" );
	QAction* pTools2 = pToolsMenu->addAction( "tool 2" );
	QAction* pTools3 = pToolsMenu->addAction( "tool 3" );
}

//------------------------------------------------------------------------------
void EditionUi::doModeChange(int iButtonId)
{
  if(iButtonId == 0)
    mController.setMode(RealEditController::mAssembly);
  else
    mController.setMode(RealEditController::mEdition);
}

//------------------------------------------------------------------------------
void EditionUi::currentNodeChanged()
{
  mpObjectNavigator->currentNodeChanged();
  for(unsigned int i = 0; i < mViewers.size(); ++i)
    mViewers[i]->currentNodeChanged();
}

//------------------------------------------------------------------------------
void EditionUi::modeChanged()
{
  /*On s'assure que les boutons de mode sont synchronisé avec le mode courant
  et on rafraichît les viewers*/
  if(mController.getMode() == RealEditController::mAssembly)
    mpAssembly->setChecked(true);
  else
    mpEdition->setChecked(true);
  
  for(unsigned int i = 0; i < mViewers.size(); ++i)
    mViewers[i]->update();
}

//------------------------------------------------------------------------------
void EditionUi::newProject()
{
	mController.newProject();
}
