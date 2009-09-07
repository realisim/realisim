/*
 *  commands, selection.cpp
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */

#include "commands, selection.h"
#include "EditionData.h"

using namespace realEdit;
  using namespace commands;
using namespace std;
  
Selection::Selection(EditionData& iEd, const vector<unsigned int>& iS) :
  mEditionData(iEd),
  mPreviousSelection(mEditionData.getSelection()),
  mSelection(iS)
{}

Selection::~Selection()
{}

//------------------------------------------------------------------------------
void Selection::execute()
{mEditionData.select(mSelection);}

//------------------------------------------------------------------------------
void Selection::undo()
{mEditionData.select(mPreviousSelection);}

