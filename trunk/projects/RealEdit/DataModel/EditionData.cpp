/*
 *  EditionData.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 19/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <algorithm>
#include "DataModel.h"
#include "EditionData.h"
#include <vector>


using namespace realisim;
using namespace realEdit;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    EditionData
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ObjectNode EditionData::mDummyNode = ObjectNode("dummyNode");

EditionData::EditionData() :
  mNodes("Root"),
  mCurrentModel(),
  mpCurrentNode(0),
  mSelection(),
  mSelectedPoints(),
  mSelectedPolygons(),
  mSelectedSegments()
{
  setCurrentNode(getRootNode());
}

//-----------------------------------------------------------------------------
EditionData::~EditionData()
{}

//-----------------------------------------------------------------------------
const ObjectNode* EditionData::getCurrentNode () const
{ return mpCurrentNode; }

//-----------------------------------------------------------------------------
ObjectNode* EditionData::getCurrentNode ()
{ return mpCurrentNode; }

//-----------------------------------------------------------------------------
const ObjectNode* EditionData::getNode(unsigned int iId) const
{ return getNode(&mNodes, iId); }

//-----------------------------------------------------------------------------
/*Méthode pour piggy-backé la version const de la même méthode au lieu de 
  dupliquer le code.*/
ObjectNode* EditionData::getNode(unsigned int iId)
{
  return const_cast<ObjectNode*>(
    const_cast<const EditionData*>(this)->getNode(iId));
}

//-----------------------------------------------------------------------------
const ObjectNode* EditionData::getNode(const ObjectNode* ipNode,
  unsigned int iId) const
{
  if(ipNode->getId() == iId)
    return ipNode;
  else
  {
    const ObjectNode* foundInChild = 0;
    for(unsigned int i = 0; i < ipNode->getChildCount(); ++i)
    {
      foundInChild = getNode(ipNode->getChild(i), iId);
      if(foundInChild != &mDummyNode) return foundInChild;
    }
		return &mDummyNode;
  }
}

//-----------------------------------------------------------------------------
const ObjectNode* EditionData::getRootNode() const
{ return &mNodes; }

//-----------------------------------------------------------------------------
ObjectNode* EditionData::getRootNode()
{ return &mNodes; }

//-----------------------------------------------------------------------------
bool EditionData::isSelected(uint iId) const
{ return mSelection.find(iId) != mSelection.end();   }

//-----------------------------------------------------------------------------
void EditionData::setCurrentNode (const ObjectNode* ipNode)
{
  mpCurrentNode = const_cast<ObjectNode*> (ipNode);
  mCurrentModel = mpCurrentNode->getModel ();
}

//-----------------------------------------------------------------------------
void EditionData::select(const set<uint>& iS)
{
  mSelection = iS;
  mSelectedPoints.clear();
  mSelectedPolygons.clear();
  mSelectedSegments.clear();
  
  /*a partir de la selection, on crée la liste de tous les points (unique)
  selectionnés. Les commandes (translate, rotate, scale etc...) s'effectueront
  sur ces points.*/
  set<unsigned int> uniquePointIds;
  set<unsigned int>::const_iterator it = iS.begin();
  for(; it != iS.end(); ++it)
  {
    if(getCurrentModel().hasPoint(*it))
      uniquePointIds.insert(*it);
    else if(getCurrentModel().hasPolygon(*it))
    {
      const RealEditPolygon& p =
        getCurrentModel().getPolygon(*it);
      mSelectedPolygons.push_back(p);
      for(unsigned int j = 0; j < p.getPointCount(); ++j )
        uniquePointIds.insert(p.getPoint(j).getId());
    }
    else if(getCurrentModel().hasSegment(*it))
    {
    	const RealEditSegment& s =
        getCurrentModel().getSegment(*it);
      mSelectedSegments.push_back(s);
      uniquePointIds.insert(s.getPoint1().getId());
      uniquePointIds.insert(s.getPoint2().getId());
    }
  }
  
  it = uniquePointIds.begin();
  for(; it != uniquePointIds.end(); ++it)
    mSelectedPoints.push_back(
      getCurrentModel().getPoint(*it));
}