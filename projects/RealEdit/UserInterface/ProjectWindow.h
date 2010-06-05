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
class QFocusEvent;
namespace realEdit { class RealEdit3d; }
#include <vector>
 
namespace realEdit
{

class ProjectWindow : public QWidget
{
public:
  ProjectWindow(QWidget* = 0);
  ~ProjectWindow();
  
  void changeCurrentNode();
  Controller& getController() {return mController;}
  void updateUi();

private:
  Controller mController;
  std::vector<RealEdit3d*> mViews;
};

}

#endif

