#include "Definitions.h"

#include "ImageLoader.h"


//--------------------------------------------------------
//--- Interfaces
//--------------------------------------------------------

//--------------------------------------------------------
//--- IGraphicNode
//--------------------------------------------------------
IGraphicNode::IGraphicNode() :
mpParent(nullptr),
mChilds(),
mNodeType(ntUndefined),
mName("N/A")
{}

IGraphicNode::~IGraphicNode()
{
    //    for(size_t i = 0; i < mChilds.size(); ++i)
    //    { delete mChilds[i]; }
    //    mChilds.clear();
}

//--------------------------------------------------------
void IGraphicNode::addChild(IGraphicNode* ipNode)
{
    ipNode->mpParent = this;
    mChilds.push_back(ipNode);
}

//--------------------------------------------------------
//--- IRenderable
//--------------------------------------------------------
IRenderable::~IRenderable()
{}

//--------------------------------------------------------
//--- IDefinitions
//--------------------------------------------------------
unsigned int IDefinition::mIdCounter = 0;

IDefinition::IDefinition() :
mId(mIdCounter++)
{ }

IDefinition::~IDefinition()
{}

//--------------------------------------------------------
//--- Image
//--------------------------------------------------------
Image::~Image()
{
    //delete payload...
}

void Image::load()
{
    //check file extension for the correct imageLoader..
    //here we assume it is RGB
    RgbImageLoader il;
    il.setFilenamePath(mFilenamePath);
    il.load();
    mWidth = il.getPixelSizeX();
    mHeight = il.getPixelSizeY();
    mNumberOfChannels = il.getNumberOfChannels();
    mBitsPerChannel = il.getBytesPerPixel() * 8;
    mSizeInBytes = mWidth * mHeight * mNumberOfChannels * mBitsPerChannel / 8;
    mpPayload = il.giveOwnershipOfImageData();
}

//--------------------------------------------------------

void Image::loadMetaData()
{
    //check file extension for the correct imageLoader..
    //here we assume it is RGB
    RgbImageLoader il;
    il.setFilenamePath(mFilenamePath);
    il.loadHeader();
    mWidth = il.getPixelSizeX();
    mHeight = il.getPixelSizeY();
    mNumberOfChannels = il.getNumberOfChannels();
    mBitsPerChannel = il.getBytesPerPixel() * 8;
    mSizeInBytes = mWidth * mHeight * mNumberOfChannels * mBitsPerChannel / 8;
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
//    if(mpVertexPool){ delete mpVertexPool; }
//    
//    for(size_t i = 0; i < mImages.size(); ++i)
//    { delete mImages[i]; }
//    mImages.clear();
}

//--------------------------------------------------------
//--- ModelNode
//--------------------------------------------------------

ModelNode::~ModelNode()
{
    //delete...
}

//--------------------------------------------------------
//--- Utilitaires
//--------------------------------------------------------
LibraryNode* getLibraryFor(IGraphicNode* iNode)
{
    IGraphicNode* r = nullptr;
    
    IGraphicNode* currentNode = iNode;
    while( currentNode != nullptr && r == nullptr )
    {
        if(currentNode->mNodeType == IGraphicNode::ntLibrary)
        {
            r = currentNode;
        }
        currentNode = currentNode->mpParent;
    }
    return (LibraryNode*)r;
}