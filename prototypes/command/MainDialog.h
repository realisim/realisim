/*
 *  MainWindow.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 08/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MainDialog_hh
#define MainDialog_hh

#include "CommandStack.h"
#include "Command.h"
#include <QDialog>
class QLineEdit;

class CommandAdd : public realisim::utils::Command
{
  public:
    CommandAdd(double iO, QLineEdit* ipLe) : mOperand(iO), mpLe(ipLe){;}
    
    virtual void execute();
    virtual void undo();
    
    double mOperand;
    QLineEdit* mpLe;
};

class CommandSubstract : public realisim::utils::Command
{
  public:
    CommandSubstract(double iO, QLineEdit* ipLe) : mOperand(iO), mpLe(ipLe){;}
    virtual void execute();
    virtual void undo();
    
    double mOperand;
    QLineEdit* mpLe;
};

class CommandMultiply : public realisim::utils::Command
{
  public:
    CommandMultiply(double iO, QLineEdit* ipLe) : mOperand(iO), mpLe(ipLe){;}
    virtual void execute();
    virtual void undo();
    
    double mOperand;
    QLineEdit* mpLe;
};

class CommandDivide : public realisim::utils::Command
{
  public:
    CommandDivide(double iO, QLineEdit* ipLe) : mOperand(iO), mpLe(ipLe){;}
    virtual void execute();
    virtual void undo();
    
    double mOperand;
    QLineEdit* mpLe;
};

class MainDialog : public QDialog
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog(){};
  
public slots:
  void onAdd();
  void onSub();
  void onMult();
  void onDiv();
  void onUndo();
  void onRedo();
                
protected:
  realisim::utils::CommandStack mCommandStack;
  QLineEdit* mpResult;
};

#endif
