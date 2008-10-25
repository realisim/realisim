
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
	//The reference is given by RealEditController
	RealEditController& mController; 

private:

};

#endif //RealEdit_WorkspaceUi_hh
