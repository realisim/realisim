/*
 *  MainWindow.cpp
 *  Realisim
 *
 *  Created by Pierre-Olivier Beaudoin on 10-05-29.
 */

#include "commands/addNode.h"
#include "commands/changeNode.h"
#include "commands/changeTool.h"
#include "commands/removeNode.h"
#include "commands/renameNode.h"
#include "Controller.h"
#include "MainWindow.h"
#include <QApplication>
#include <QButtonGroup>
#include <QDockWidget>
#include <QFrame>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLayout>
#include <QListWidget>
#include <QMenuBar>
#include <QPushButton>
#include <QShortcut>
#include <QTreeWidgetItem>

using namespace realisim;
using namespace realEdit;

MainWindow::MainWindow() 
  : QMainWindow(),
  mDummyProjectWindow(this),
  mpController(0),
  mpObjectNavigator( 0 ),
  mTreeItemToNodeId(),
  mNodeIdToTreeItem(),
  mpActiveProjectWindow(0),
  mProjectWindows(),
  mPalettes(),
  mpAdd(0),
  mpRemove(0)
{	
  setFocusPolicy(Qt::StrongFocus);
  move(0, 25);
	resize(200, 400);
	QFrame* pMainFrame = new QFrame( this );
	setCentralWidget( pMainFrame );
	addMenuBar();
	
	QGridLayout* pGLyt = new QGridLayout( pMainFrame );
  pGLyt->setContentsMargins(1, 1, 1, 1);
  pGLyt->setSpacing(3);
  
  //--add the Object Navigator
  mpObjectNavigator = new QTreeWidget(pMainFrame);
  mpObjectNavigator->header()->hide();
  mpObjectNavigator->setAlternatingRowColors(true);
  connect( mpObjectNavigator,
    SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*) ),
    this, SLOT(objectNodeSelectionChanged(QTreeWidgetItem*, QTreeWidgetItem*) ) );
  connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)),
    this, SLOT(handleFocusChanged(QWidget*, QWidget*)));
  connect(mpObjectNavigator, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
    this, SLOT(objectNodeChanged(QTreeWidgetItem*, int)));
    
  //--Ajout des boutons pour ajouter supprimer un object
  mpAdd = new QPushButton("Add", this);
  mpRemove = new QPushButton("Remove", this);
  QHBoxLayout* pButtonLyt = new QHBoxLayout(this);
  pButtonLyt->setContentsMargins(1, 1, 1, 1);
  pButtonLyt->addWidget(mpAdd);
  pButtonLyt->addWidget(mpRemove);
  pButtonLyt->addStretch(1);
	connect(mpAdd, SIGNAL(clicked()), this, SLOT(addObjectNode()));
  connect(mpRemove, SIGNAL(clicked()), this, SLOT(removeObjectNode()));
  
  //--Layout du panneau de la mainWindow
  int row = 0;
  pGLyt->addWidget(mpObjectNavigator, 0, 0);
  ++row;
  pGLyt->addLayout(pButtonLyt, row, 0);
  ++row;
  
  //--création des palettes
  mPalettes[pEditionTools] = new palette::Tools(this);
  
  //creations des shortcuts clavier
  QShortcut* pSelectShortcut = new QShortcut(QKeySequence("S"), this);
  pSelectShortcut->setContext(Qt::ApplicationShortcut);
  connect(pSelectShortcut, SIGNAL(activated()), this, SLOT(shortcutSelect()));
  
  QShortcut* pTranslateShortcut = new QShortcut(QKeySequence("T"), this);
  pTranslateShortcut->setContext(Qt::ApplicationShortcut);
  connect(pTranslateShortcut, SIGNAL(activated()), this, SLOT(shortcutTranslate()));
  
  setActiveProjectWindow(&mDummyProjectWindow);
}

MainWindow::~MainWindow()
{
  for(unsigned int i = 0; i < mProjectWindows.size(); ++i)
    delete mProjectWindows[i];
  mProjectWindows.clear();
}

//------------------------------------------------------------------------------
void MainWindow::addMenuBar()
{
	//create a menu bar
	QMenuBar* pMenuBar = new QMenuBar();
	//add a menu to the mainWindow
  setMenuBar( pMenuBar );
	
	createFileMenu( pMenuBar );
	createEditMenu( pMenuBar );
	createWindowMenu( pMenuBar );
}

//------------------------------------------------------------------------------
/*Cette fonction est appelée par le controlleur quand un noeud est ajouté et 
  elle sert a mettre la liste des noeuds a jours*/
void MainWindow::addNode(unsigned int iId)
{
	const EditionData& e = mpController->getEditionData();
  const ObjectNode* n = e.getNode(iId);
  const ObjectNode* pParent = n->getParentNode();
  QTreeWidgetItem* pParentItem = getNavigatorItem(pParent->getId());
  createTree(pParentItem, n);
}

