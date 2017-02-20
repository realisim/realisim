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
bool IRenderable::isVisible() const
{
    return mIsVisible;
}

//--------------------------------------------------------
void IRenderable::setAsVisible(bool iVisible)
{
    setAsVisible( dynamic_cast<IGraphicNode*>(this), iVisible );
}

//--------------------------------------------------------
// the recursive function
void IRenderable::setAsVisible(IGraphicNode* ipNode, bool iVisible)
{
    if(ipNode == nullptr) {return;}

    IRenderable* r = dynamic_cast<IRenderable*>(ipNode);
    if (r)
    {
        r->mIsVisible = !r->mIsVisible;
    }

    //recurse on all child
    for (size_t i = 0; i < ipNode->mChilds.size(); ++i)
    {
        setAsVisible(ipNode->mChilds[i], iVisible);
    }
}

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
    unload();
}

//--------------------------------------------------------
void Image::load()
{
    //check file extension for the correct imageLoader..
    //here we assume it is RGB
    RgbImage il;
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
    RgbImage il;
    il.setFilenamePath(mFilenamePath);
    il.loadHeader();
    mWidth = il.getPixelSizeX();
    mHeight = il.getPixelSizeY();
    mNumberOfChannels = il.getNumberOfChannels();
    mBitsPerChannel = il.getBytesPerPixel() * 8;
    mSizeInBytes = mWidth * mHeight * mNumberOfChannels * mBitsPerChannel / 8;
}

//--------------------------------------------------------
void Image::unload()
{
    if(mpPayload)
    {
        delete[] mpPayload;
        mpPayload = nullptr;
    }
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