//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "ObjectNode.h"

#include "DataModel.h"
#include "Matrix4x4.h"
#include "MathUtils.h"

using namespace RealEdit;
using namespace Realisim;
//------------------------------------------------------------------------------
ObjectNode::ObjectNode( const std::string& iName ) : mpModel( new RealEditModel() ),
  mpParentNode( 0 ),
  mChilds(),
  mTransformation(),
  mName( iName )
{
}

//------------------------------------------------------------------------------
ObjectNode::~ObjectNode()
{
  //il faut delete toute l'arboresence
  delete mpModel;
  for( unsigned int i = 0; i < getChildCount(); ++i )
  {
    delete getChild(i);
  }
}

//------------------------------------------------------------------------------
ObjectNode*
ObjectNode::addNode( const std::string& iName )
{
  ObjectNode* pNode = new ObjectNode( iName );
  this->mChilds.push_back( pNode );
  pNode->mpParentNode = this;
  
  return pNode;
}

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
const RealEditModel*
ObjectNode::getModel() const
{
  return mpModel;
}

//------------------------------------------------------------------------------
RealEditModel*
ObjectNode::getModel()
{
  return const_cast<RealEditModel*>( 
    static_cast<const ObjectNode&>(*this).getModel() );
}

//------------------------------------------------------------------------------
const std::string&
ObjectNode::getName() const
{
  return mName;
}

//------------------------------------------------------------------------------
const Realisim::Point3d
ObjectNode::getTranslation() const
{
  return Point3d( mTransformation[3][0], 
    mTransformation[3][1],
    mTransformation[3][2] );
}

//------------------------------------------------------------------------------
const Realisim::Matrix4d&
ObjectNode::getTransformation() const
{ 
  return mTransformation; 
}

//------------------------------------------------------------------------------
void ObjectNode::setTransformation( const Realisim::Matrix4d& iMat )
{
  mTransformation = iMat;
}

//------------------------------------------------------------------------------
//Translate le noeud a la position spécifiée par iPos
void ObjectNode::translate( const Realisim::Point3d& iPos )
{
  mTransformation.setTranslation(iPos);
}

//------------------------------------------------------------------------------
//Ajoute iTranslation a la position actuelle du noeud
void ObjectNode::translate( const Realisim::Vector3d& iTranslation )
{
  translate( getTranslation() + iTranslation  );
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
