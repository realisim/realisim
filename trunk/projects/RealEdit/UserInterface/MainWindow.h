/*
 *  MainWindow.h
 *  Created by Pierre-Olivier Beaudoin on 10-05-29.
 */

#ifndef RealEdit_MainWindow_hh
#define RealEdit_MainWindow_hh

#include <map>
class QKeyEvent;
#include <QMainWindow.h>
class QMenuBar;
class QPushButton;
#include <QTreeWidget>
namespace realEdit{ class Controller; }
namespace realEdit{ class ObjectNode; }
namespace realEdit{ class RealEdit3d; }
#include "UserInterface/ProjectWindow.h"
#include "UserInterface/Palettes/Tools.h"

/*Cette classe est la fenêtre principale de l'application. C'est ici que les
  menus File, Edit, Window etc... ainsi que leurs actions sont créés et gérés.
  De plus c'est ici que la liste des fenêtre de projets est gérée. Quand un
  fenêtre devient active (par l'interaction d'un usager), MainWindow s'empare
  du controlleur de cette fenêtre de projet afin d'afficher correctement les
  données de ce projet.
  
  membres:
  mDummyProjectWindow: Sert à traiter tous les cas ou il n'y a plus de vrai
    fenêtre de projet.  
  mpController: Le controlleur associé à la fenêtre de projet courante. Ce
    pointeur ne sera jamais null;
  mpObjectNavigator: La liste des noeuds
  mTreeItemToNodeId: une map des items de la liste de noeud vers les id des
    noeuds.
  mNodeIdToTreeItem: une map des id vers les items de la liste des noeuds.
  mpActiveProjectWindow: La fenêtre de projet active.
  mProjectWindows: La liste de toutes les fenêtres de projet.
  palette::Tools* mpTools: une palette
  mpAdd: le bouton qui sert a ajouter un noeud à la liste
  mpRemove: le bouton qui sert a enlever un noeud à la liste.
  */
namespace realEdit
{
class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow();
  
  enum paletteId{pEditionTools, pAssemblyTools, pTexture};
  
  void addNode(unsigned int);
  template<class T> T* getPalette(paletteId);
  void removeNode(unsigned int);
  void renameNode(unsigned int);
  void updateUi();

protected:
  virtual void keyPressEvent(QKeyEvent*);
  virtual void keyReleaseEvent(QKeyEvent*);
  
private slots:
  void addObjectNode();
  void doUndo();
  void doRedo();
  void handleFocusChanged(QWidget*, QWidget*);
	void newProject();
  void objectNodeChanged(QTreeWidgetItem*, int);
  void objectNodeSelectionChanged(QTreeWidgetItem*, QTreeWidgetItem*);
  void projectWindowAboutToClose(ProjectWindow*);
  void removeObjectNode();
  void shortcutSelect();
  void shortcutTranslate();
  void showTools();
	//void openProject();
  
	
private:
	void addMenuBar();
	//void addToolPanel();
  void createEditMenu(QMenuBar*);
	void createFileMenu(QMenuBar*);
  template<class TreeItem> void createTree(TreeItem, const ObjectNode*);
  void createWindowMenu(QMenuBar*);
  void expandAllItems();
  unsigned int getObjectNode(QTreeWidgetItem*) const;
  QTreeWidgetItem* getNavigatorItem(unsigned int);
  void setActiveProjectWindow(ProjectWindow*);
  void setController(Controller&);
	
  typedef std::map<QTreeWidgetItem*, unsigned int> TreeItemToNodeId;
  typedef std::map<unsigned int, QTreeWidgetItem*> NodeIdToTreeItem;
  
  ProjectWindow mDummyProjectWindow;
  Controller* mpController; //ne sera jamais null
  QTreeWidget* mpObjectNavigator;
  TreeItemToNodeId mTreeItemToNodeId;
  NodeIdToTreeItem mNodeIdToTreeItem;
  ProjectWindow* mpActiveProjectWindow;
  std::vector<ProjectWindow*> mProjectWindows;
  std::map<paletteId, palette::Palette*> mPalettes;
  QPushButton* mpAdd;
  QPushButton* mpRemove;
};

template<class T>
T* MainWindow::getPalette(paletteId iId)
{
  if(mPalettes.find(iId) != mPalettes.end())
		switch (iId) 
    {
      case pEditionTools:
        {
          palette::Tools* p =dynamic_cast<palette::Tools*>(mPalettes[iId]);
          assert(p);
          return p;
        }
        break;
      default:
        break;
    }
  return 0;
}

} //realEdit

#endif //Realisim_RealEdit_UserInterface_MainWindow_hh


