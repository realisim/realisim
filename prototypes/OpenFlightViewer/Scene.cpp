#include "Broker.h"
#include "Definitions.h"
#include <deque>
#include "ImageLoader.h"
#include "FileStreamer.h"
#include "GpuStreamer.h"
#include "math/Matrix4.h"
#include "Representations.h"
#include "Scene.h"
#include "utils/Timer.h"

using namespace realisim;
    using namespace math;

const int gMaxGpuStreamerPostPerFrame = 1;
int gNumberOfGpuStreamerPostOnThisFrame = 0;

const int gMaxFileStreamerPostPerFrame = 1;
int gNumberOfFileStreamerPostOnThisFrame = 0;

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

    //register gpuStreamingDoneQueue to the gpuStream
    GpuStreamer& gs = getHub().getGpuStreamer();
    using placeholders::_1;
    mGpuStreamingDoneQueue.setProcessingFunction(
        std::bind( &Scene::processGpuStreammingDoneMessage, this, _1));
    gs.registerDoneQueue(this, &mGpuStreamingDoneQueue);
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
//void Scene::addToTextureLibrary(Image *iIm)
//{
//    IDefinition *def = dynamic_cast<IDefinition*>(iIm);
//    auto it = mImageIdToTexture.find(def->getId());
//    if(it == mImageIdToTexture.end())
//    {
//        realisim::treeD::Texture t;
//
//        realisim::math::Vector2i size(iIm->mWidth, iIm->mHeight);
//        GLenum internalFormat = GL_SRGB8_ALPHA8;
//        GLenum format = GL_RGBA;
//        GLenum datatype = GL_UNSIGNED_BYTE;
//        
//        switch(iIm->mNumberOfChannels)
//        {
//            case 1:
//                internalFormat = GL_R8;
//                format = GL_RED;
//                break;
//            case 2:
//                internalFormat = GL_RG8;
//                format = GL_RG;
//                break;
//            case 3:
//                //internalFormat = GL_SRGB8;
//                internalFormat = GL_RGB8;
//                format = GL_RGB;
//                break;
//            case 4:
//                //internalFormat = GL_SRGB8_ALPHA8;
//                internalFormat = GL_RGBA8;
//                format = GL_RGBA;
//                break;
//            default: break;
//        }
//
//        t.set(iIm->mpPayload, size, internalFormat, format, datatype);
//        t.generateMipmap(true);
//        t.setMagnificationFilter(GL_LINEAR);
//        t.setMinificationFilter(GL_LINEAR_MIPMAP_LINEAR);
//        
//        mImageIdToTexture.insert( make_pair(def->getId(), t) );
//    }
//}

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
    
    // delete all boundingboxes
    {
        auto it = mDefinitionIdToBoundingBox.begin();
        for(; it != mDefinitionIdToBoundingBox.end(); ++it)
        {
            delete it->second;
        }
    }

    // delete all representation
    {
        auto it = mDefinitionIdToRepresentation.begin();
        for(; it != mDefinitionIdToRepresentation.end(); ++it)
        {
            delete it->second;
        }
    }
    
    mIdToDefinition.clear();
    mDefinitionIdToBoundingBox.clear();
    mDefinitionIdToRepresentation.clear();
    mImageIdToTexture.clear();
    
    mNeedsRepresentationCreation.clear();
    mNeedsBBoxRepresentationCreation.clear();
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

    iCurrentCount += (int)ipCurrentNode->mChilds.size();
    for(size_t i = 0; i < ipCurrentNode->mChilds.size(); ++i)
    {
        countChilds(ipCurrentNode->mChilds[i], iCurrentCount);
    }
}

//------------------------------------------------------------------------------
void Scene::createBoundingBoxRepresentation(IGraphicNode* ipNode)
{
    const IDefinition *def = dynamic_cast<const IDefinition*>(ipNode);

    auto repIt = mDefinitionIdToBoundingBox.find(def->getId());
    if(repIt == mDefinitionIdToBoundingBox.end())
    {
        Representations::BoundingBox *bbox = new Representations::BoundingBox;
        bbox->create(ipNode);
        mDefinitionIdToBoundingBox.insert( make_pair(def->getId(), bbox) );   
    }
}

