/*
 *  MainWindow.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 08/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "MainDialog.h"
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>

void CommandAdd::execute()
{
  bool ok;
  double result = mpLe->text().toDouble(&ok);
  result += mOperand;
  mpLe->setText(QString::number(result));
}
void CommandAdd::undo()
{
  bool ok;
  double result = mpLe->text().toDouble(&ok);
  result -= mOperand;
  mpLe->setText(QString::number(result));
}

void CommandSubstract::execute()
{
  bool ok;
  double result = mpLe->text().toDouble(&ok);
  result -= mOperand;
  mpLe->setText(QString::number(result));
}
void CommandSubstract::undo()
{
  bool ok;
  double result = mpLe->text().toDouble(&ok);
  result += mOperand;
  mpLe->setText(QString::number(result));
}

void CommandMultiply::execute()
{
  bool ok;
  double result = mpLe->text().toDouble(&ok);
  result *= mOperand;
  mpLe->setText(QString::number(result));
}
void CommandMultiply::undo()
{
  bool ok;
  double result = mpLe->text().toDouble(&ok);
  result /= mOperand;
  mpLe->setText(QString::number(result));
}

void CommandDivide::execute()
{
  bool ok;
  double result = mpLe->text().toDouble(&ok);
  result /= mOperand;
  mpLe->setText(QString::number(result));
}
void CommandDivide::undo()
{
  bool ok;
  double result = mpLe->text().toDouble(&ok);
  result *= mOperand;
  mpLe->setText(QString::number(result));
}

MainDialog::MainDialog() 
: QDialog(),
mpResult(0)
{
  QVBoxLayout* pVlyt = new QVBoxLayout(this);
  
  QHBoxLayout* pButtonLyt = new QHBoxLayout();
  {
    QPushButton* pAdd = new QPushButton("Add 5", this);
    QPushButton* pSub = new QPushButton("Sub 3", this);
    QPushButton* pMult = new QPushButton("Mult 2", this);
    QPushButton* pDiv = new QPushButton("Div 6", this);
    
    pButtonLyt->addWidget(pAdd);
    pButtonLyt->addWidget(pSub);
    pButtonLyt->addWidget(pMult);
    pButtonLyt->addWidget(pDiv);
    pButtonLyt->addStretch(1);
    
    connect(pAdd, SIGNAL(clicked()), this, SLOT(onAdd()));
    connect(pSub, SIGNAL(clicked()), this, SLOT(onSub()));
    connect(pMult, SIGNAL(clicked()), this, SLOT(onMult()));
    connect(pDiv, SIGNAL(clicked()), this, SLOT(onDiv()));
  }
  
  mpResult = new QLineEdit(this);
  mpResult->setReadOnly(true);
  mpResult->setText(QString::number(0.0));
  
  QHBoxLayout* pUndoRedoLyt = new QHBoxLayout();
  {
    QPushButton* pUndo = new QPushButton("Undo", this);
    QPushButton* predo = new QPushButton("Redo", this);

    pUndoRedoLyt->addStretch(1);    
    pUndoRedoLyt->addWidget(pUndo);
    pUndoRedoLyt->addWidget(predo);
    
    connect(pUndo, SIGNAL(clicked()), this, SLOT(onUndo()));
    connect(predo, SIGNAL(clicked()), this, SLOT(onRedo()));
  }
  
  pVlyt->addLayout(pButtonLyt, 1);
  pVlyt->addWidget(mpResult);
  pVlyt->addLayout(pUndoRedoLyt);
	//showFullScreen();
}

void MainDialog::onAdd()
{
  CommandAdd* a = new CommandAdd(5, mpResult);
  mCommandStack.add(a);
}

void MainDialog::onSub()
{
  CommandSubstract* a = new CommandSubstract(3, mpResult);
  mCommandStack.add(a);
}

void MainDialog::onMult()
{
  CommandMultiply* a = new CommandMultiply(2, mpResult);
  mCommandStack.add(a);
}

void MainDialog::onDiv()
{
  CommandDivide* a = new CommandDivide(6, mpResult);
  mCommandStack.add(a);

}

void MainDialog::onUndo()
{ mCommandStack.undo();}
void MainDialog::onRedo()
{ mCommandStack.redo();}