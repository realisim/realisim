
#ifndef RealEdit_MainWindow_hh
#define RealEdit_MainWindow_hh

namespace RealEdit{ class MainWindow;
                    class EditionUi; }

class QMainWindow;

class RealEdit::MainWindow
{
public:
	MainWindow();
	~MainWindow();

protected:
private:

	QMainWindow* mpMainWindow;
	EditionUi* mpEditionUi;
};

#endif //RealEdit_MainWindow_hh
