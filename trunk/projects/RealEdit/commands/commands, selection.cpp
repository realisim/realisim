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
  vector<RealEditPoint>& points = mEditionData.getSelectedPoints();
  vector<RealEditPolygon>& polygons = mEditionData.getSelectedPolygons();
  
  points.clear();
  polygons.clear();
  
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
  
  {
    /*a partir de la selection, on crée la liste de tous les points (unique)
    selectionnés. Les commandes (translate, rotate, scale etc...) s'effectueront
    sur ces points.*/
    set<unsigned int> uniquePointIds;
    set<unsigned int>::const_iterator it = selection.begin();
    for(; it != selection.end(); ++it)
    {
      if(mEditionData.getCurrentModel().hasPoint(*it))
        uniquePointIds.insert(*it);
      else if(mEditionData.getCurrentModel().hasPolygon(*it))
      {
        const RealEditPolygon& p =
          mEditionData.getCurrentModel().getPolygonFromId(*it);
        polygons.push_back(p);
        for(unsigned int j = 0; j < p.getPointCount(); ++j )
          uniquePointIds.insert(p.getPoint(j).getId());
      }
    }
    
    it = uniquePointIds.begin();
    for(; it != uniquePointIds.end(); ++it)
      points.push_back(
        mEditionData.getCurrentModel().getPointFromId(*it));
  }
}

//------------------------------------------------------------------------------
void Selection::undo()
{mEditionData.getSelection() = mPreviousSelection;}

//------------------------------------------------------------------------------
void Selection::update(const unsigned int iS, mode iMode)
{
  mSelection.push_back(make_pair(iS, iMode));
  unique(mSelection.begin(), mSelection.end());
}
