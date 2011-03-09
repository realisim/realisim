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


using namespace realisim;
using namespace realEdit;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    EditionData
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ObjectNode EditionData::mDummyNode = ObjectNode("dummyNode");

EditionData::EditionData() :
  mNodes("Root"),
  mCurrentModel(),
  mpCurrentNode(0),
  mSelection(),
  mSelectedPoints(),
  mSelectedPolygons(),
  mSelectedSegments()
{
  setCurrentNode(getRootNode());
}

//-----------------------------------------------------------------------------
EditionData::~EditionData()
{}

//-----------------------------------------------------------------------------
/*Retourne tous les points de la sélection i.e: Les points explicitement
  sélectionnés par l'usager mais aussi les points qui font partis des
  segments et polygones selectionnés. Par example si l'usager a sélectionné
  un polygone (aucun point explicitement sélectionné) alors cette méthode
  retourne tous les points constituant ce polygone.
  Note: Il est possible de filtrer avec le paramètre iFilter.
   fAll: indique que les points proviendront des points des segments et
     des polygones sélectionnés.
   fPoints: indique que les points proviendront des points sélectionné (pareil
     comme faire un appel à la méthode getSelectedPoints() )
   fSegments: indique que les points proviendront des segments
     sélectionnés.
   fPolygons: indique que les points proviendront des polygones
     sélectionnés.
     
  Il est possible de combiner ces flags comme ceci:
   filter f = fPoints | fPolygons. */
vector<RealEditPoint> EditionData::getAllPointsFromSelection(
  filter iFilter /*= fAll*/)
{
		vector<RealEditPoint> r;
    /*on fabrique une liste de point unique à partir de la
      sélection afin d'appliquer la translation sur tous
      les points.*/
    set<uint> uniquePoints;
    
    if(iFilter & fPoints)
    {
    	vector<RealEditPoint>& sp = getSelectedPoints();
    	for(uint i = 0; i < sp.size(); ++i)
      	uniquePoints.insert(sp[i].getId());
    }
      
    if(iFilter & fSegments)
    {
      vector<RealEditSegment>& ss = getSelectedSegments();
      for(uint i = 0; i < ss.size(); ++i)
      {
        uniquePoints.insert(ss[i].getPoint1().getId());
        uniquePoints.insert(ss[i].getPoint2().getId());
      }
    }
    
    if(iFilter & fPolygons)
    {
      vector<RealEditPolygon>& sp2 = getSelectedPolygons();
      for(uint i = 0; i < sp2.size(); ++i)
      {
        const RealEditPolygon& p = sp2[i];
        for(uint j = 0; j < p.getPoints().size(); ++j)
          uniquePoints.insert(p.getPoint(j).getId());
      }
    }            
    
    RealEditModel& m = getCurrentModel();
    set<uint>::iterator it = uniquePoints.begin();
    for(; it != uniquePoints.end(); ++it)
    {
    	assert(m.hasPoint(*it));
      r.push_back(m.getPoint(*it));
    }
    return r;
}

//-----------------------------------------------------------------------------
const ObjectNode* EditionData::getCurrentNode () const
{ return mpCurrentNode; }

//-----------------------------------------------------------------------------
ObjectNode* EditionData::getCurrentNode ()
{ return mpCurrentNode; }

//-----------------------------------------------------------------------------
const ObjectNode* EditionData::getNode(unsigned int iId) const
{ return getNode(&mNodes, iId); }

//-----------------------------------------------------------------------------
/*Méthode pour piggy-backé la version const de la même méthode au lieu de 
  dupliquer le code.*/
ObjectNode* EditionData::getNode(unsigned int iId)
{
  return const_cast<ObjectNode*>(
    const_cast<const EditionData*>(this)->getNode(iId));
}

//-----------------------------------------------------------------------------
const ObjectNode* EditionData::getNode(const ObjectNode* ipNode,
  unsigned int iId) const
{
  if(ipNode->getId() == iId)
    return ipNode;
  else
  {
    const ObjectNode* foundInChild = 0;
    for(unsigned int i = 0; i < ipNode->getChildCount(); ++i)
    {
      foundInChild = getNode(ipNode->getChild(i), iId);
      if(foundInChild != &mDummyNode) return foundInChild;
    }
		return &mDummyNode;
  }
}

//-----------------------------------------------------------------------------
const ObjectNode* EditionData::getRootNode() const
{ return &mNodes; }

//-----------------------------------------------------------------------------
ObjectNode* EditionData::getRootNode()
{ return &mNodes; }

//-----------------------------------------------------------------------------
bool EditionData::isSelected(uint iId) const
{ return mSelection.find(iId) != mSelection.end();   }

//-----------------------------------------------------------------------------
void EditionData::setCurrentNode (const ObjectNode* ipNode)
{
  mpCurrentNode = const_cast<ObjectNode*> (ipNode);
  mCurrentModel = mpCurrentNode->getModel ();
}

//-----------------------------------------------------------------------------
void EditionData::select(const set<uint>& iS)
{
	const RealEditModel& m = getCurrentModel();
  mSelection = iS;
  mSelectedPoints.clear();
  mSelectedPolygons.clear();
  mSelectedSegments.clear();
  
  /*a partir de la selection, on crée la liste des points, polygones
   et segments selectionnés*/
  set<unsigned int>::const_iterator it = iS.begin();
  for(; it != iS.end(); ++it)
  {
    if(m.hasPoint(*it))
      mSelectedPoints.push_back(m.getPoint(*it));
    else if(m.hasPolygon(*it))
    {
      const RealEditPolygon& p =
        m.getPolygon(*it);
      mSelectedPolygons.push_back(p);
    }
    else if(m.hasSegment(*it))
    {
    	const RealEditSegment& s =
        m.getSegment(*it);
      mSelectedSegments.push_back(s);
    }
  }
}