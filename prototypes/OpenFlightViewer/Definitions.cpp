#include "Definitions.h"

unsigned int Definition::mIdCounter = 0;

Definition::Definition() :
mId(mIdCounter++)
{ }

Definition::~Definition()
{}

//--------------------------------------------------------
//--- Node
//--------------------------------------------------------

Node::~Node()
{
    for(size_t i = 0; i < mChilds.size(); ++i)
    { delete mChilds[i]; }
    mChilds.clear();
}

//--------------------------------------------------------
//--- OpenFlightNode
//--------------------------------------------------------

OpenFlightNode::~OpenFlightNode()
{}

//--------------------------------------------------------
//--- LibraryNode
//--------------------------------------------------------

LibraryNode::~LibraryNode()
{
    if(mpVertexPool){ delete mpVertexPool; }
    
    for(size_t i = 0; i < mImages.size(); ++i)
    { delete mImages[i]; }
    mImages.clear();
}

//--------------------------------------------------------
//--- ModelNode
//--------------------------------------------------------

ModelNode::~ModelNode()
{
    for(size_t i = 0; i < mMeshes.size(); ++i)
    { delete mMeshes[i]; }
    mMeshes.clear();
}

//--------------------------------------------------------
//--- GroupNode
//--------------------------------------------------------

GroupNode::~GroupNode()
{
}

//--------------------------------------------------------
//--- Utilitaires
//--------------------------------------------------------
LibraryNode* getLibraryFor(Node* iNode)
{
    Node* r = nullptr;
    
    Node* currentNode = iNode;
    while( currentNode != nullptr && r == nullptr )
    {
        if(currentNode->mNodeType == Node::ntLibrary)
        {
            r = currentNode;
        }
        currentNode = currentNode->mpParent;
    }
    return (LibraryNode*)r;
}