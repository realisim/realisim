#include "Broker.h"
#include "Definitions.h"
#include <deque>
#include "ImageLoader.h"
#include "FileStreamer.h"
#include "math/Matrix4.h"
#include "Representations.h"
#include "Scene.h"
#include "utils/Timer.h"

using namespace realisim;
    using namespace math;

Scene::Scene(Hub* ipHub) :
mpHub(ipHub),
mpRoot(nullptr)
{

    //register fileStreamingDoneQueue to the fileStreamer
    FileStreamer& fs = getHub().getFileStreamer();
    using placeholders::_1;
    mFileLoadingDoneQueue.setProcessingFunction(
        std::bind( &Scene::processFileLoadingDoneMessage, this, _1));
    fs.registerDoneQueue(this, &mFileLoadingDoneQueue);
}

//----------------------------------------
Scene::~Scene()
{
    clear();
    
    if(mpRoot)
    { delete mpRoot; }
}

//------------------------------------------------------------------------------
void Scene::addNode(IGraphicNode* iNode)
{
    if(mpRoot == nullptr)
    { mpRoot = new GroupNode; } //first dummy node...
    
    mpRoot->mChilds.push_back(iNode);
    
    //populate the id to definition map
    utils::Timer _t;
    addToDefinitionMap(iNode);
    printf("time to addToDefinitionMap %f\n", _t.getElapsed() );
    
    Broker& b = getHub().getBroker();
    StatsPerFrame& spf = b.getStatsPerFrame();
    spf.mTotalNumberOfIGraphicNode += countChilds(iNode);

    mNeedsTransformUpdate.push_back(iNode);
}

//------------------------------------------------------------------------------
void Scene::addToDefinitionMap(IGraphicNode *iNode)
{
    if(iNode == nullptr) return;
    
    switch (iNode->mNodeType)
    {
        // tte library contains all the images...
        case IGraphicNode::ntLibrary:
        {
            LibraryNode* ln = dynamic_cast<LibraryNode*>(iNode);
            assert(ln);
            for(size_t i = 0; i < ln->mImages.size(); ++i)
            {
                Image *im = ln->mImages[i];
                auto it = mIdToDefinition.insert( make_pair(im->getId(), im) );
                // just validate that there is no duplicate...
                // if there is a duplicate we should understand why..
                // It could be possible if we reuse nodes, but in that case
                // nodes should have a refcount, so only the last deleted instance
                // frees up memory...
                assert(it.second);
            }
            
        }break;
        default: break;
    }
    
    for(size_t i = 0; i < iNode->mChilds.size(); ++i)
    {
        addToDefinitionMap(iNode->mChilds[i]);
    }
}

//------------------------------------------------------------------------------
void Scene::addToTextureLibrary(Image *iIm)
{
    IDefinition *def = dynamic_cast<IDefinition*>(iIm);
    auto it = mImageIdToTexture.find(def->getId());
    if(it == mImageIdToTexture.end())
    {
        realisim::treeD::Texture t;

        realisim::math::Vector2i size(iIm->mWidth, iIm->mHeight);
        GLenum internalFormat = GL_SRGB8_ALPHA8;
        GLenum format = GL_RGBA;
        GLenum datatype = GL_UNSIGNED_BYTE;
        
        switch(iIm->mNumberOfChannels)
        {
            case 1:
                internalFormat = GL_R8;
                format = GL_RED;
                break;
            case 2:
                internalFormat = GL_RG8;
                format = GL_RG;
                break;
            case 3:
                //internalFormat = GL_SRGB8;
                internalFormat = GL_RGB8;
                format = GL_RGB;
                break;
            case 4:
                //internalFormat = GL_SRGB8_ALPHA8;
                internalFormat = GL_RGBA8;
                format = GL_RGBA;
                break;
            default: break;
        }

        t.set(iIm->mpPayload, size, internalFormat, format, datatype);
        t.generateMipmap(true);
        t.setMagnificationFilter(GL_LINEAR);
        t.setMinificationFilter(GL_LINEAR_MIPMAP_LINEAR);
        
        mImageIdToTexture.insert( make_pair(def->getId(), t) );
    }
}

