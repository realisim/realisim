/*
 *  EditionData.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 19/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "EditionData.h"

#include <vector>
#include <map>

using namespace Realisim;
using namespace RealEdit;

//------------------------EditionData-------------------------------------------
EditionData::EditionData() : mScene(),
                             mpCurrentModel( 0 )
{
}

EditionData::~EditionData()
{
}

void
EditionData::addPoint( const Point3f& iPoint )
{
  if ( mpCurrentModel )
  {
    RealEditPoint* pPoint = new RealEditPoint( iPoint );
    mPoints.insert(
      std::make_pair<RealEditPointId, RealEditPoint*>( pPoint->getId(), pPoint ) );
    
    mpCurrentModel->addPoint( pPoint );
  }
}

void
EditionData::addPolygon( const std::vector<RealEditPointId>& iPointsId )
{
  if ( !mpCurrentModel )
  {
    return; 
  }
  
  std::vector<RealEditPoint*> points;
  for( unsigned int i = 0; i < iPointsId.size(); ++i )
  {
    PointsIt it = mPoints.find( iPointsId[i] );
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