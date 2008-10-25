
#ifndef RealEdit_AnimationUi_hh
#define RealEdit_AnimationUi_hh

namespace RealEdit{ class AnimationUi; }
namespace Realisim{ class Widget3d; }

#include "WorkspaceUi.h"

class QListWidget;

class RealEdit::AnimationUi : public RealEdit::WorkspaceUi
{
public:
	AnimationUi( QWidget* ipParent, RealEditController& iController, Qt::WindowFlags iFlags = 0 );
	~AnimationUi();

protected:
private:
	void addAnimationNavigator();

	Realisim::Widget3d* mpWidget3d_1;
	Realisim::Widget3d* mpWidget3d_2;

	QListWidget* mpAnimationNavigator;
};

#endif //RealEdit_AnimationUi_hh
