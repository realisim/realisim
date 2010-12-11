#include "commands/changeTool.h"
#include "UserInterface/MainWindow.h"
#include "UserInterface/Palettes/Tools.h"

using namespace realisim;
using namespace realEdit;
  using namespace commands;
  
ChangeTool::ChangeTool(Controller& iC,
  Controller::tool iTool) :
  Command(),
  mController(iC),
  mPreviousTool(iC.getTool()),
  mTool(iTool)
{}

ChangeTool::~ChangeTool()
{}

//------------------------------------------------------------------------------
void ChangeTool::execute()
{
  mController.setTool(mTool);
  MainWindow& mw = mController.getMainWindow();
  palette::Tools* p = mw.getPalette<palette::Tools>(MainWindow::pEditionTools);
  p->updateUi();
}

//------------------------------------------------------------------------------
void ChangeTool::undo()
{
  mController.setTool(mPreviousTool);
  MainWindow& mw = mController.getMainWindow();
  palette::Tools* p = mw.getPalette<palette::Tools>(MainWindow::pEditionTools);
  p->updateUi();
}

