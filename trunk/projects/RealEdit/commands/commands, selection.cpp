/*
 *  commands, selection.cpp
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */

#include "commands, selection.h"
#include "EditionData.h"

using namespace realEdit;
  using namespace commands;
using namespace std;
  
Selection::Selection(EditionData& iEd, const unsigned int iS, mode iMode) :
  Command(),
  mEditionData(iEd),
  mPreviousSelection(mEditionData.getSelection()),
  mSelection()
{
  mSelection.push_back(make_pair(iS, iMode));
}

Selection::~Selection()
{}

//------------------------------------------------------------------------------
void Selection::execute()
{
  set<uint>& selection = mEditionData.getSelection();
  
  vector<pair<unsigned int, mode> >::const_iterator it = mSelection.begin();
  for(; it != mSelection.end(); ++it)
  {
    mode m = it->second;
    unsigned int id = it->first;
    if(m == mNormal)
      selection.clear(); 
      
    if(m == mAdditive || m == mNormal)
      selection.insert(id);
    if(m == mSubtractive)
    {
      set<unsigned int>::const_iterator itToRemove = selection.find(id);
      if(itToRemove != selection.end())
        selection.erase(itToRemove);
    }
  }
  
  mEditionData.select(selection);
}

//------------------------------------------------------------------------------
void Selection::undo()
{mEditionData.select(mPreviousSelection);}

//------------------------------------------------------------------------------
void Selection::update(const unsigned int iS, mode iMode)
{
  mSelection.push_back(make_pair(iS, iMode));
  unique(mSelection.begin(), mSelection.end());
}