////------------------------------------------------------------------------------
//void Scene::checkAndCreateRepresentation(ModelNode *iNode)
//{
//    const IDefinition *def = dynamic_cast<const IDefinition*>(iNode);
//    auto repIt = mDefinitionIdToRepresentation.find(def->getId());
//    if(repIt == mDefinitionIdToRepresentation.end())
//    {
//        check if model can be created... meaning that images
//        have been uploaded to gpu...
//        if not, send a request to the fileStreamer to load the image.
//        
//        Representations::Model  *model = nullptr;
//        model = new Representations::Model(iNode, mImageIdToTexture);
//        mDefinitionIdToRepresentation.insert( make_pair(def->getId(), model) );
//    }
//}

//------------------------------------------------------------------------------
void Scene::clear()
{
    if(mpRoot)
    {
        delete mpRoot;
        mpRoot = nullptr;
    }
    
    auto it = mDefinitionIdToRepresentation.begin();
    for(; it != mDefinitionIdToRepresentation.end(); ++it)
    {
        delete it->second;
    }
    
    mIdToDefinition.clear();
    mDefinitionIdToRepresentation.clear();
    mImageIdToTexture.clear();
    
    mNeedsRepresentationCreation.clear();
    mNeedsTransformUpdate.clear();
    
    mDefaultLayer.clear();
    mLayers.clear();

    //clear stats
    Broker& b = getHub().getBroker();
    StatsPerFrame& spf = b.getStatsPerFrame();
    spf.clear();

}

//------------------------------------------------------------------------------
int Scene::countChilds(const IGraphicNode* ipCurrentNode) const
{
    int r = 0;
    countChilds(ipCurrentNode, r);
    return r;

}

//------------------------------------------------------------------------------
void Scene::countChilds(const IGraphicNode* ipCurrentNode, int& iCurrentCount) const
{
    if(ipCurrentNode == nullptr) {return;}

    iCurrentCount += ipCurrentNode->mChilds.size();
    for(size_t i = 0; i < ipCurrentNode->mChilds.size(); ++i)
    {
        countChilds(ipCurrentNode->mChilds[i], iCurrentCount);
    }
}

//------------------------------------------------------------------------------
void Scene::createRepresentations(ModelNode *iNode)
{
    const IDefinition *def = dynamic_cast<const IDefinition*>(iNode);

    auto repIt = mDefinitionIdToRepresentation.find(def->getId());
    if(repIt == mDefinitionIdToRepresentation.end())
    {
        // instance creation
        if( iNode->isInstantiated() )
        {
            auto instantiatedFromRepIt = mDefinitionIdToRepresentation.find(def->getInstantiatedFromId());

            // check just in case the instance would try to be created before the original
            // model is created...
            //
            if (instantiatedFromRepIt != mDefinitionIdToRepresentation.end())
            {
                Representations::Model *instantiatedFrom = (Representations::Model*)(instantiatedFromRepIt->second);
                Representations::Model *model = new Representations::Model;
                model->createInstance(iNode, instantiatedFrom);
                mDefinitionIdToRepresentation.insert( make_pair(def->getId(), model) );            
            }
        }
        else
        {
            //        check if model can be created... meaning that images
            //            have been uploaded to gpu...
            //            if not, send a request to the fileStreamer to load the image.
            bool canCreateRepresentation = true;
            for(int i = 0; i < iNode->mFaces.size() && canCreateRepresentation; ++i)
            {
                Face *f = iNode->mFaces[i];
                if(f->mpMaterial && f->mpMaterial->mpImage)
                {
                    Image *im = f->mpMaterial->mpImage;
                    auto it = mImageIdToTexture.find(im->getId());
                    if(it == mImageIdToTexture.end())
                    {
                        canCreateRepresentation = false;
                        // image is not on gpu, lets ask filestreamer to
                        // load it
                        //
                        FileStreamer& fs = getHub().getFileStreamer();
                        FileStreamer::Request *r = new FileStreamer::Request(this);
                        r->mRequestType = FileStreamer::rtLoadRgbImage;
                        r->mFilenamePath = im->mFilenamePath;
                        r->mAffectedDefinitionId = im->getId();
                        fs.postRequest(r);
                    }
                }
            }

            if(canCreateRepresentation)
            {
                Representations::Model *model = new Representations::Model;
                model->create(iNode, mImageIdToTexture);
                mDefinitionIdToRepresentation.insert( make_pair(def->getId(), model) );
            }
        }
    }
}

