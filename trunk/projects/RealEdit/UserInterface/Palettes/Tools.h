#ifndef RealEdit_PaletteTools_hh
#define RealEdit_PaletteTools_hh

namespace realEdit {class Controller;}
class QButtonGroup;
#include "QtGui/QWidget"

namespace realEdit
{
namespace palette 
{

class Palette : public QWidget
{
public:
	Palette(QWidget* = 0);
  virtual ~Palette();
  
  virtual void setController(Controller&);
  virtual void updateUi() {;}
  
protected:
  Controller* mpController;
};

class Tools : public Palette
{
  Q_OBJECT
public:
  Tools(QWidget* = 0);
  virtual ~Tools();

  virtual void updateUi();

protected slots:
	virtual void toolSelected(int);
  
protected:
	enum toolId{idSelect, idTranslate};

  QButtonGroup* mpButtonGroup;
};


}
}

#endif

