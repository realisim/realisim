/*
 *  commands, Select.cpp
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */

#include "commands/select.h"
#include "Controller.h"
#include "UserInterface/ProjectWindow.h"

using namespace realEdit;
  using namespace commands;
using namespace std;
  
Select::Select(Controller& iC) :
  Command(),
  mC(iC),
  mPreviousSelection(mC.getEditionData().getSelection()),
  mSelection()
{}

Select::~Select()
{}

//------------------------------------------------------------------------------
void Select::execute()
{
	EditionData& e = mC.getEditionData();
  set<uint>& selection = e.getSelection();
  
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

	//Le set garanti que chaque élément est unique  
  e.select(selection);
  mC.getProjectWindow().updateUi();
}

//------------------------------------------------------------------------------
void Select::undo()
{
	EditionData& e = mC.getEditionData();
  e.select(mPreviousSelection);
  mC.getProjectWindow().updateUi();
}

//------------------------------------------------------------------------------
void Select::update(const unsigned int iS, mode iMode)
{
  mSelection.push_back(make_pair(iS, iMode));
  execute();
}

//------------------------------------------------------------------------------
void Select::update(const std::vector<unsigned int>& iS, mode iMode)
{
  for(unsigned int i = 0; i < iS.size(); ++i)
  {
  	if(i != 0 && iMode == mNormal)
      iMode = mAdditive;
    mSelection.push_back(make_pair(iS[i], iMode));
  }
  execute();
}