//------------------------------------------------------------------------------
void MainWindow::addObjectNode()
{
  /*On change le focus pour corriger un bug avec l'edition de la ligne courante.
    Si on ne change pas le focus lors du click sur add, le champs d'édition
    n'acceptera pas les changement apportés.*/
  mpAdd->setFocus();
  this->setFocus();
  
  unsigned int currentNodeId = getObjectNode(mpObjectNavigator->currentItem());
  commands::AddNode* c = new commands::AddNode(*mpController, currentNodeId);
  c->execute();
  mpController->addCommand(c);
  
  QTreeWidgetItem* newItem = getNavigatorItem(c->getNodeId());
  mpObjectNavigator->scrollToItem(newItem);
  mpObjectNavigator->editItem(newItem, 0);
}

//------------------------------------------------------------------------------
void MainWindow::createEditMenu( QMenuBar* ipMenuBar )
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
void MainWindow::createFileMenu( QMenuBar* ipMenuBar )
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
template<class TreeItem>
void MainWindow::createTree (TreeItem ipItem,
                             const ObjectNode* ipNode)
{
  //create a new tree item
  QTreeWidgetItem* pItem = new QTreeWidgetItem( ipItem );
  //add the tree item and its corresponding ObjectNode to the map.
  mTreeItemToNodeId.insert( 
    make_pair<QTreeWidgetItem*, unsigned int>(pItem, ipNode->getId()) );
  
  mNodeIdToTreeItem.insert(
    make_pair<unsigned int, QTreeWidgetItem*>(ipNode->getId(), pItem) );
  
  pItem->setText( 0, ipNode->getName() );
  pItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
  mpObjectNavigator->scrollToItem(pItem);
  
  for( unsigned int i = 0; i < ipNode->getChildCount(); ++i )
  {
    createTree( pItem, ipNode->getChild( i ) );
  }
}

//------------------------------------------------------------------------------
void MainWindow::createWindowMenu( QMenuBar* ipMenuBar )
{
	QMenu* pToolsMenu = ipMenuBar->addMenu( QObject::tr( "&Window" ) );
	
	QAction* pTools = pToolsMenu->addAction( "Tools" );
  
  connect(pTools, SIGNAL(triggered()), this, SLOT(showTools()));
}

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
void MainWindow::expandAllItems()
{
  TreeItemToNodeId::const_iterator it = mTreeItemToNodeId.begin();
  while(it != mTreeItemToNodeId.end())
  {
    mpObjectNavigator->expandItem(it->first);
    ++it;
  }
}

//------------------------------------------------------------------------------
/*Cette méthode retourne l'id de l'object node représenté par le QTreeWidgetItem
  iptem. Règle générale, il y a toujours un id pour un ipItem sauf dans le cas
  où la méthode est appelé sur un item qui n'a pas encore été inséré dans la
  map. Dans ce cas un assert est émis. Il est important de conserver l'invariant:
  c'est-à-dire que cette méthode retourne toujours un Id.*/
unsigned int MainWindow::getObjectNode(QTreeWidgetItem* ipItem) const
{
  TreeItemToNodeId::const_iterator it = mTreeItemToNodeId.find( ipItem );
  if( it != mTreeItemToNodeId.end() )
  {
    return it->second;
  }
  assert(it != mTreeItemToNodeId.end() &&
   "Ceci ne devrait pas arriver... il y a un probleme avec la map...");
  return 0;
}

