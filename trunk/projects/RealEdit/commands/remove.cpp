#include "commands/remove.h"
#include "Controller.h"
#include "UserInterface/MainWindow.h"

using namespace realEdit;
  using namespace commands;
using namespace std;
  
Remove::Remove(Controller& iC) :
  Command(),
  mController(iC),
  mSelection(),
  mPoints(),
  mPolygons(),
  mSegments()
{
}

Remove::~Remove()
{}

//------------------------------------------------------------------------------
void Remove::execute()
{  
 	EditionData& e = mController.getEditionData();
  //pas de selection on sort.
  if(!e.hasSelection())
  	return;

  mSelection = e.getSelection();
  
  for(unsigned int i = 0; i < e.getSelectedPolygons().size(); ++i)
  	removePolygon(e.getSelectedPolygons()[i]);
    
  for(unsigned int i = 0; i < e.getSelectedSegments().size(); ++i)
  	removeSegment(e.getSelectedSegments()[i]);

  for(unsigned int i = 0; i < e.getSelectedPoints().size(); ++i)
  	removePoint(e.getSelectedPoints()[i]);
      
  e.select(set<unsigned int>());
  mController.getProjectWindow().updateUi();
  
//debug
//  cout << "\nPolygones: " << m.getPolygons().size() <<endl;
//  cout << "Segments: " << m.getSegments().size() <<endl;
//  cout << "Points: " << m.getPoints().size() <<endl;
//  cout << "Polygones effacés: " << mPolygons.size() <<endl;
//  cout << "Segments effacés: " << mSegments.size() <<endl;
//  cout << "Points effacé: " << mPoints.size() <<endl;
}

//------------------------------------------------------------------------------
void Remove::redo()
{
	EditionData& e = mController.getEditionData();
  RealEditModel& m = e.getCurrentModel();
  
  for(unsigned int i = 0; i < mPoints.size(); ++i)
  	m.removePoint(mPoints[i].getId());
    
  for(unsigned int i = 0; i < mSegments.size(); ++i)
  	m.removeSegment(mSegments[i].getId());
    
  for(unsigned int i = 0; i < mPolygons.size(); ++i)
  	m.removePolygon(mPolygons[i].getId());
    
  e.select(set<unsigned int>());
  mController.getProjectWindow().updateUi();
}

//------------------------------------------------------------------------------
void Remove::removePoint(const RealEditPoint& iP)
{
  RealEditModel& m = mController.getEditionData().getCurrentModel();
  RealEditPoint pt = iP;
  
  /*Quand on enlève un point, on doit aussi enlever tous les segments
    et les polygones qui ont ce point. Le remove est un peu tricky...
    voici ce qui se passe:
    On enleve les polygones qui contienne ce point en passant
    par la methode removePolygon qui va s'occuper corretement
    d'enlever le polygone ainsi que les segments et points
    qu'il contient.*/
  vector<RealEditPolygon> p = m.getPolygonsContainingPoint(pt.getId());
  for(unsigned int i = 0; i < p.size(); ++i)
  	removePolygon(p[i]);
  
  /*On enleve les segments qui contiennent ce point par la méthode
  removeSegment qui s'occupera d'enlever correctement, les 
  polygones et points qui touchent à ce segment.*/
  vector<RealEditSegment> s = m.getSegmentsContainingPoint(pt.getId());
  for(unsigned int i = 0; i < s.size(); ++i)
    removeSegment(s[i]);
    
  /*Si le point n'a pas déja été enlevé par les méthodes précédentes, on
    l'enlève*/
  if(m.hasPoint(pt.getId()))
  {
    mPoints.push_back(pt);
    m.removePoint(pt.getId());
  }
}

