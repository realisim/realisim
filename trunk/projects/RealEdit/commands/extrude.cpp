#include "commands/extrude.h"
#include "Controller.h"
#include "math/Point.h"
#include "math/MathUtils.h"
#include "UserInterface/MainWindow.h"

using namespace realEdit;
  using namespace commands;
using namespace std;
  
Extrude::Extrude(Controller& iC) :
  Command(),
  mController(iC),
  mContour(),
  mAddedPoints(),
  mAddedPolygons(),
  mNewSelection(),
  mpRemove(0)
{
}

Extrude::~Extrude()
{
	if(mpRemove)
    delete mpRemove;
}

//------------------------------------------------------------------------------
void Extrude::execute()
{
  EditionData& e = mController.getEditionData();
  RealEditModel& m = e.getCurrentModel();
  vector<RealEditPolygon>& ps = e.getSelectedPolygons();
  
	//pas de selection on sort.
  if(ps.empty())
  	return;
  
  if(!makeOrderedContour())
  	return;
  
  //On trouve la direction moyenne d'extrusion
  Vector3d extrusionDirection = getExtrusionDirection();
  
  /*On ajoute au modele les points qui constitueront la surface 
    extrudée. De plus, on génère la map de correspondance entre
    les points de la surface à extruder avec les points de la
    surface extrudée.*/
  map<unsigned int, RealEditPoint> currentPointsToNewPoints;
  vector<RealEditPoint> currentPoints =
    e.getAllPointsFromSelection(EditionData::fPolygons);
  for(uint i = 0; i < currentPoints.size(); ++i)
  {
    /*on cree un nouveau point a partir de la position du point
    courant plus un offset dans la direction d'extrusion.*/
    RealEditPoint p(currentPoints[i].pos() + extrusionDirection);
    m.addPoint(p);
    currentPointsToNewPoints.insert(make_pair(currentPoints[i].getId(), p));
    mAddedPoints.push_back(p);
  }
  
  /*A partir des polygones initialement sélectionné, on va créer les 
    polygones de la surface extrudé*/
  vector<RealEditPoint> polygonPoints;
  for(uint i = 0; i < ps.size(); ++i)
  {
    RealEditPolygon& p = ps[i]; 
    for(uint j = 0; j < p.getPoints().size(); ++j)
    {
      polygonPoints.push_back(
        currentPointsToNewPoints[p.getPoint(j).getId()]);
    }
  
    RealEditPolygon newPolygon(polygonPoints);
    m.addPolygon(newPolygon);
    mAddedPolygons.push_back(newPolygon);
    mNewSelection.insert(newPolygon.getId());
    polygonPoints.clear();
  }
  
  /*A partir du contour ordonné, on va créer les polygones qui
    forment le contour. */
  vector<RealEditPoint> poly1Points, poly2Points;
  uint startId = mContour[0].getId();
  for(uint i = 0; i < mContour.size() - 1; ++i)
  {    	
    poly1Points.push_back(mContour[i]);
    poly1Points.push_back(mContour[i+1]);
    poly1Points.push_back(currentPointsToNewPoints[mContour[i].getId()]);
    
    poly2Points.push_back(currentPointsToNewPoints[mContour[i].getId()]);
    poly2Points.push_back(mContour[i+1]);
    poly2Points.push_back(currentPointsToNewPoints[mContour[i+1].getId()]);
          
    RealEditPolygon p1(poly1Points);
    RealEditPolygon p2(poly2Points);
    m.addPolygon(p1);
    m.addPolygon(p2);
    mAddedPolygons.push_back(p1);
    mAddedPolygons.push_back(p2);
    poly1Points.clear();
    poly2Points.clear();
    
    /*Ici, si on retrouve le point de départ et que nous n'avons pas encore
      fait le tour de tous les poins, c'est qu'il y a un autre contour
      fermé. Donc on saute le prochain point pour aller à celui
      qui suit (donc 2 plus loin). On incrémente i une fois parce qu'il
      sera aussi incrémenté par la boucle for (donc +2).*/
    if(mContour[i+1].getId() == startId && i+2 < mContour.size())
    {startId = mContour[i+2].getId();  ++i;}
  }
  
  //on store la sélection de polygone précédente
  set<uint> previousSelection;
  for(uint i = 0; i < ps.size(); ++i)
  	previousSelection.insert(ps[i].getId());
    
  //on enleve tous les polygones de la sélection précedente
  e.select(previousSelection);
  mpRemove = new Remove(mController);
  mpRemove->execute();
  
  //on sélectionne les polygones de la surface extrudée
  e.select(mNewSelection);
    
  mController.getProjectWindow().updateUi();
}

