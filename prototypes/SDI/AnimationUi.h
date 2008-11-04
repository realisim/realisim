
#ifndef RealEdit_AnimationUi_hh
#define RealEdit_AnimationUi_hh

namespace RealEdit{ class AnimationUi; }
namespace Realisim{ class Widget3d; }

#include <QMainWindow>

class QListWidget;

class RealEdit::AnimationUi : public QMainWindow
{
public:
	AnimationUi();
	~AnimationUi();
	
	void addMenuBar();

protected:
private:
	void addAnimationNavigator();

	Realisim::Widget3d* mpWidget3d_1;
	Realisim::Widget3d* mpWidget3d_2;

	QListWidget* mpAnimationNavigator;
};

#endif //RealEdit_AnimationUi_hh
