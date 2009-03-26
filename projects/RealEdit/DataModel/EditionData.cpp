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
const ObjectNode*
Scene::getObjectNode() const
{
  return &mNodes;
}

//-----------------------------------------------------------------------------
ObjectNode*
Scene::getObjectNode()
{
  return const_cast<ObjectNode*>(
    static_cast<const Scene&> (*this).getObjectNode() );
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    EditionData
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
EditionData::EditionData() : mScene(),
                             mpCurrentModel( 0 ),
                             mpCurrentNode( 0 ),
                             mSelectedPoints(),
                             mSelectedPolygons()
{
  setCurrentNode( getScene().getObjectNode() );
}

//-----------------------------------------------------------------------------
EditionData::~EditionData()
{
}

//-----------------------------------------------------------------------------
ObjectNode*
EditionData::addNode( const std::string iName )
{
  return mpCurrentNode->addNode( iName );
}

//-----------------------------------------------------------------------------
void
EditionData::addPoint( const Point3f& iPoint )
{
  if ( !mpCurrentModel )
  {
    return;
  }
  
  RealEditPoint* pPoint = new RealEditPoint( iPoint );
  
  std::pair<PointMapIt, bool> result =
    mPoints.insert(
      std::make_pair<int, RealEditPoint*>( pPoint->getId(), pPoint ) );
  
  assert( result.second );
  
  mpCurrentModel->addPoint( pPoint );
}

//-----------------------------------------------------------------------------
void
EditionData::addPolygon( const std::vector<int>& iPointsId )
{
  if ( !mpCurrentModel )
  {
    return; 
  }
  
  std::vector<RealEditPoint*> points;
  for( unsigned int i = 0; i < iPointsId.size(); ++i )
  {
    PointMapIt it = mPoints.find( iPointsId[i] );
    if ( it != mPoints.end() )
    {
      points.push_back(it->second);
    }
  }
  
  //On doit absoluement avoir autant de RealEditPoint que
  //de pointId
  assert( points.size() == iPointsId.size() );
  
  RealEditPolygon* pPoly = new RealEditPolygon( points );
  mpCurrentModel->addPolygon(pPoly);
}

//-----------------------------------------------------------------------------
const ObjectNode*
EditionData::getCurrentNode() const
{
  return mpCurrentNode;
}

//-----------------------------------------------------------------------------
ObjectNode* EditionData::getCurrentNode()
{
  return const_cast<ObjectNode*>(
    static_cast<const EditionData&> (*this).getCurrentNode() );
}

//-----------------------------------------------------------------------------
const Scene&
EditionData::getScene() const
{ 
  return mScene;
}

//-----------------------------------------------------------------------------
Scene&
EditionData::getScene()
{
  return const_cast<Scene&>(
   static_cast<const EditionData&>(*this).getScene() );
}

//-----------------------------------------------------------------------------
void
EditionData::setCurrentNode( ObjectNode* ipNode )
{
  mpCurrentNode = ipNode;
  mpCurrentModel = mpCurrentNode->getModel();
}