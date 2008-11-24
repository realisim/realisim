/*
 *  EditionData.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 19/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef RealEdit_EditionData_hh
#define RealEdit_EditionData_hh

#include "ObjectNode.h"
#include "Point.h"

#include <map>

namespace RealEdit
{
  class EditionData;
  class ObjectNode;
  class RealEditPoint;
  class RealEditPolygon;
  class Scene;

}

//------------------------------------------------------------------------------
class RealEdit::Scene
{
public:
  Scene();
  ~Scene();
  
  const ObjectNode* getObjectNode() const;
  ObjectNode* getObjectNode();
  
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
  
  //on ajoute un polygon en passant un vecteur de id de point
  void addPolygon( const std::vector<int>& iPoints );
  
  const Scene& getScene() const;
  Scene& getScene();
  
  const RealEditModel* getCurrentModel() const;
  
  void setCurrentModel( RealEditModel* ipModel );
  
private:
  Scene mScene;
  
  RealEditModel* mpCurrentModel;
  std::vector<RealEditPoint*> mSelectedPoints;
  std::vector<RealEditPolygon*> mSelectedPolygons;
  
  //une map de id de point
  typedef std::map<int, RealEditPoint*> PointMap;
  typedef PointMap::iterator PointMapIt;
  PointMap mPoints; //Propriétaire du pointeur
  
  //une map de id de polygon
  
  typedef std::map<int, RealEditPolygon*> PolygonMap;
  typedef PolygonMap::iterator PolygonMapIt;
  PolygonMap mPolygons;  //Propriétaire du pointeur
};

#endif
