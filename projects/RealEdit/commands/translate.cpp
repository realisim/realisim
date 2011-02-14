/*
 *  commands, translate.cpp
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */

#include "commands/translate.h"
#include "Controller.h"
#include "math/MathUtils.h"
#include "UserInterface/ProjectWindow.h"

using namespace realEdit;
  using namespace commands;
using namespace std;
  
Translate::Translate(Controller& iC) : Command(),
  mC(iC),
  mDelta(0.0),
  mTotalDelta(0.0)
{}

Translate::~Translate()
{}

//------------------------------------------------------------------------------
void Translate::execute()
{
  EditionData& e = mC.getEditionData();
  if(e.hasSelection())
  {
    if(mC.getMode() == Controller::mEdition)
    {
      for(uint i = 0; i < e.getSelectedPoints().size(); ++i)
      {
        RealEditPoint p = e.getSelectedPoints()[i];
        p.set(p.pos() + mDelta);      
      }
      e.getCurrentModel().updateNormals();
      e.getCurrentModel().updateBoundingBox();
    }
    else
      e.getCurrentNode()->translate(mDelta);
      
    mC.getProjectWindow().updateUi();
  }
}

//------------------------------------------------------------------------------
void Translate::undo()
{
  mDelta = mTotalDelta * -1;
  execute();
  mDelta *= -1;
}

//------------------------------------------------------------------------------
void Translate::update(const Vector3d& iDelta)
{
  mDelta = iDelta;
  mTotalDelta += mDelta;
  execute();
}
