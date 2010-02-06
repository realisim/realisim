/*
 *  commands, translate.cpp
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */

#include "commands/translate.h"
#include "DataModel/EditionData.h"
#include "math/MathUtils.h"

using namespace realEdit;
  using namespace commands;
using namespace std;
  
Translate::Translate(EditionData& iEd, const Vector3d& iDelta,
  Controller::mode iMode) : Command(),
  mEditionData(iEd),
  mDelta(iDelta),
  mTotalDelta(iDelta),
  mMode(iMode)
{
}

Translate::~Translate()
{}

//------------------------------------------------------------------------------
void Translate::execute()
{
  if(mEditionData.hasSelection())
  {
    if(mMode == Controller::mEdition)
    {
      for(uint i = 0; i < mEditionData.getSelectedPoints().size(); ++i)
      {
        RealEditPoint p = mEditionData.getSelectedPoints()[i];
        p.set(p.pos() + mDelta);      
      }
      mEditionData.getCurrentModel().updateNormals();
      mEditionData.getCurrentModel().updateBoundingBox();
    }
    else
      mEditionData.getCurrentNode()->translate(mDelta);
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
}
