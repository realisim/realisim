/*
 *  EditionData.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 19/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "ObjectNode.h"

#include <map>

namespace RealEdit
{
  class ObjectNode;
  
  class Scene;
  class EditionData;
}

//------------------------------------------------------------------------------
class RealEdit::Scene
{
public:
  Scene(){;}
  ~Scene(){;}
  
private:
  ObjectNode mNodes;
  //std::vector<Lights> mLights;
};

//------------------------------------------------------------------------------
class RealEdit::EditionData
{
public:
  EditionData();
  ~EditionData();
  
  void addPoint( const Realisim::Point3f& iPoint );
  void addPolygon( const std::vector<RealEditPointId>& iPoints );
  
private:
  Scene mScene;
  
  RealEditModel* mpCurrentModel;
  
  typedef std::map<RealEditPointId, RealEditPoint*>::iterator PointsIt;
  std::map<RealEditPointId, RealEditPoint*> mPoints; //Propriétaire du pointeur
  
  typedef std::map<RealEditPolygonId, RealEditPolygon*>::iterator PolygonsIt;
  std::map<RealEditPolygonId, RealEditPolygon*> mPolygons;  //Propriétaire du pointeur
};
