
#include "RealEditController.h"

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
  vector<int> idPoints;
  idPoints.push_back( mEditionData.addPoint( Point3d( -2.0, 0.0, 0.0 ) ) );
  idPoints.push_back( mEditionData.addPoint( Point3d( 2.0, 0.0, 0.0 ) ) );
  idPoints.push_back( mEditionData.addPoint( Point3d( 0.0, 2.0, 0.0 ) ) );
  mEditionData.addPolygon(idPoints);
  
  ObjectNode* pBouetteNode = mEditionData.addNode( "bouette" );
  pBouetteNode->rotate( PI/4.0,
                       Vector3d( 1.0, 0.0, 0.0 ) );
  pBouetteNode->translate( Point3d( -8.0, 0.0, 0.0 ) );
//  pBouetteNode->rotate( PI/4.0,
//                       Vector3d( 0.0, 0.0, 1.0 ) );
  mEditionData.setCurrentNode( pBouetteNode );
  mEditionData.addPoint( Point3d( -2.0, 0.0, 0.0 ) );
  mEditionData.addPoint( Point3d( 2.0, 0.0, 0.0 ) );
    
  ObjectNode* pNode = mEditionData.addNode( "bouette3" );
  pNode->translate( Point3d( -8.0, 0.0, 0.0 ) );
  pNode->rotate( PI/2.0,
    Vector3d( 0.0, 0.0, 1.0 ),
    Point3d( 0.0, 0.0, 0.0 ) );
  mEditionData.setCurrentNode( pNode );
  idPoints.clear();
  idPoints.push_back(mEditionData.addPoint( Point3d( -2.0, 0.0, 0.0 ) ));
  idPoints.push_back(mEditionData.addPoint( Point3d( 2.0, 0.0, 0.0 ) ));
  idPoints.push_back(mEditionData.addPoint( Point3d( 0.0, 2.0, 0.0 ) ));
  mEditionData.addPolygon(idPoints);
  
  ObjectNode* pNodeCaca = mEditionData.addNode( "caca" );
  mEditionData.setCurrentNode(pNodeCaca);
  pNodeCaca->rotate(PI/4, 
                    Vector3d( 1.0, 0.0, 1.0 ) );
  pNodeCaca->translate(Point3d( -8.0, 0.0, 0.0 ));
  int a,b,c,d;
  a = mEditionData.addPoint( Point3d( -2.0, -2.0, -2.0 ) );
  b = mEditionData.addPoint( Point3d( 2.0, -2.0, 0.0 ) );
  c = mEditionData.addPoint( Point3d( 2.0, 2.0, 0.0 ) );
  d = mEditionData.addPoint( Point3d( -2.0, 2.0, 0.0 ) );
  idPoints.clear();
  idPoints.push_back(a);
  idPoints.push_back(b);
  idPoints.push_back(c);
  mEditionData.addPolygon(idPoints);
  idPoints.clear();
  idPoints.push_back(a);
  idPoints.push_back(c);
  idPoints.push_back(d);
  mEditionData.addPolygon(idPoints);
  
  mEditionData.addNode( "caca2" );
  mEditionData.addNode( "caca3" );
  
}

RealEditController::~RealEditController()
{
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