//------------------------------------------------------------------------------
/*Place deux points dans la liste du contour ordonné.*/
void Extrude::firstSegment(const RealEditSegment& iS)
{
  if(keepSegmentOrder(iS))
  {
    mContour.push_back(iS.getPoint1());
    mContour.push_back(iS.getPoint2());
  }
  else
  {
    mContour.push_back(iS.getPoint2());
    mContour.push_back(iS.getPoint1());
  }
}

//------------------------------------------------------------------------------
/*Retourne la moyenne normalisé des normales des polygones sélectionnés.*/
Vector3d Extrude::getExtrusionDirection()
{
  EditionData& e = mController.getEditionData();
  vector<RealEditPolygon>& ps = e.getSelectedPolygons();
  
	Vector3d extrusionDirection;
  int count = 0;
  for(uint i = 0; i < ps.size(); ++i)
  {
    RealEditPolygon& p = ps[i];
    for(uint j = 0; j < p.getNormals().size(); ++j)
    {
      extrusionDirection += p.getNormal(j);
      count++;
    }
  }
  extrusionDirection /= count;
  extrusionDirection.normalise();
  
  return extrusionDirection;
}

//------------------------------------------------------------------------------
/*Étant donné que les segments n'ont pas de direction et qu'on veut avoir un
  contour anti horaire afin de facilité la création des polygones qui formeront
  les cotés de la surface extrudée; on doit décider si on garde l'ordre des
  points du segment ou si on les inverse. Pour se faire, on le premier 
  polygone de la sélection qui contient se segment et on compare l'ordre
  des points du polygones (qui est déjà anti horaire) à l'ordre des points
  du segment. S'il concorde, on gardera l'ordre sinon on l'inversera.*/
bool Extrude::keepSegmentOrder(const RealEditSegment& iS)
{
	//on trouve a quel polygon ce segment appartient
  EditionData& e = mController.getEditionData();
  vector<RealEditPolygon>& ps = e.getSelectedPolygons();
  
  for(unsigned int i = 0; i < ps.size(); ++i)
  {
    for(unsigned int j = 0; j < ps[i].getSegments().size(); ++j)
    {
    	const RealEditSegment& s = ps[i].getSegment(j);
      if(s.getId() == iS.getId())
      {
      	for(unsigned int k = 0; k < ps[i].getPoints().size(); ++k)
        {
        	if(ps[i].getPoint(k).getId() == iS.getPoint1().getId())
          {
          	unsigned int nextIndex = k + 1;
          	if(nextIndex >= ps[i].getPoints().size())
            	nextIndex = 0;
            
            if(ps[i].getPoint(nextIndex).getId() != iS.getPoint2().getId())
            {
            	return false;
            }
            else
            	return true;
          }
        }        
      }      
    }
  }
  
  assert(0 && "Le segment n'a pas été trouvé dans la liste \
    des polygones selectionne");
  return true;
}