//------------------------------------------------------------------------------
void Remove::removePolygon(const RealEditPolygon& iP)
{
  RealEditModel& m = mController.getEditionData().getCurrentModel();
  RealEditPolygon p = iP;
  
 /*Quand on enlève le polygon, on enleve aussi les segments qui
   n'appartiennent a aucun autre polygones et ensuite on enleve
   les point qui n'appartiennent ni à un autre polygone ni à
   un autre segment.*/

  /*On enleve d'abord le polygone parce qu'ensuite on veut 
    enlever les segments qui n'appartiennent à aucun polygones.
    Donc, si on enlevait pas le polygone en premier, les segments
    du polygones appartiendraient au minimum à un polygone et
    ne pourraient pas être enlevés. Même chose pour les points*/
    
  /*On ne passe pas par les méthodes removeSegment/removePoint
    parce qu'on sait que ces primitives sont orphelines. Donc
    on les enlève directement des map internes.*/
  m.removePolygon(iP.getId());
  mPolygons.push_back(iP);
  for(unsigned int i = 0; i < p.getSegments().size(); ++i)
  {
  	RealEditSegment s = p.getSegment(i);
    if(m.getPolygonsContainingSegment(s.getId()).empty())
    {
      mSegments.push_back(s);
    	m.removeSegment(s.getId());
    }
  }
  
  for(unsigned int i = 0; i < p.getPoints().size(); ++i)
  {
  	RealEditPoint pt = p.getPoint(i);
    if(m.getPolygonsContainingPoint(pt.getId()).empty() &&
       m.getSegmentsContainingPoint(pt.getId()).empty() )
    {
      mPoints.push_back(pt);
      m.removePoint(pt.getId());
    }
  }
}

//------------------------------------------------------------------------------
void Remove::removeSegment(const RealEditSegment& iS)
{
  RealEditModel& m = mController.getEditionData().getCurrentModel();
  RealEditSegment s = iS;
  /*On enleve aussi les polygones qui touche a ce segment et
    on enleve les points qui n'appartiennent ni à un autre polygone ni à
   un autre segment.*/
  
  /*On commence par enlever les polygones qui contiennent ce segment.
    On passe par la méthode removePolygone afin d'enlever correctement
    ce polygone (il n'est probablement pas orphelin, il faut donc 
    gérer ses points/segments).*/
  vector<RealEditPolygon> p = m.getPolygonsContainingSegment(s.getId());
  for(unsigned int i = 0; i < p.size(); ++i)
    removePolygon(p[i]);
   
  /*Ensuite on enlève le segment de la map interne s'il sont encore
    présent*/
  if(m.hasSegment(s.getId()))
  {
    mSegments.push_back(s);
  	m.removeSegment(s.getId());
  }
   
  /*On finit par enlever les points du segment si aucun polygones/segments
    n'y font référence. Dans ce cas, on sait que ses points sont 
    orphelins et on les enlève directement de la map.*/
  if(m.getPolygonsContainingPoint(s.getPoint1().getId()).empty() &&
    m. getSegmentsContainingPoint(s.getPoint1().getId()).empty())
  {
  	mPoints.push_back(s.getPoint1());
    m.removeSegment(s.getPoint1().getId());
  }
  if(m.getPolygonsContainingPoint(s.getPoint2().getId()).empty() &&
    m.getSegmentsContainingPoint(s.getPoint2().getId()).empty())
  {
    mPoints.push_back(s.getPoint2());
    m.removeSegment(s.getPoint2().getId());
  }
}

//------------------------------------------------------------------------------
void Remove::undo()
{
	EditionData& e = mController.getEditionData();
  RealEditModel& m = e.getCurrentModel();
  
  /*L'ordre est important! voir note dans DataModel::RealEditModel.
    c'Est au sujet de la duplication des segments.*/
  for(unsigned int i = 0; i < mPoints.size(); ++i)
  	m.addPoint(mPoints[i]);
    
  for(unsigned int i = 0; i < mSegments.size(); ++i)
  	m.addSegment(mSegments[i]);
    
  for(unsigned int i = 0; i < mPolygons.size(); ++i)
  	m.addPolygon(mPolygons[i]);
    
  e.select(mSelection);
  mController.getProjectWindow().updateUi();
}

