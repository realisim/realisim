
#include "RealEditController.h"
#include "DataModel.h"
#include "EditionUi.h"
#include "Point.h"

using namespace Realisim;
using namespace RealEdit;
using namespace std;

RealEditController::RealEditController(EditionUi& iEditionUi) : 
  mDisplayData(),
  mEditionUi(iEditionUi),
  mEditionData()
{
  ObjectNode* pRootNode = mEditionData.getCurrentNode();
  for (int i = 0; i < 5; ++i)
  {
    ObjectNode* n = mEditionData.addNode(QString::number(i).toStdString());
    mEditionData.setCurrentNode(n);
    n->translate(Vector3d(5*i, 0, 0));
    createSphere();
    mEditionData.setCurrentNode(pRootNode);
  }  
  
  ObjectNode* pBouetteNode = mEditionData.addNode( "bouette" );
  pBouetteNode->rotate( PI/4.0,
    Vector3d( 0.0, 0.0, 1.0 ) );
  pBouetteNode->translate( Point3d( -8.0, 0.0, 0.0 ) );
  mEditionData.setCurrentNode( pBouetteNode );
  createCube();
  
  ObjectNode* pNode = mEditionData.addNode( "bouette3" );
  pNode->rotate( PI/4.0,
    Vector3d( 1.0, 0.0, 0.0 ) );
  pNode->translate( Point3d( -8.0, 0.0, 0.0 ) );
  mEditionData.setCurrentNode( pNode );  
  createCube();
}

RealEditController::~RealEditController()
{
}

//------------------------------------------------------------------------------
void RealEditController::createCube()
{
  double c = 0.5;
  GLfloat vdata[8][3] = {    
    {-c, -c, c}, {c, -c, c}, {c, -c, -c}, {-c, -c, -c},    
    {-c, c, c}, {c, c, c}, {c, c, -c}, {-c, c, -c}};
  
  GLint tindices[12][3] = { 
    {0,1,5}, {0,5,4}, {1,2,6}, {1,6,5},
    {2,3,7}, {2,7,6}, {3,0,4}, {3,4,7},
    {0,1,2}, {0,2,3}, {4,5,6}, {4,6,7}};
    
  vector<unsigned int> idCube;
  vector<unsigned int> idPoints;
  for (int i = 0; i < 8; ++i) 
  {    
    idCube.push_back( 
      mEditionData.addPoint( Point3d( vdata[i][0],
        vdata[i][1],
        vdata[i][2] ) ) );
  }
  
  for (int i = 0; i < 12; ++i)
  {
    idPoints.clear();
    idPoints.push_back(idCube[tindices[i][0]]);
    idPoints.push_back(idCube[tindices[i][1]]);
    idPoints.push_back(idCube[tindices[i][2]]);
    mEditionData.addPolygon(idPoints);
  }
}

