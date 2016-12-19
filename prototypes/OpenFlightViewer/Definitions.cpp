#include "Definitions.h"

//--------------------------------------------------------
//--- Interfaces
//--------------------------------------------------------
IRenderable::~IRenderable()
{}

//--------------------------------------------------------
//--- Definitions
//--------------------------------------------------------
unsigned int Definition::mIdCounter = 0;

Definition::Definition() :
mId(mIdCounter++),
mpParent(nullptr),
mChilds(),
mNodeType(ntDefinition)
{ }

Definition::~Definition()
{
//    for(size_t i = 0; i < mChilds.size(); ++i)
//    { delete mChilds[i]; }
//    mChilds.clear();
}

//--------------------------------------------------------
void Definition::addChild(Definition* ipDef)
{
    ipDef->mpParent = this;
    mChilds.push_back(ipDef);
}

//--------------------------------------------------------
//--- OpenFlightNode
//--------------------------------------------------------

OpenFlightNode::~OpenFlightNode()
{}

//--------------------------------------------------------
//--- FaceNode
//--------------------------------------------------------

void FaceNode::addChild(Definition *ipDef)
{
    Definition::addChild(ipDef);
    switch (ipDef->mNodeType)
    {
        case Definition::ntVertexPool : mpVertexPool = (VertexPoolNode*)ipDef; break;
        case Definition::ntMaterial : mpMaterial = (MaterialNode*)ipDef; break;
        default: break;
    }
}

//--------------------------------------------------------
//--- LibraryNode
//--------------------------------------------------------

void LibraryNode::addChild(Definition *ipDef)
{
    Definition::addChild(ipDef);
    switch (ipDef->mNodeType)
    {
        case Definition::ntVertexPool : mpVertexPool = (VertexPoolNode*)ipDef; break;
        case Definition::ntImage : mImages.push_back( (ImageNode*)ipDef ); break;
        default: break;
    }
}

LibraryNode::~LibraryNode()
{
//    if(mpVertexPool){ delete mpVertexPool; }
//    
//    for(size_t i = 0; i < mImages.size(); ++i)
//    { delete mImages[i]; }
//    mImages.clear();
}

//--------------------------------------------------------
//--- MaterialNode
//--------------------------------------------------------

void MaterialNode::addChild(Definition *ipDef)
{
    Definition::addChild(ipDef);
    if(ipDef->mNodeType == ntImage)
    {
        mpImage = (ImageNode*)ipDef;
    }
}


//--------------------------------------------------------
//--- MeshNode
//--------------------------------------------------------

void MeshNode::addChild(Definition *ipDef)
{
    Definition::addChild(ipDef);
    switch (ipDef->mNodeType)
    {
        case Definition::ntVertexPool : mpVertexPool = (VertexPoolNode*)ipDef; break;
        case Definition::ntMaterial : mpMaterial = (MaterialNode*)ipDef; break;
        default: break;
    }
}

//--------------------------------------------------------
//--- ModelNode
//--------------------------------------------------------

ModelNode::~ModelNode()
{}

void ModelNode::addChild(Definition *ipDef)
{
    Definition::addChild(ipDef);
    switch (ipDef->mNodeType)
    {
        case Definition::ntMesh : mMeshes.push_back( (MeshNode*)ipDef ); break;
        case Definition::ntFace : mFaces.push_back( (FaceNode*)ipDef ); break;
        default: break;
    }
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
LibraryNode* getLibraryFor(Definition* iNode)
{
    Definition* r = nullptr;
    
    Definition* currentNode = iNode;
    while( currentNode != nullptr && r == nullptr )
    {
        if(currentNode->mNodeType == Definition::ntLibrary)
        {
            r = currentNode;
        }
        currentNode = currentNode->mpParent;
    }
    return (LibraryNode*)r;
}