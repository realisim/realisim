/*-----------------------------------------------------------------------------
RealEditController.h


RealEditController gère la creation de tout le UI de l'application. Il permet
aussi au diffent WorspaceUi ( animation, edition ... ) d'ajouter des éléments
( QDockWiget, items de menu etc... ) a la MainWindow. Il gère aussi le 
hide/show des widgets propre a un workspaceUi. Par exemple, le ui d'édition
a besoin d'un dockwidget qui contient le navigateur d'object. Par contre, 
le ui d'animation n'a pas besoin de ce dockwidget. Quand l'usager cliquera
sur le tab de l'animation, le dockwidget d'édition ( ainsi que tout les autres
widgets propre à l'édition ) se cacheront et les widgets d'animation 
s'afficheront.

important: Il faut que tout le ui soit créé par les
constructeurs des workspaceUi ( edition, animation, etc... ) pour que 
RealEditContoller puisse initialiser correctement le Ui de l'application.

-----------------------------------------------------------------------------*/


#ifndef RealEdit_RealEditController_hh
#define RealEdit_RealEditController_hh

#include <map>

#include <QObject>

class QDockWidget;
class QTabWidget;
class QMainWindow;
class QWidget;

namespace RealEdit{ class RealEditController;
					class EditionUi;
					class AnimationUi;
					class WorkspaceUi; }

class RealEdit::RealEditController : public QObject
{
	Q_OBJECT
public:
	RealEditController();
	~RealEditController();

	enum WorkspaceUiTabIndex
	{
		EditionTabIndex = 0,
		AnimationTabIndex
	};

	//----------------gestion du ui
	QDockWidget* addDockWidget( WorkspaceUi* ipRequester );
	//QMenuItem* addMenuItem( a venir... );

protected:

private slots:
	void handleCurrentTabChanged( int );

private:
	void addEditionUi( QTabWidget* ipCentralWidget );
	void addAnimationUi( QTabWidget* ipCentralWidget );

	void hideAllWorkspaceUiWidgets();
	void showWorkspaceUiWidgets( WorkspaceUi* ipWorkspace );
	void hideWorkspaceUiWidgets( WorkspaceUi* ipWorkspace );

	QMainWindow* mpMainWindow;
	EditionUi* mpEditionUi;
	AnimationUi* mpAnimationUi;

	//cette map contient la relation entre le WorkspaceUi et tous ses
	//widgets qui sont rattachés a la MainWindow ( le dockWidget, les
	//items de menu, items de tool bar etc... )
	typedef std::map<WorkspaceUi*, QWidget*> WorkspaceUiToWidgetMap;
	WorkspaceUiToWidgetMap mWorkspaceUiToWidgetMap;
};

#endif //RealEdit_RealEditController_hh