//------------------------------------------------------------------------------
IDefinition* Scene::findDefinition(unsigned int iId)
{
    auto it = mIdToDefinition.find(iId);
    return it != mIdToDefinition.end() ? it->second : nullptr;
}

//------------------------------------------------------------------------------
IGraphicNode* Scene::getRoot() const
{ return mpRoot; }

//----------------------------------------
void Scene::prepareFrame(IGraphicNode* iNode, std::vector<Representations::Representation*> *ipCurrentLayer)
{
    //depth First...
    if(iNode == nullptr) {return;}

    Broker& b = getHub().getBroker();
    StatsPerFrame& spf = b.getStatsPerFrame();
    spf.mNumberOfIGraphicNodeDisplayed++;

    //perform culling, lod, switches and most of the scenegraph logic...

    //fill layers
    bool recurseInChilds = true;
    switch (iNode->mNodeType)
    {
    case IGraphicNode::ntGroup:
    {
        GroupNode* gn = dynamic_cast<GroupNode*>(iNode);
        if (gn->isLayered())
        {
            //add layer if it does not exist and make it current
            auto itLayer = mLayers.insert( 
                make_pair(gn->getLayerIndex(), vector<Representations::Representation*>() ) );

            // itLayer is the pair<iterator, bool> returned by insert.
            // itLayer.first is the iterator to the pair<int, vector>, so
            // itLayer.first->second is the vector of representation, namely the list
            // representing the layer.
            //
            ipCurrentLayer = &(itLayer.first->second);
        }
    }break;
    case IGraphicNode::IGraphicNode::ntModel:
    {
        ModelNode* mn = dynamic_cast<ModelNode*>(iNode);
        auto itModel = mDefinitionIdToRepresentation.find(mn->getId());
        if(itModel != mDefinitionIdToRepresentation.end()) 
        {
            if(mn->isVisible())
            { ipCurrentLayer->push_back( itModel->second ); }
        }
        else
        {
            // this model does not have a representation, lets
            // put it in the list of the ones that need a rep creation...
            //
            mNeedsRepresentationCreation.push_back(mn);
        }

        spf.mNumberOfPolygons += mn->mFaces.size();
    }break;
    case IGraphicNode::ntLevelOfDetail:
    {
        //evaluate lod and if failure, do not recurse...
        Broker& b = getHub().getBroker();
        Point3d cameraPos = b.getCamera().getPos();
        
        LevelOfDetailNode* lod = dynamic_cast<LevelOfDetailNode*>(iNode);
        double lodDistance = (lod->getPositionnedLodCenter() - cameraPos).fastNorm();

        recurseInChilds = false;
        if( lodDistance >= lod->getSwitchOutDistance() && lodDistance <= lod->getSwitchInDistance() )
        { recurseInChilds = true; }
        
    }break;
    default: break;
    }

    // recurse on all childs
    if (recurseInChilds)
    {
        for (size_t i = 0; i < iNode->mChilds.size(); ++i)
        {
            prepareFrame(  iNode->mChilds[i], ipCurrentLayer );
        }
    }
}

//----------------------------------------
void Scene::performCulling(IGraphicNode* iNode)
{
    
}

