/*-----------------------------------------------------------------------------
WorkspaceUi.h

Cette classe sert de classe de base a tout les workspaces  ( edition, animation
etc.. )

Elle possede une réference sur RealEditController ( qui créé tout l'interface 
graphique de RealEdit ).

Le but d'avoir une réference sur realEditController est d'offrir la pssibilité
aux workspaces d'instancier des widgets qui doivent appartenir a la MainWindow.

example:
	QDockWidget* pDockWidget = mController.addDockWidget( this );
	QMenuItem* pMenuItem = mController.addMenuItem( this, 'tools', 'extrude' );

-----------------------------------------------------------------------------*/

#ifndef RealEdit_WorkspaceUi_hh
#define RealEdit_WorkspaceUi_hh

#include <QFrame>


namespace RealEdit{ class WorkspaceUi;
					class RealEditController;}

class RealEdit::WorkspaceUi : public QFrame
{
public:
	WorkspaceUi( QWidget* ipParent, RealEditController& mController, Qt::WindowFlags iFlags = 0 );
	~WorkspaceUi();

protected:
	RealEditController& mController; 

private:

};

#endif //RealEdit_WorkspaceUi_hh
