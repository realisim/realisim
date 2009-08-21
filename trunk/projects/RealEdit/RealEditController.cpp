
#include "commands, translate.h"
#include "assembly, rotate.h"
#include "RealEditController.h"
#include "DataModel.h"
#include "EditionUi.h"
#include "MathUtils.h"

using namespace realisim;
using namespace realEdit;
using namespace std;

RealEditController::RealEditController(EditionUi& iEditionUi) :
  mCommandStack(), 
  mDisplayData(),
  mEditionUi(iEditionUi),
  mEditionData(),
  mMode(mEdition)
{
  ObjectNode* pRootNode = getEditionData().getCurrentNode();
  createTetrahedron();

  ObjectNode* pBouetteNode = getEditionData().addNode( "étron" );
  pBouetteNode->translate( Point3d( -8.0, 0.0, 0.0 ) );
  pBouetteNode->rotate( PI/2.0,
    Vector3d( 0.0, 0.0, 1.0 ) );
  getEditionData().setCurrentNode( pBouetteNode );
  createCube();
  
  ObjectNode* pNode = getEditionData().addNode( "bâche" );
  pNode->rotate( PI/4.0,
    Vector3d( 0.0, 0.0, 1.0 ) );
  pNode->translate( Vector3d( -8.0, 0.0, 0.0 ) );
  getEditionData().setCurrentNode( pNode );
  createSphere(1);
}

RealEditController::~RealEditController()
{
}

void RealEditController::translate()
{
  commands::Translate* c = new commands::Translate(getEditionData());
  mCommandStack.add(c);
}

void RealEditController::rotate()
{
  commands::assembly::Rotate* c = new commands::assembly::Rotate(getEditionData());
  mCommandStack.add(c);
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
    {0,2,1}, {0,3,2}, {4,5,6}, {4,6,7}};
    
  vector<RealEditPoint> vPoints;
  vector<RealEditPoint> vFace;
  for (int i = 0; i < 8; ++i) 
  {   
    RealEditPoint p =
      getEditionData().addPoint ( Point3d (vdata[i][0],
          vdata[i][1],
          vdata[i][2])); 
    vPoints.push_back(p);
  }
  
  for (int i = 0; i < 12; ++i)
  {
    vFace.clear();
    vFace.push_back(vPoints[tindices[i][0]]);
    vFace.push_back(vPoints[tindices[i][1]]);
    vFace.push_back(vPoints[tindices[i][2]]);
    getEditionData().addPolygon(vFace);
  }
}

//------------------------------------------------------------------------------
void RealEditController::createSphere(unsigned int iLevel /*= 0*/)
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

  vector<RealEditPoint> vPoints;
  vector<RealEditPoint> vFace;
  for (int i = 0; i < 12; ++i) 
  {    
    RealEditPoint p =
      getEditionData().addPoint (Point3d (vdata[i][0],
        vdata[i][1],
        vdata[i][2]));
    vPoints.push_back(p);
  }
  
  for (int i = 0; i < 20; ++i)
  {
    vFace.clear();
    vFace.push_back(vPoints[tindices[i][0]]);
    vFace.push_back(vPoints[tindices[i][1]]);
    vFace.push_back(vPoints[tindices[i][2]]);
    subdivideIsocahedron(vFace, iLevel);
  }
  mIsocahedronSubdivision.clear();
}

//------------------------------------------------------------------------------
void RealEditController::createTetrahedron()
{
  GLfloat vdata[4][3] = {    
    {1, 1, 1}, {-1, -1, 1}, {-1, 1, -1}, {1, -1, -1}};    
  
  GLint tindices[4][3] = { 
    {0,2,1}, {0,1,3}, {1,2,3}, {2,0,3}};
    
  vector<RealEditPoint> vPoints;
  vector<RealEditPoint> vFace;
  for (int i = 0; i < 4; ++i) 
  {    
    RealEditPoint p = 
      getEditionData().addPoint ( Point3d (vdata[i][0],
        vdata[i][1],
        vdata[i][2]));
    vPoints.push_back(p);
  }
  
  for (int i = 0; i < 4; ++i)
  {
    vFace.clear();
    vFace.push_back(vPoints[tindices[i][0]]);
    vFace.push_back(vPoints[tindices[i][1]]);
    vFace.push_back(vPoints[tindices[i][2]]);
    getEditionData().addPolygon(vFace);
  }
}