//----------------------------------------
void Scene::processFileLoadingDoneMessage(MessageQueue::Message* ipMessage)
{
    FileStreamer::Request *r = (FileStreamer::Request *)ipMessage;
    /*printf("Scene::processFileLoadingDoneMessage %s\n",
           r->mFilenamePath.c_str() );*/
    
    RgbImage *loadedIm = (RgbImage*)r->mpData;
    
    // find the image associated with the request that we are currently handling.
    IDefinition* def = findDefinition(r->mAffectedDefinitionId);
    Image *im = dynamic_cast<Image*>(def);
    assert(im);
    
    if(loadedIm->isValid() && im != nullptr)
    {
        im->mpPayload = loadedIm->giveOwnershipOfImageData();
        addToTextureLibrary(im);
        im->unload();
    }
    else
    {
        // loaded image is not valid... maybe not present or else...
        // lets replace it by a dummy image...
        im->mWidth = 64;
        im->mHeight = 64;
        im->mNumberOfChannels = 3;
        im->mBitsPerChannel = 8;
        im->mSizeInBytes = im->mWidth * im->mHeight * im->mNumberOfChannels * im->mBitsPerChannel / 8;
        unsigned char *payload = new unsigned char[im->mSizeInBytes];
        memset(payload, 255, im->mSizeInBytes);
        im->mpPayload = payload;

        addToTextureLibrary(im);
        im->unload();
        //assert(false && "image was not loaded or definition was not found...");
    }
    
    delete loadedIm;
}

//----------------------------------------
void Scene::update()
{
    //clear per frame stats...
    Broker& b = getHub().getBroker();
    StatsPerFrame& spf = b.getStatsPerFrame();
    spf.clearPerFrameStats();

    //process fileStreamingDoneQueue
    mFileLoadingDoneQueue.processMessages();
    
    for(size_t i = 0; i < mNeedsTransformUpdate.size(); ++i)
    {
        updateTransform( mNeedsTransformUpdate[i] );
        updateBoundingBoxes( mNeedsTransformUpdate[i] );
        updateLod(mNeedsTransformUpdate[i]);
    }
    mNeedsTransformUpdate.clear();
    
    for(size_t i = 0; i < mNeedsRepresentationCreation.size(); ++i)
    {
        createRepresentations(mNeedsRepresentationCreation[i]);
    }
    mNeedsRepresentationCreation.clear();
    
    //clear all draw list...
    mDefaultLayer.clear();
    mLayers.clear();

    // update stats...
    utils::Timer _t;
    prepareFrame(mpRoot, &mDefaultLayer);
    spf.mTimeToPrepareFrame = _t.getElapsed();
}

//----------------------------------------
void Scene::updateBoundingBoxes(IGraphicNode* ipNode)
{
    //depth first...
    for (size_t i = 0; i < ipNode->mChilds.size(); ++i)
    {
        updateBoundingBoxes( ipNode->mChilds[i] );
    }

    IRenderable *r = dynamic_cast<IRenderable*>(ipNode);
    if (r)
    { r->updateBoundingBoxes(); }
}

//----------------------------------------
void Scene::updateLod(IGraphicNode* ipNode)
{
    //depth first...
    for (size_t i = 0; i < ipNode->mChilds.size(); ++i)
    {
        updateLod( ipNode->mChilds[i] );
    }

    LevelOfDetailNode *lod = dynamic_cast<LevelOfDetailNode*>(ipNode);
    if (lod)
    {
        Point3d o = lod->getOriginalLodCenter();        
        lod->setPositionnedLodCenter( lod->mWorldTransform * o );
    }
}

//----------------------------------------
void Scene::updateTransform(IGraphicNode* ipNode)
{
    //find first iRenderable parent, to get parentTransform
    IRenderable *parent = ipNode->getFirstParent<IRenderable>();
    Matrix4 parentTransform;
    if (parent != nullptr)
    {
        parentTransform = parent->mWorldTransform;
    }

    updateTransform(ipNode, parentTransform);
}

//----------------------------------------
void Scene::updateTransform(IGraphicNode* ipNode, Matrix4 iParentWorldTransform)
{
    IRenderable *r = dynamic_cast<IRenderable*>(ipNode);
    if (r && r->mIsTransformDirty)
    {
        r->mWorldTransform = iParentWorldTransform * r->mParentTransform;
        iParentWorldTransform = r->mWorldTransform;
        r->mIsTransformDirty = false;
    }

    for(size_t i = 0; i < ipNode->mChilds.size(); ++i)
    { updateTransform(ipNode->mChilds[i], iParentWorldTransform); }
}

//---------------
//--- Scene::Path
//---------------
Scene::Path::Path(IGraphicNode* ipNode)
{
    IGraphicNode* n = ipNode;
    while (n != nullptr)
    {
        mParents.push_back(n);
        n = n->mpParent;
    }
}