//------------------------------------------------------------------------------
bool Extrude::makeOrderedContour()
{
  EditionData& e = mController.getEditionData();
  vector<RealEditPolygon>& ps = e.getSelectedPolygons();    

  map<RealEditSegment, int, RealEditSegment::Comparator> segmentsCount;
  map<RealEditSegment, int, RealEditSegment::Comparator>::iterator it;
    
  vector<RealEditSegment> unorderedContour;
  /*On cherche a trouver le/les contours des régions selectionnées
    afin de pouvoir les extruder. Le contour est défini par les segments
    qui n'appartiennent qu'a un seul polygon de la selection.
      
    Pour chaque polygones, pour chacun des ces segments, On les ajoutes
    a la map et a chaque fois qu'on rencontre une occurence d'un
    segment on increment le compte. Ainsi, les segment qui auront
    un compte de 1 seront nécessairement sur le contour.*/
  for(unsigned int i = 0; i < ps.size(); ++i)
  {
  	for(unsigned int j = 0; j < ps[i].getSegments().size(); ++j)
    {
      RealEditSegment s = ps[i].getSegment(j);
      it = segmentsCount.find(s);
    	if(it == segmentsCount.end())
        segmentsCount.insert(make_pair(s, 1));   
      else
        ++(it->second);
    }
  }
  
  /*On met tous les segment qui on un compte de 1 dans la liste du
    contour. Ce contour n'est pas encore ordonné*/
  for(it = segmentsCount.begin(); it != segmentsCount.end(); ++it)
  {
  	if(it->second == 1)
    	unorderedContour.push_back(it->first);
  }
  
  /*On ordonne le contour*/
  /*On prend le premier segment et on trouve a quel polygone il appartient.
    A partir du polygone, on trouve le sens du contour. Ie. le polygone est
    fermé en sens horaire ou antihoraire (c'est ce qui permet de trouver la
    normale du polygone).
    Les point du segments sont toujours placé comme ceci:
      point1 a un id < point2
    Donc, on ne peut pas se fier au segment pour déterminer l'ordre du contour.
    
    Typiquement, tous les polygones de la surface seront fermés dans le même
    sens (sinon il y aura des backfacing polygones, ce qui indique clairement
    qu'ils appartiennent à une autre surface et il n'y a aucune raison de faire
    un extrude entre deux surfaces complètement disjointes). 
    1-Donc a partir de ce sens, on trouve l'ordre des deux premier point (a-b).
    2-Ensuite, on trouve le prochain segment qui a le point b.
    3- De la, on trouve l'ordre des
    points de ce segment et on recommence.
    
    Example:
    
   a-----b\      h
   |       \    /|
   |        \  / |
   f         c/--i
   \         /
    \       /
     e--- d
    unorderedContour
    b-a|b-c|d-c|e-f|d-e|c-h|i-c|h-i
    
    contour
    a|b|c|d|e|f|a|c|h|i|c */
    
  //pas de contour non ordonné, on sort
  if(unorderedContour.empty())
    return false;
  
  firstSegment(unorderedContour.front());
  unorderedContour.erase(unorderedContour.begin());
  
  while(!unorderedContour.empty())
  {
    nextSegment(unorderedContour);
  }
  
  /*mContour contient maintenant tous les points de contour
    ordonnée!*/
  return true;
}

//------------------------------------------------------------------------------
/*Puisqu'on a déja le premier segment dans le bon ordre, on parcours le reste
  des segments non ordonnés en cherchant le dernier point du contour ordonné.
  Quand on le trouve, on ajoute le deuxieme point du segment au contour 
  ordonné.*/
void Extrude::nextSegment(vector<RealEditSegment>& iUnordered)
{
	bool nextSegmentFound = false;
	//le id du dernier point du contour ordonné
	unsigned int id = mContour.back().getId();
  for(unsigned int i = 0; i < iUnordered.size(); ++i)
  {
  	RealEditSegment& s = iUnordered[i];
    if(s.getPoint1().getId() == id)
    {
      mContour.push_back(s.getPoint2());
      iUnordered.erase(iUnordered.begin() + i);
      nextSegmentFound = true;
      break;
    }
    else if(s.getPoint2().getId() == id)
    {
      mContour.push_back(s.getPoint1());
      iUnordered.erase(iUnordered.begin() + i);
      nextSegmentFound = true;
      break;
    }
  }
  
  if(!nextSegmentFound)
  {
  	firstSegment(iUnordered.front());
    iUnordered.erase(iUnordered.begin());
  }
}

//------------------------------------------------------------------------------
void Extrude::redo()
{
  EditionData& e = mController.getEditionData();
  RealEditModel& m = e.getCurrentModel(); 
  
  mpRemove->redo();
  
  for(uint i = 0; i < mAddedPoints.size(); ++i)
  	m.addPoint(mAddedPoints[i]);
    
  for(uint i = 0; i < mAddedPolygons.size(); ++i)
  	m.addPolygon(mAddedPolygons[i]);
    
  e.select(mNewSelection);
}

//------------------------------------------------------------------------------
void Extrude::undo()
{
  EditionData& e = mController.getEditionData();
  
  //on enleve tous les polygones créé lors de l'Extrusion
  set<uint> s;
  for(uint i = 0; i < mAddedPolygons.size(); ++i)
  	s.insert(mAddedPolygons[i].getId());
    
  e.select(s);
  Remove r(mController);
  r.execute();
  
  //on rajoute les polygones qu'on avait enlevé durant l'extrusion
  mpRemove->undo();
}

