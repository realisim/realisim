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
  const ObjectNode* getCurrentNode () const;
  ObjectNode* getCurrentNode ();
//const vector<RealEditPoint>& getSelectedPoints() const {return mSelectedPoints;}
//vector<RealEditPoint>& getSelectedPoints() {return mSelectedPoints;}
  const vector<uint>& getSelection() const {return mSelection;}
  const Scene& getScene () const;
  Scene& getScene ();
  void select(vector<uint>);
  void setCurrentNode (const ObjectNode* ipNode);
  
private:
  RealEditModel getCurrentModel(){return mCurrentModel;}

  RealEditModel mCurrentModel;
  ObjectNode* mpCurrentNode; //Ne sera jamais Null
  Scene mScene;
  vector<uint> mSelection;
  vector<RealEditPoint> mSelectedPoints;
  vector<RealEditPolygon> mSelectedPolygons;
};

} // realEdit

#endif