//------------------------------------------------------------------------------
/*Cette méthode est protégé afin d'être utilisée seulement par 
RealEditController dans le but d'obliger les utilisateurs à passé par 
RealEditController afin de modifier les données.*/
EditionData& RealEditController::getEditionData()
{return mEditionData;}

//------------------------------------------------------------------------------
/*Cette méthode sert a offrir les données en lecture seulement a quiconque.*/
const EditionData& RealEditController::getEditionData() const
{return mEditionData;}

//------------------------------------------------------------------------------
/*Place le noeud courant et notifie l'interface, qui placera ensuite les caméras
*/
void RealEditController::setCurrentNode (const ObjectNode* ipNode)
{
  if(getEditionData().getCurrentNode() != ipNode)
  {
    getEditionData().setCurrentNode (ipNode);
    mEditionUi.currentNodeChanged ();
  }
}

//------------------------------------------------------------------------------
void RealEditController::setMode(MODE iMode)
{
  if(mMode != iMode)
  {
    mMode = iMode;
    mEditionUi.modeChanged();
  }
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
void RealEditController::subdivideIsocahedron(const vector<RealEditPoint>& iFace, long depth)
{
  Vector3d v1 = toVector (iFace[0].pos ()),
    v2 = toVector (iFace[1].pos ()),
    v3 = toVector (iFace[2].pos ());
  Vector3d v12, v23, v31;
    
  if (depth == 0)
  {
    getEditionData().addPolygon(iFace);
    return;
  }

  RealEditPoint newP1, newP2, newP3; 
  IsocahedronSubdivision::const_iterator it = 
    mIsocahedronSubdivision.find(make_pair(iFace[0].getId(), iFace[1].getId()));
  if(it == mIsocahedronSubdivision.end())
  {
    v12 = v1 + v2;
    v12.normalise();
    newP1 = getEditionData().addPoint(toPoint (v12));
    mIsocahedronSubdivision.insert (make_pair(make_pair(iFace[0].getId(), iFace[1].getId()), newP1));
    mIsocahedronSubdivision.insert (make_pair(make_pair(iFace[1].getId(), iFace[0].getId()), newP1));
  }
  else
  {
    newP1 = it->second;
  }
  
  it = 
  mIsocahedronSubdivision.find(make_pair(iFace[1].getId(), iFace[2].getId()));
  if(it == mIsocahedronSubdivision.end())
  {
    v23 = v2 + v3;
    v23.normalise();
    newP2 = getEditionData().addPoint (toPoint (v23));
    mIsocahedronSubdivision.insert (make_pair(make_pair(iFace[1].getId(), iFace[2].getId()), newP2));
    mIsocahedronSubdivision.insert (make_pair(make_pair(iFace[2].getId(), iFace[1].getId()), newP2));
  }
  else
  {
    newP2 = it->second;
  }
  
  it =
  mIsocahedronSubdivision.find(make_pair(iFace[2].getId(), iFace[0].getId()));
  if(it == mIsocahedronSubdivision.end())
  {
    v31 = v3 + v1;
    v31.normalise();
    newP3 = getEditionData().addPoint (toPoint (v31));
    mIsocahedronSubdivision.insert (make_pair(make_pair(iFace[2].getId(), iFace[0].getId()), newP3));
    mIsocahedronSubdivision.insert (make_pair(make_pair(iFace[0].getId(), iFace[2].getId()), newP3));
  }
  else
  {
    newP3 = it->second;
  }

  vector<RealEditPoint> v;
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
}
