#include "Commands/changeTool.h"
#include "Commands/extrude.h"
#include "Controller.h"
#include "Palettes/Tools.h"
#include <QtGui>

using namespace realEdit;
  using namespace palette;
  
//------------------------------------------------------------------------------
// Palette
//------------------------------------------------------------------------------
Palette::Palette(QWidget* ipParent /* = 0*/) : QWidget(ipParent),
  mpController(0)
{}

Palette::~Palette()
{}

//------------------------------------------------------------------------------
void Palette::setController(Controller& iC)
{ mpController = &iC; }
  
//------------------------------------------------------------------------------
// Tools
//------------------------------------------------------------------------------
Tools::Tools(QWidget* ipParent /* = 0*/) : Palette(ipParent)
{
  setWindowFlags(windowFlags() | Qt::Tool);
  
  QHBoxLayout* pLyt = new QHBoxLayout(this);
  pLyt->setMargin(1);
  mpButtonGroup = new QButtonGroup(this);
  mpButtonGroup->setExclusive(true);
  
  QPushButton* pSelect = new QPushButton("s", this);
  pSelect->setCheckable(true);
  QPushButton* pTranslate = new QPushButton("t", this);
  pTranslate->setCheckable(true);
  QPushButton* pExtrude = new QPushButton("e", this);
  pExtrude->setCheckable(false);
  
  mpButtonGroup->addButton(pSelect, idSelect);
  mpButtonGroup->addButton(pTranslate, idTranslate);
  mpButtonGroup->addButton(pExtrude, idExtrude);
  pLyt->addWidget(pSelect);
  pLyt->addWidget(pTranslate);
  pLyt->addWidget(pExtrude);
  pLyt->addStretch(1);

  connect(mpButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(toolSelected(int)));
}

Tools::~Tools()
{}

//------------------------------------------------------------------------------
void Tools::toolSelected(int iId)
{
  using namespace commands;
  if(!mpController->canChangeTool())
  	return;
  
  switch ((toolId)iId) 
  {
    case idSelect:
      {
        ChangeTool* c = new ChangeTool(*mpController, Controller::tSelect);
        c->execute();
        mpController->addCommand(c);
      }      
      break;
    case idTranslate:
      {
        ChangeTool* c = new ChangeTool(*mpController, Controller::tTranslate);
        c->execute();
        mpController->addCommand(c);
      }
      break;
    case idExtrude:
      {
        Extrude* c = new Extrude(*mpController);
        c->execute();
        mpController->addCommand(c);
      }
      break;
    default:break;
  }
}

//------------------------------------------------------------------------------
void Tools::updateUi()
{
  //Cette palette est uniquement pour les outils D'édition
  if(!mpController || mpController->getMode() != Controller::mEdition)
    return;
    
  mpButtonGroup->blockSignals(true);
  switch (mpController->getTool())
  {
    case Controller::tSelect:
      {
        QPushButton* p = (QPushButton*)mpButtonGroup->button(idSelect);
        p->setChecked(true);
        break;
      }
    case Controller::tTranslate:
    {
        QPushButton* p = (QPushButton*)mpButtonGroup->button(idTranslate);
        p->setChecked(true);
        break;
    }
    case Controller::tExtrude: break;
    default: break;
  }
  mpButtonGroup->blockSignals(false);
}