//------------------------------------------------------------------------------
void RealEditController::createSphere()
{  
  double X = .525731112119133606 ;
  double Z = .850650808352039932;
  
  GLfloat vdata[12][3] = {    
    {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},    
    {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},    
    {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 
  };
  
  GLint tindices[20][3] = { 
    {0,1,4}, {0,4,9}, {9,4,5}, {4,8,5}, {4,1,8},    
    {8,1,10}, {8,10,3}, {5,8,3}, {5,3,2}, {2,3,7},    
    {7,3,10}, {7,10,6}, {7,6,11}, {11,6,0}, {0,6,1}, 
    {6,10,1}, {9,11,0}, {9,2,11}, {9,5,2}, {7,11,2} };

  vector<unsigned int> idIsocahedron;
  vector<unsigned int> idPoints;
  for (int i = 0; i < 12; ++i) 
  {    
    idIsocahedron.push_back( 
      mEditionData.addPoint( Point3d( vdata[i][0],
        vdata[i][1],
        vdata[i][2] ) ) );
  }
  
  for (int i = 0; i < 20; ++i)
  {
    idPoints.clear();
    idPoints.push_back(idIsocahedron[tindices[i][0]]);
    idPoints.push_back(idIsocahedron[tindices[i][1]]);
    idPoints.push_back(idIsocahedron[tindices[i][2]]);
    subdivideIsocahedron(idPoints, 1);
    //mEditionData.addPolygon(idPoints);
  }
  
  mIsocahedronSubdivision.clear();
}


void RealEditController::subdivideIsocahedron(const vector<unsigned int>& iFace, long depth)
{
  Vector3d v1, v2, v3;
  Vector3d v12, v23, v31;
    
  if (depth == 0)
  {
    mEditionData.addPolygon(iFace);
    return;
  }
   
  const RealEditModel* m = mEditionData.getCurrentNode()->getModel();
  for(unsigned int i = 0; i < m->getPointCount(); ++i)
  {
   if (m->getPoint(i)->getId() == iFace[0])
     v1 = Vector3d(m->getPoint(i)->getX(), m->getPoint(i)->getY(), m->getPoint(i)->getZ());
   if (m->getPoint(i)->getId() == iFace[1])
     v2 = Vector3d(m->getPoint(i)->getX(), m->getPoint(i)->getY(), m->getPoint(i)->getZ());
   if (m->getPoint(i)->getId() == iFace[2])
     v3 = Vector3d(m->getPoint(i)->getX(), m->getPoint(i)->getY(), m->getPoint(i)->getZ());
  }

  unsigned int newP1, newP2, newP3; 
  IsocahedronSubdivision::const_iterator it = 
    mIsocahedronSubdivision.find(make_pair(iFace[0], iFace[1]));
  if(it == mIsocahedronSubdivision.end())
  {
    v12 = v1 + v2;
    v12.normalise();
    newP1 = mEditionData.addPoint(v12.toPoint());
    mIsocahedronSubdivision.insert (make_pair(make_pair(iFace[0], iFace[1]), newP1));
    mIsocahedronSubdivision.insert (make_pair(make_pair(iFace[1], iFace[0]), newP1));
  }
  else
  {
    newP1 = it->second;
  }
  
  it = 
  mIsocahedronSubdivision.find(make_pair(iFace[1], iFace[2]));
  if(it == mIsocahedronSubdivision.end())
  {
    v23 = v2 + v3;
    v23.normalise();
    newP2 = mEditionData.addPoint(v23.toPoint());
    mIsocahedronSubdivision.insert (make_pair(make_pair(iFace[1], iFace[2]), newP2));
    mIsocahedronSubdivision.insert (make_pair(make_pair(iFace[2], iFace[1]), newP2));
  }
  else
  {
    newP2 = it->second;
  }
  
  it =
  mIsocahedronSubdivision.find(make_pair(iFace[2], iFace[0]));
  if(it == mIsocahedronSubdivision.end())
  {
    v31 = v3 + v1;
    v31.normalise();
    newP3 = mEditionData.addPoint(v31.toPoint());
    mIsocahedronSubdivision.insert (make_pair(make_pair(iFace[2], iFace[0]), newP3));
    mIsocahedronSubdivision.insert (make_pair(make_pair(iFace[0], iFace[2]), newP3));
  }
  else
  {
    newP3 = it->second;
  }

  vector<unsigned int> v;
  v.push_back(iFace[0]);
  v.push_back(newP1);
  v.push_back(newP3);
  subdivideIsocahedron(v, depth-1);
  v.clear();
  v.push_back(iFace[1]);
  v.push_back(newP2);
  v.push_back(newP1);
  subdivideIsocahedron(v, depth-1);
  v.clear();
  v.push_back(iFace[2]);
  v.push_back(newP3);
  v.push_back(newP2);
  subdivideIsocahedron(v, depth-1);
  v.clear();
  v.push_back(newP1);
  v.push_back(newP2);
  v.push_back(newP3);
  subdivideIsocahedron(v, depth-1);
  
    
//  subdivide(v1, v12, v31, depth-1);
//  subdivide(v2, v23, v12, depth-1);
//  subdivide(v3, v31, v23, depth-1);
//  subdivide(v12, v23, v31, depth-1);
  
}


//------------------------------------------------------------------------------
void
RealEditController::newProject()
{
  EditionUi* p = new EditionUi();
  //pas besoin de deleter la fenetre, Qt le fera lorsque la fenêtre sera fermé
  p->setAttribute(Qt::WA_DeleteOnClose, true);
}

//------------------------------------------------------------------------------
void RealEditController::setCurrentNode( ObjectNode* ipNode )
{
  mEditionData.setCurrentNode(ipNode);
  mEditionUi.currentNodeChanged();
}



