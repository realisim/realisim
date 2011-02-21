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

#include "DataModel/ObjectNode.h"
#include "math/Point.h"
#include <set>

namespace realEdit
{
using namespace realisim;
using namespace std;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    EditionData
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class EditionData
{
public:
  EditionData ();
  ~EditionData ();
  
//bool fromString(const QString&);
  const RealEditModel getCurrentModel() const {return mCurrentModel;}
  RealEditModel getCurrentModel(){return mCurrentModel;}
  const ObjectNode* getCurrentNode () const;
  ObjectNode* getCurrentNode ();
  const ObjectNode* getNode(unsigned int) const;
  ObjectNode* getNode(unsigned int);
  vector<RealEditPoint>& getSelectedPoints() {return mSelectedPoints;}
  vector<RealEditPolygon>& getSelectedPolygons() {return mSelectedPolygons;}
  vector<RealEditSegment>& getSelectedSegments() {return mSelectedSegments;}
  const set<uint>& getSelection() const {return mSelection;}
  set<uint>& getSelection() {return mSelection;}
  const ObjectNode* getRootNode() const;
  ObjectNode* getRootNode();
  bool hasSelection() const {return mSelection.size() > 0;}
  bool isSelected(uint) const;
  void select(const set<uint>& iS);
  void setCurrentNode (const ObjectNode* ipNode);
//QString toString() const;
  
private:
  const ObjectNode* getNode(const ObjectNode*, unsigned int) const;

  static ObjectNode mDummyNode;
  ObjectNode mNodes;
  RealEditModel mCurrentModel;
  ObjectNode* mpCurrentNode; //Ne sera jamais Null
  set<uint> mSelection;
  vector<RealEditPoint> mSelectedPoints;
  vector<RealEditPolygon> mSelectedPolygons;
  vector<RealEditSegment> mSelectedSegments;
};

} // realEdit

#endif
