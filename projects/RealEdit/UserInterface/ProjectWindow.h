/*
 *  Vierwer.h
 *  Realisim
 *
 *  Created by Pierre-Olivier Beaudoin on 10-05-30.
 */
#ifndef RealEdit_ProjectWindow_hh
#define RealEdit_ProjectWindow_hh

#include "Controller.h"
#include "DataModel/EditionData.h"
#include "QtGui/QWidget"
class QCloseEvent;
class QFocusEvent;
class QKeyEvent;
namespace realEdit { class RealEdit3d; }
#include <vector>
 
namespace realEdit
{

class ProjectWindow : public QWidget
{
  Q_OBJECT
public:
  ProjectWindow(QWidget* = 0);
  virtual ~ProjectWindow();
  
  virtual void changeCurrentNode();
  virtual Controller& getController() {return mController;}
  virtual void updateUi();

signals:
  void aboutToClose(ProjectWindow*);
  
protected:
	virtual void closeEvent(QCloseEvent*);
  virtual void keyPressEvent(QKeyEvent*);

  Controller mController;
  std::vector<RealEdit3d*> mViews;
};

}

#endif

