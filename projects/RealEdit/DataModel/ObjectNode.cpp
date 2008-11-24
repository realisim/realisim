//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "ObjectNode.h"

#include "DataModel.h"

using namespace RealEdit;

ObjectNode::ObjectNode() : mpModel( 0 ),
  mpParentNode( 0 ),
  mChilds()
{
  mpModel = new RealEditModel( "object 1" );
}

ObjectNode::~ObjectNode()
{
  //il faut delete toute l'arboresence
  delete mpModel;
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
ObjectNode::getNumChild() const
{
  return mChilds.size();
}

const ObjectNode*
ObjectNode::getChild( int iChildNumber ) const
{
  return mChilds[iChildNumber];
}