//------------------------------------------------------------------------------
void Scene::createModelRepresentation(ModelNode *iNode)
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
            //            have been uploaded to gpu and properly created!
            //          
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

                        if (gNumberOfFileStreamerPostOnThisFrame < gMaxFileStreamerPostPerFrame)
                        {
                            // image is not on gpu, lets ask filestreamer to
                            // load it
                            //
                            FileStreamer& fs = getHub().getFileStreamer();
                            FileStreamer::Request *r = new FileStreamer::Request(this);
                            r->mRequestType = FileStreamer::rtLoadRgbImage;
                            r->mFilenamePath = im->mFilenamePath;
                            r->mAffectedDefinitionId = im->getId();
                            fs.postRequest(r);
                            gNumberOfFileStreamerPostOnThisFrame++;
                        }
                    }
                    else
                    {
                        //check that the fence has been signaled... else wait some more
                        realisim::treeD::Texture t = it->second;
                        canCreateRepresentation = t.isFenceSignaled();
                        if (canCreateRepresentation)
                        {                            
                            t.deleteFenceSync();
                        }
                        else
                        { printf("----------------the fence was not signaled!!!\n"); }
                    }
                }
            }

            if(canCreateRepresentation)
            {

                if (gNumberOfGpuStreamerPostOnThisFrame < gMaxGpuStreamerPostPerFrame)
                {
                    // send a request to the gpuStreamer to create the model
                    GpuStreamer &gs = getHub().getGpuStreamer();
                    GpuStreamer::Message *m = new GpuStreamer::Message(this);
                    m->mMessageType = GpuStreamer::mtCreateModel;
                    m->mAffectedDefinitionId = def->getId();
                    m->mpModelNode = iNode;
                    m->mpImageIdToTexture = &mImageIdToTexture;
                    gs.postMessage(m);

                    gNumberOfGpuStreamerPostOnThisFrame++;
                }

                /*Representations::Model *model = new Representations::Model;
                model->create(iNode, mImageIdToTexture);
                mDefinitionIdToRepresentation.insert( make_pair(def->getId(), model) );*/
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
    spf.mNumberOfIGraphicNodeVisited++;

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
            Representations::Representation *rep = itModel->second;

            if(rep->isFenceSignaled())
            {
                if(mn->isVisible())
                { 
                    spf.mNumberOfModelDisplayed++;
                    ipCurrentLayer->push_back( rep );
                }
                rep->deleteFenceSync();
            }
            else
            {
                printf("-----------Representations::Model fence was not yet signaled...\n");
            }
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


    // add bounding box to draw list
    if (iNode->isBoundingBoxVisible())
    {
        IDefinition *def = dynamic_cast<IDefinition*>(iNode);
        if (def)
        {
            auto itBBox = mDefinitionIdToBoundingBox.find(def->getId());
            if(itBBox != mDefinitionIdToBoundingBox.end()) 
            { 
                spf.mNumberOfModelDisplayed++;
                ipCurrentLayer->push_back( itBBox->second );
            }
            else
            { mNeedsBBoxRepresentationCreation.push_back(iNode); }
        }
        
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
        //addToTextureLibrary(im);
        //im->unload();
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

        //addToTextureLibrary(im);
        //im->unload();
        //assert(false && "image was not loaded or definition was not found...");
    }
    
    //time to upload image to gpu... lets ask GpuStreamer
    GpuStreamer& gs = mpHub->getGpuStreamer();
    GpuStreamer::Message *m = new GpuStreamer::Message(this);
    m->mMessageType = GpuStreamer::mtTexture;
    m->mAffectedDefinitionId = im->getId();
    m->mpData = im;  // this is where we need COPY_ON_WRITE!!!!
    gs.postMessage(m);

    delete loadedIm;
}

//----------------------------------------
void Scene::processGpuStreammingDoneMessage(MessageQueue::Message* ipMessage)
{
    // add the texture to the texture library.
    GpuStreamer::Message *message = (GpuStreamer::Message *)ipMessage;
    //printf("Scene::processGpuStreammingDoneMessage \n" );

    unsigned int defId = message->mAffectedDefinitionId;

    switch (message->mMessageType)
    {

    case GpuStreamer::mtTexture:
    {        
        Image *loadedIm = (Image*)message->mpData;

        auto it = mImageIdToTexture.find(defId);
        if(it == mImageIdToTexture.end())
        {
            mImageIdToTexture.insert( make_pair(defId, message->mTexture) );
        }

        loadedIm->unload();
    }break;

    case GpuStreamer::mtCreateModel:
    {
        Representations::Model *model = (Representations::Model *)message->mpData;
        mDefinitionIdToRepresentation.insert(make_pair(defId, model));

        //check for insertion... if it fails... delete it? or move that
        //check to somewhere better...

    }break;

    default: break;
    }
    
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
    mGpuStreamingDoneQueue.processMessages();
    
    for(size_t i = 0; i < mNeedsTransformUpdate.size(); ++i)
    {
        updateTransform( mNeedsTransformUpdate[i] );
        updateBoundingBoxes( mNeedsTransformUpdate[i] );
        updateLod(mNeedsTransformUpdate[i]);
    }
    mNeedsTransformUpdate.clear();
    
    for(size_t i = 0; i < mNeedsRepresentationCreation.size(); ++i)
    { createModelRepresentation(mNeedsRepresentationCreation[i]); }
    mNeedsRepresentationCreation.clear();
    
    for(size_t i = 0; i < mNeedsBBoxRepresentationCreation.size(); ++i)
    { createBoundingBoxRepresentation(mNeedsBBoxRepresentationCreation[i]); }
    mNeedsBBoxRepresentationCreation.clear();

    //clear all draw list...
    mDefaultLayer.clear();
    mLayers.clear();

    gNumberOfFileStreamerPostOnThisFrame = 0;
    gNumberOfGpuStreamerPostOnThisFrame = 0;

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

    ipNode->updateBoundingBoxes();
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
    //IRenderable *parent = ipNode->getFirstParent<IRenderable>();
    IGraphicNode *parent = ipNode->mpParent;
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
    if (ipNode->mIsTransformDirty)
    {
        ipNode->mWorldTransform = iParentWorldTransform * ipNode->mParentTransform;
        iParentWorldTransform = ipNode->mWorldTransform;
        ipNode->mIsTransformDirty = false;
    }

    for(size_t i = 0; i < ipNode->mChilds.size(); ++i)
    { updateTransform(ipNode->mChilds[i], iParentWorldTransform); }
}
