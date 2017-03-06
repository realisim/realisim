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
mName("N/A"),
mUseCount(1)
{}

IGraphicNode::~IGraphicNode()
{
        for(size_t i = 0; i < mChilds.size(); ++i)
        { 
            if( mChilds[i]->decrementUseCount() == 1 )
            { delete mChilds[i]; }
        }
        mChilds.clear();
}

//--------------------------------------------------------
void IGraphicNode::addChild(IGraphicNode* ipNode)
{
    ipNode->mpParent = this;
    mChilds.push_back(ipNode);
}

//--------------------------------------------------------
int IGraphicNode::decrementUseCount()
{
    int r = mUseCount;
    incrementUseCount(this, -1);
    return r;
}

//--------------------------------------------------------
void IGraphicNode::incrementUseCount()
{
    incrementUseCount(this, 1);
}

//--------------------------------------------------------
void IGraphicNode::incrementUseCount(IGraphicNode* ipNode, int iInc)
{
    ipNode->mUseCount += iInc;
    for (size_t i = 0; i < ipNode->mChilds.size(); ++i)
    {
        incrementUseCount(ipNode->mChilds[i], iInc);
    }
}

//--------------------------------------------------------
int IGraphicNode::getUseCount() const
{
    return mUseCount;
}

//--------------------------------------------------------
//--- IRenderable
//--------------------------------------------------------
IRenderable::IRenderable() : 
    mIsTransformDirty(true), 
    mIsVisible(true),
    mIsBoundingBoxVisible(false)
{}

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
void IRenderable::updateBoundingBoxes()
{
    using namespace realisim::math;

    // update the positionned AABB
    Point3d m = mAxisAlignedBoundingBox.getMin();
    Point3d M = mAxisAlignedBoundingBox.getMax();

    mPositionnedAxisAlignedBoundingBox.add( mWorldTransform * m );
    mPositionnedAxisAlignedBoundingBox.add( mWorldTransform * Point3d(M.x(), m.y(), m.z()) );
    mPositionnedAxisAlignedBoundingBox.add( mWorldTransform * Point3d(M.x(), M.y(), m.z()) );
    mPositionnedAxisAlignedBoundingBox.add( mWorldTransform * Point3d(m.x(), M.y(), m.z()) );
    
    mPositionnedAxisAlignedBoundingBox.add( mWorldTransform * Point3d(m.x(), m.y(), M.z()) );
    mPositionnedAxisAlignedBoundingBox.add( mWorldTransform * Point3d(M.x(), m.y(), M.z()) );    
    mPositionnedAxisAlignedBoundingBox.add( mWorldTransform * M );
    mPositionnedAxisAlignedBoundingBox.add( mWorldTransform * Point3d(m.x(), M.y(), M.z()) );
}

//--------------------------------------------------------
//--- IDefinitions
//--------------------------------------------------------
unsigned int IDefinition::mIdCounter = 0;

IDefinition::IDefinition() :
mId(mIdCounter++),
mInstantiatedFromId(0)
{ }

IDefinition::~IDefinition()
{}

//--------------------------------------------------------
unsigned int IDefinition::getId() const
{  return mId; }

//--------------------------------------------------------
unsigned int IDefinition::getInstantiatedFromId() const
{ return mInstantiatedFromId; }

//--------------------------------------------------------
void IDefinition::setAsInstanceOf(unsigned int iId)
{
    mInstantiatedFromId = iId;
}

//--------------------------------------------------------
bool IDefinition::isInstantiated() const
{
    return mInstantiatedFromId != 0;
}

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
    if(mpVertexPool){ delete mpVertexPool; }
    
    for(size_t i = 0; i < mImages.size(); ++i)
    { delete mImages[i]; }
    mImages.clear();
}

//--------------------------------------------------------
//--- ModelNode
//--------------------------------------------------------
ModelNode::ModelNode() : IDefinition(), IGraphicNode(), IRenderable(),
mpInstancedFrom(nullptr)
{ mNodeType = ntModel; }

//--------------------------------------------------------
ModelNode::~ModelNode()
{
    if (isInstantiated())
    {
        mpInstancedFrom->decrementUseCount();
    }
}

//--------------------------------------------------------
void ModelNode::addFace(Face* iFace)
{
    mFaces.push_back(iFace);

    for (int i = 0; i < iFace->mVertexIndices.size(); ++i)
    {
        int index = iFace->mVertexIndices[i];
        
        mAxisAlignedBoundingBox.add( toPoint(iFace->mpVertexPool->mVertices[index]) );
    }
    
}

//--------------------------------------------------------
void ModelNode::setAsInstanceOf(ModelNode* ipInstancedFrom)
{
    // we should not call setAsInstanceOf twice on the same
    // object... If we want to do so, we need to handle it
    // here
    //
    assert(mpInstancedFrom == nullptr);
    mpInstancedFrom = ipInstancedFrom;
    mpInstancedFrom->incrementUseCount();
    IDefinition::setAsInstanceOf(mpInstancedFrom->getId());

    mAxisAlignedBoundingBox = ipInstancedFrom->getAABB();
}

//--------------------------------------------------------
//--- LevelOfDetailNode
//--------------------------------------------------------
LevelOfDetailNode::LevelOfDetailNode() : 
    IDefinition(),
    IGraphicNode(),
    mSitchInDistance(0.0),
    mSitchOutDistance(0.0),
    mOriginalLodCenter(0.0),
    mPositionnedLodCenter(0.0)
{ mNodeType = ntLevelOfDetail; }

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