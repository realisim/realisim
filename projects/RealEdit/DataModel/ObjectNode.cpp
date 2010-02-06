//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "DataModel/ObjectNode.h"
#include "math/Matrix4x4.h"
#include "math/MathUtils.h"
#include "math/Quaternion.h"

using namespace realEdit;
using namespace realisim;
using namespace realisim::math;
using namespace std;

//------------------------------------------------------------------------------
ObjectNode::ObjectNode( const QString iName ) : mModel (),
  mpParentNode( 0 ),
  mChilds(),
  mTransformation(),
  mName( iName )
{
}

//------------------------------------------------------------------------------
/*Très important que le parent de la copie du noeud ne soit pas associé parce
qu'il faut utiliser la méthode addNode(ObjectNode*) afin de conserver la
hiérarchie de noeud organisée.
*/
ObjectNode::ObjectNode (const ObjectNode& iN) : mModel (iN.mModel),
  mpParentNode (0),
  mChilds (),
  mTransformation (iN.mTransformation),
  mName (iN.mName)
{
  //créer l'arbre d'enfants
  for (unsigned int i = 0; i < iN.getChildCount(); ++i)
  {
    mChilds.push_back( new ObjectNode (*iN.getChild (i)) );
    //On parente les enfants correctement afin de conserver la hierarchie.
    mChilds[i]->setParentNode (this);
  }
}

//------------------------------------------------------------------------------
/*Object node est un arbre de pointeur sur d'autres objectNode. Il est donc
  important que chaque parent détruise ses enfants. Ainsi l'arbre se détruira
  complètement de façon récursive.
*/
ObjectNode::~ObjectNode()
{
  for (unsigned int i = 0; i < getChildCount (); ++i)
    delete getChild (i);
}

//------------------------------------------------------------------------------
ObjectNode* ObjectNode::addNode( const QString iName )
{
  ObjectNode* pNode = new ObjectNode( iName );
  this->mChilds.push_back( pNode );
  pNode->mpParentNode = this;
  
  return pNode;
}

//------------------------------------------------------------------------------
//void ObjectNode::addNode( ObjectNode* ipNode )
//{
//  this->mChilds.push_back( ipNode );
//  ipNode->mpParentNode = this;
//}

//------------------------------------------------------------------------------
const ObjectNode*
ObjectNode::getChild( int iChildNumber ) const
{
  return mChilds[iChildNumber];
}

//------------------------------------------------------------------------------
ObjectNode*
ObjectNode::getChild( int iChildNumber )
{
  return const_cast<ObjectNode*>(
    static_cast<const ObjectNode*>(this)->getChild( iChildNumber ) );
}

//------------------------------------------------------------------------------
unsigned int
ObjectNode::getChildCount() const
{
  return mChilds.size();
}

//------------------------------------------------------------------------------
const RealEditModel& ObjectNode::getModel() const
{ return mModel; }

//------------------------------------------------------------------------------
RealEditModel& ObjectNode::getModel()
{ return mModel; }

//------------------------------------------------------------------------------
const QString
ObjectNode::getName() const
{
  return mName;
}

//------------------------------------------------------------------------------
const Point3d
ObjectNode::getTranslation() const
{
  return Point3d( mTransformation(3, 0), 
    mTransformation(3, 1),
    mTransformation(3, 2) );
}

//------------------------------------------------------------------------------
const Matrix4d&
ObjectNode::getTransformation() const
{ 
  return mTransformation; 
}

//------------------------------------------------------------------------------
void ObjectNode::setTransformation( const Matrix4d& iMat )
{
  mTransformation = iMat;
}

//------------------------------------------------------------------------------
/*Translate le noeud a la position spécifiée par iPos. Cette translation est
rigide, elle ne tiendra pas compte de l'orientation actuelle du noeud
ex: 
node.rotate (Pi/2, Vector3d (0, 0, 1)); // rotation de 45 deg par rapport a Z
node.translate (Point3d (3, 0, 0));

  Root---node
*/
void ObjectNode::translate( const Point3d& iPos )
{
  mTransformation.setTranslation(iPos);
}

//------------------------------------------------------------------------------
/*Ajoute iTranslation a la position actuelle du noeud. Cette translation prend
en compte l'orientation actuelle du noeud.
ex: 
node.rotate (Pi/2, Vector3d (0, 0, 1)); // rotation de 45 deg par rapport a Z
node.translate (Vector3d (8, 0, 0));

  Root
      \
       \
        \
         node
*/
void ObjectNode::translate( const Vector3d& iTranslation )
{
  Vector3d v = iTranslation * mTransformation;
  translate( getTranslation() + v  );
}

//------------------------------------------------------------------------------
//rotate le systeme de coordonné de iAngle radian
void ObjectNode::rotate( double iAngle, const Vector3d& iAxis )
{
  Matrix4d rotation = getRotationMatrix( iAngle, iAxis );

  rotation *= getTransformation();
  setTransformation( rotation );
}

//------------------------------------------------------------------------------
//rotate le systeme de coordonné du noeud de iAngle radian autour de l'axe
//iAxis localisé au point iAxisPos
void ObjectNode::rotate( const double iAngle, const Vector3d& iAxis,
                         const Point3d& iAxisPos )
{
  rotate( iAngle, iAxis );
  Point3d translation = rotatePoint(iAngle, getTranslation(), iAxis, iAxisPos);
  translate( translation );
}

//------------------------------------------------------------------------------
//   PATH
//------------------------------------------------------------------------------
Path::Path(const ObjectNode* ipNode) : mNodeToScene(), mSceneToNode()
{
  const ObjectNode* currentNode = ipNode;
  vector<const ObjectNode*> nodes;
  nodes.push_back(currentNode);
  while( currentNode->getParentNode() )
  {
    currentNode = currentNode->getParentNode();
    nodes.push_back(currentNode);
  }
  
  vector<const ObjectNode*>::iterator it = nodes.begin();
  for(; it != nodes.end(); ++it)
  {
    mNodeToScene *= (*it)->getTransformation();
  }
  
  mSceneToNode = mNodeToScene;
  mSceneToNode.inverse();
}

