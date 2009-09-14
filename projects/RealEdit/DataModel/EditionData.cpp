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
#include <set>


using namespace realisim;
using namespace realEdit;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    Scene
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Scene::Scene() : mNodes( "Root" )
{
}

//-----------------------------------------------------------------------------
Scene::~Scene()
{
}

//-----------------------------------------------------------------------------
const ObjectNode* Scene::getObjectNode() const
{
  return &mNodes;
}

//-----------------------------------------------------------------------------
ObjectNode* Scene::getObjectNode()
{
  return const_cast<ObjectNode*>(
    static_cast<const Scene&> (*this).getObjectNode() );
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    EditionData
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
EditionData::EditionData() : mCurrentModel (),
  mpCurrentNode (0),
  mScene (),
  mSelection(),
  mSelectedPoints(),
  mSelectedPolygons()
{
  setCurrentNode (getScene (). getObjectNode ());
}

//-----------------------------------------------------------------------------
EditionData::~EditionData()
{}

//-----------------------------------------------------------------------------
ObjectNode* EditionData::addNode (const QString iName)
{ return mpCurrentNode->addNode (iName); }

//-----------------------------------------------------------------------------
//void EditionData::addNode (ObjectNode* ipNode)
//{ mpCurrentNode->addNode (ipNode); }

//-----------------------------------------------------------------------------
RealEditPoint EditionData::addPoint (const Point3d& iPoint)
{
  RealEditPoint point (iPoint);
  getCurrentModel().addPoint (point);
  return point;
}

//-----------------------------------------------------------------------------
RealEditPolygon EditionData::addPolygon (const std::vector<RealEditPoint>& iPoints)
{
  RealEditPolygon poly( iPoints );
  getCurrentModel().addPolygon(poly);
  return poly;
}

//-----------------------------------------------------------------------------
const ObjectNode* EditionData::getCurrentNode () const
{
  return mpCurrentNode;
}

//-----------------------------------------------------------------------------
ObjectNode* EditionData::getCurrentNode ()
{
  return const_cast<ObjectNode*>(
    static_cast<const EditionData&> (*this).getCurrentNode() );
}

//-----------------------------------------------------------------------------
const Scene& EditionData::getScene () const
{ 
  return mScene;
}

//-----------------------------------------------------------------------------
Scene& EditionData::getScene ()
{
  return const_cast<Scene&>(
   static_cast<const EditionData&>(*this).getScene() );
}

//-----------------------------------------------------------------------------
bool EditionData::isSelected(uint iId) const
{
  return find(mSelection.begin(), mSelection.end(), iId) != mSelection.end();  
}

//-----------------------------------------------------------------------------
void EditionData::select(const vector<uint>& iS)
{
  mSelectedPoints.clear();
  mSelectedPolygons.clear();
  mSelection.clear();
  mSelection = iS;
  
  /*a partir de la selection, on crée la liste de tous les points (unique)
  selectionnés. Les commandes (translate, rotate, scale etc...) s'effectueront
  sur ces points.*/
  
  set<unsigned int> uniquePointIds;
  for(unsigned int i = 0; i < mSelection.size(); ++i)
  {
    if(getCurrentModel().hasPoint(mSelection[i]))
      uniquePointIds.insert(mSelection[i]);
    else if(getCurrentModel().hasPolygon(mSelection[i]))
    {
      const RealEditPolygon& p =
        getCurrentModel().getPolygonFromId(mSelection[i]);
      mSelectedPolygons.push_back(p);
      for(unsigned int j = 0; j < p.getPointCount(); ++j )
        uniquePointIds.insert(p.getPoint(j).getId());
    }
  }
  
  set<unsigned int>::const_iterator it = uniquePointIds.begin();
  for(it; it != uniquePointIds.end(); ++it)
  {
    mSelectedPoints.push_back(
      getCurrentModel().getPointFromId(*it));
  }
}

//-----------------------------------------------------------------------------
void EditionData::setCurrentNode (const ObjectNode* ipNode)
{
  mpCurrentNode = const_cast<ObjectNode*> (ipNode);
  mCurrentModel = mpCurrentNode->getModel ();
}