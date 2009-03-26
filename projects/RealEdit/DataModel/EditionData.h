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

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    Scene
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class RealEdit::Scene
{
public:
  Scene();
  ~Scene();
    
  const ObjectNode* getObjectNode() const; //ne retourne jamais de pointeur Null
  ObjectNode* getObjectNode(); //ne retourne jamais de pointeur Null
  
private:
  ObjectNode mNodes;
  //std::vector<Lights> mLights;
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    EditionData
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class RealEdit::EditionData
{
public:
  EditionData();
  ~EditionData();
  
  void addPoint( const Realisim::Point3f& iPoint );
  void addPolygon( const std::vector<int>& iPoints );
  ObjectNode* addNode( const std::string iName );
  const ObjectNode* getCurrentNode() const;
  ObjectNode* getCurrentNode();
  const Scene& getScene() const;
  Scene& getScene();
  void setCurrentNode( ObjectNode* ipNode );
  
private:
  Scene mScene;

  //mpCurrentModel n'est pas accessible a l'extérieur de EditionData
  //il est seulement utilisé a l'interne
  RealEditModel* mpCurrentModel; //Ne sera jamais Null
  ObjectNode* mpCurrentNode; //Ne sera jamais Null

  std::vector<RealEditPoint*> mSelectedPoints;
  std::vector<RealEditPolygon*> mSelectedPolygons;
  
  //une map de id de point
  typedef std::map<unsigned int, RealEditPoint*> PointMap;
  typedef PointMap::iterator PointMapIt;
  PointMap mPoints; //Propriétaire du pointeur
  
  //une map de id de polygon
  typedef std::map<unsigned int, RealEditPolygon*> PolygonMap;
  typedef PolygonMap::iterator PolygonMapIt;
  PolygonMap mPolygons;  //Propriétaire du pointeur
};

#endif
