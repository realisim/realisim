//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "ObjectNode.h"

#include "DataModel.h"

using namespace RealEdit;

ObjectNode::ObjectNode( const std::string& iName ) : mpModel( new RealEditModel() ),
  mpParentNode( 0 ),
  mChilds(),
  mName( iName )
{
}

ObjectNode::~ObjectNode()
{
  //il faut delete toute l'arboresence
  delete mpModel;
  for( unsigned int i = 0; i < getChildCount(); ++i )
  {
    delete getChild(i);
  }
}

const RealEditModel*
ObjectNode::getModel() const
{
  return mpModel;
}

RealEditModel*
ObjectNode::getModel()
{
  return const_cast<RealEditModel*>( 
    static_cast<const ObjectNode&>(*this).getModel() );
}

unsigned int
ObjectNode::getChildCount() const
{
  return mChilds.size();
}

const ObjectNode*
ObjectNode::getChild( int iChildNumber ) const
{
  return mChilds[iChildNumber];
}

const std::string&
ObjectNode::getName() const
{
  return mName;
}

ObjectNode*
ObjectNode::addNode( const std::string& iName )
{
  ObjectNode* pNode = new ObjectNode( iName );
  this->mChilds.push_back( pNode );
  pNode->mpParentNode = this;
  
  return pNode;
}