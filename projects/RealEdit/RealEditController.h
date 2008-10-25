

#ifndef RealEdit_RealEditController_hh
#define RealEdit_RealEditController_hh


namespace RealEdit{ class RealEditController;
					class MainWindow; }

class RealEdit::RealEditController
{
public:
	RealEditController();
	~RealEditController();

protected:
private:

	MainWindow* mpMainWindow; //owned
};

#endif //RealEdit_RealEditController_hh