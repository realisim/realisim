/*
 *  EditionData.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 19/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "EditionData.h"

#include "DataModel.h"

#include <vector>
#include <map>

using namespace Realisim;
using namespace RealEdit;

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
  mSelectedPoints (),
  mSelectedPolygons ()
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
void EditionData::addNode (ObjectNode* ipNode)
{ mpCurrentNode->addNode (ipNode); }

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
void EditionData::setCurrentNode (const ObjectNode* ipNode)
{
  mpCurrentNode = const_cast<ObjectNode*> (ipNode);
  mCurrentModel = mpCurrentNode->getModel ();
}