//------------------------------------------------------------------------------
//voir getObjectNode
QTreeWidgetItem* MainWindow::getNavigatorItem(unsigned int iId)
{
  NodeIdToTreeItem::const_iterator it = mNodeIdToTreeItem.find(iId);
  if( it != mNodeIdToTreeItem.end() )
  {
    return it->second;
  }
  assert(0 && "Ceci ne devrait pas arriver... il y a un probleme avec la map...");
  return 0;
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
void MainWindow::keyPressEvent(QKeyEvent* ipE)
{
	//on élimine les auto repeat
  if(ipE->isAutoRepeat())
    return;
    
  switch (ipE->key()) 
  {
    default: ipE->ignore(); QWidget::keyPressEvent(ipE); break;
  }
}

//------------------------------------------------------------------------------
void MainWindow::keyReleaseEvent(QKeyEvent*)
{
}

//------------------------------------------------------------------------------
void MainWindow::newProject()
{
  ProjectWindow* pW = new ProjectWindow(this);
  connect(pW, SIGNAL(aboutToClose(ProjectWindow*)),
    this, SLOT(projectWindowAboutToClose(ProjectWindow*)));

  pW->move(205, 25);
  pW->show();
  
  setActiveProjectWindow(pW);
  mProjectWindows.push_back(pW);
}

//------------------------------------------------------------------------------
/*Cette fonction sert a modifier le nom du node courant.*/
void MainWindow::objectNodeChanged(QTreeWidgetItem* ipItem, int iCol)
{
	unsigned int id = getObjectNode(ipItem);
  const EditionData& e = mpController->getEditionData();
  const ObjectNode* n = e.getNode(id);
  if(ipItem->text(0) != n->getName())
  {
		commands::RenameNode* c =
    	new commands::RenameNode(*mpController, id, ipItem->text(0));
	  c->execute();
  	mpController->addCommand(c);
  }
}

//------------------------------------------------------------------------------
void MainWindow::objectNodeSelectionChanged(QTreeWidgetItem* ipItem,
  QTreeWidgetItem* ipPreviousItem)
{
 	commands::ChangeNode* c = new commands::ChangeNode(*mpController,
    getObjectNode(ipItem));
  c->execute();
  mpController->addCommand(c); 
}

//------------------------------------------------------------------------------
void MainWindow::projectWindowAboutToClose(ProjectWindow* ipProjectWindow)
{
  mProjectWindows.erase(
    remove(mProjectWindows.begin(), mProjectWindows.end(), ipProjectWindow),
    mProjectWindows.end());

  if(!mProjectWindows.empty())
  {
    mProjectWindows.back()->activateWindow();
		mProjectWindows.back()->raise();
  }
  else
    setActiveProjectWindow(&mDummyProjectWindow);
}

//------------------------------------------------------------------------------
void MainWindow::removeObjectNode()
{
  unsigned int id = getObjectNode(mpObjectNavigator->currentItem());
  commands::RemoveNode* c = new commands::RemoveNode(*mpController, id);
  c->execute();
  mpController->addCommand(c);
}

//------------------------------------------------------------------------------
void MainWindow::removeNode(unsigned int iId)
{
  QTreeWidgetItem* pItem = getNavigatorItem(iId);
  NodeIdToTreeItem::iterator it = mNodeIdToTreeItem.find(iId);
  mNodeIdToTreeItem.erase(it);
  TreeItemToNodeId::iterator it2 = mTreeItemToNodeId.find(pItem);
  mTreeItemToNodeId.erase(it2);
  int numChild = pItem->childCount();
  for(int i = numChild - 1; i >= 0  ; --i)
    removeNode(mTreeItemToNodeId[pItem->child(i)]);
  delete pItem;
}

//------------------------------------------------------------------------------
void MainWindow::renameNode(unsigned int iId)
{
  const EditionData& e = mpController->getEditionData();
  const ObjectNode* n = e.getNode(iId);
  QTreeWidgetItem* pItem = getNavigatorItem(iId);
  mpObjectNavigator->blockSignals(true);
  pItem->setText(0, n->getName());
  mpObjectNavigator->blockSignals(false);
}

//------------------------------------------------------------------------------
void MainWindow::setActiveProjectWindow(ProjectWindow* ipW)
{
  //early out
	if(mpActiveProjectWindow == ipW)
    return;
    
  mpActiveProjectWindow = ipW;
  setController(mpActiveProjectWindow->getController());
  
  if(mpActiveProjectWindow == &mDummyProjectWindow)
  {
		mpAdd->setDisabled(true);
    mpRemove->setDisabled(true);
  }
  else
  {
    mpAdd->setEnabled(true);
    mpRemove->setEnabled(true);
  }

}

//------------------------------------------------------------------------------
void MainWindow::setController(Controller& iC)
{
  mpController = &iC;
  
  //-- on met a jour la liste des objectNode
  mpObjectNavigator->blockSignals(true);
  mpObjectNavigator->clear();
  mNodeIdToTreeItem.clear();
  mTreeItemToNodeId.clear();
  const EditionData& e = mpController->getEditionData();
  createTree( mpObjectNavigator, e.getRootNode() );
  mpObjectNavigator->blockSignals(false);
  
  std::map<paletteId, palette::Palette*>::iterator it = mPalettes.begin();
  for(; it != mPalettes.end(); ++it )
    it->second->setController(iC);
  
  updateUi();
}

//------------------------------------------------------------------------------
void MainWindow::shortcutSelect()
{
  commands::ChangeTool* c = 
    new commands::ChangeTool(*mpController, Controller::tSelect);
  c->execute();
  mpController->addCommand(c);
}

//------------------------------------------------------------------------------
void MainWindow::shortcutTranslate()
{
  commands::ChangeTool* c = 
    new commands::ChangeTool(*mpController, Controller::tTranslate);
  c->execute();
  mpController->addCommand(c);
}

//------------------------------------------------------------------------------
void MainWindow::showTools()
{
  getPalette<palette::Tools>(pEditionTools)->show();
}

//------------------------------------------------------------------------------
void MainWindow::updateUi()
{
  const ObjectNode* n = mpController->getEditionData().getCurrentNode();
  //selection du node courant sans trigger de signaux
  mpObjectNavigator->blockSignals(true);
  QTreeWidgetItem* pItem = 
    getNavigatorItem(n->getId());
  mpObjectNavigator->setCurrentItem(pItem);
  mpObjectNavigator->scrollToItem(pItem);
  mpObjectNavigator->blockSignals(false);
  
  /*Si le noeud courant n'a plus de parent (donc le root node) on désactive
    le bouton remove. On ne permet pas d'enlever le root. Il n'y a pas vraiment
    de raison qui empêche de l'enlever, mais je ne vois pas à quoi cela
    pourrait servir.*/
  mpRemove->setDisabled(!n->getParentNode());
}

