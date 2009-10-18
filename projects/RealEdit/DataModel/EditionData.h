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
#include <set>

namespace realEdit
{
using namespace realisim;
using namespace std;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    Scene
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class Scene
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
class EditionData
{
public:
  EditionData ();
  ~EditionData ();
  
  RealEditPoint addPoint ( const Point3d& iPoint);
  RealEditPolygon addPolygon (const std::vector<RealEditPoint>& iPoints);
  ObjectNode* addNode (const QString iName);
//void addNode (ObjectNode* ipNode);
  bool hasSelection() const {return mSelection.size() > 0;}
  bool isSelected(uint) const;
  RealEditModel getCurrentModel(){return mCurrentModel;}
  const ObjectNode* getCurrentNode () const;
  ObjectNode* getCurrentNode ();
  vector<RealEditPoint>& getSelectedPoints() {return mSelectedPoints;}
  vector<RealEditPolygon>& getSelectedPolygons() {return mSelectedPolygons;}
  const set<uint>& getSelection() const {return mSelection;}
  set<uint>& getSelection() {return mSelection;}
  const Scene& getScene () const;
  Scene& getScene ();
  void setCurrentNode (const ObjectNode* ipNode);
  
private:
  RealEditModel mCurrentModel;
  ObjectNode* mpCurrentNode; //Ne sera jamais Null
  Scene mScene;
  set<uint> mSelection;
  vector<RealEditPoint> mSelectedPoints;
  vector<RealEditPolygon> mSelectedPolygons;
};

} // realEdit

#endif
