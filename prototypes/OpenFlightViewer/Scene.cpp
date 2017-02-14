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
    FileStreamer& fs = mpHub->getFileStreamer();
    using placeholders::_1;
    mFileLoadingDoneQueue.setProcessingFunction(
        std::bind( &Scene::processFileLoadingDoneMessage, this, _1));
    fs.registerDoneQueue(this, &mFileLoadingDoneQueue);
}

//----------------------------------------
Scene::~Scene()
{
    if(mpRoot)
    { delete mpRoot; }
    
    mDefinitionIdToRepresentation.clear();
}

//------------------------------------------------------------------------------
void Scene::addNode(IGraphicNode* iNode)
{
    if(mpRoot == nullptr)
    { mpRoot = new GroupNode; } //first dummy node...
    
    mpRoot->mChilds.push_back(iNode);
    
    filterRenderables(iNode);
    
//    appliquer les filtres sur interfaces ici afin de creer les sous
//    arbres...
//    
//    faire un methode getParent<T> qui retourne le premier parent de type T.
    
//mNeedsRepresentationCreation.push_back(iNode);
    mNeedsTransformUpdate.push_back(iNode);
}

//------------------------------------------------------------------------------
void Scene::addToTextureLibrary(Image *iIm)
{
    IDefinition *def = dynamic_cast<IDefinition*>(iIm);
    auto it = mImageIdToTexture.find(def->mId);
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
        t.setFilter(GL_LINEAR_MIPMAP_LINEAR);
        
        mImageIdToTexture.insert( make_pair(def->mId, t) );
    }
}

////------------------------------------------------------------------------------
//void Scene::checkAndCreateRepresentation(ModelNode *iNode)
//{
//    const IDefinition *def = dynamic_cast<const IDefinition*>(iNode);
//    auto repIt = mDefinitionIdToRepresentation.find(def->mId);
//    if(repIt == mDefinitionIdToRepresentation.end())
//    {
//        check if model can be created... meaning that images
//        have been uploaded to gpu...
//        if not, send a request to the fileStreamer to load the image.
//        
//        Representations::Model  *model = nullptr;
//        model = new Representations::Model(iNode, mImageIdToTexture);
//        mDefinitionIdToRepresentation.insert( make_pair(def->mId, model) );
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
    mDefinitionIdToRepresentation.clear();
    mImageIdToTexture.clear();
    
    mNeedsRepresentationCreation.clear();
    mNeedsTransformUpdate.clear();
    
    mRenderableFilter = Filter();
    
    mDefaultLayer.clear();
    mLayers.clear();
}

//------------------------------------------------------------------------------
void Scene::createRepresentations(ModelNode *iNode)
{
    const IDefinition *def = dynamic_cast<const IDefinition*>(iNode);
    auto repIt = mDefinitionIdToRepresentation.find(def->mId);
    if(repIt == mDefinitionIdToRepresentation.end())
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
                auto it = mImageIdToTexture.find(im->mId);
                if(it == mImageIdToTexture.end())
                {
                    canCreateRepresentation = false;
                    // image is not on gpu, lets ask filestreamer to
                    // load it
                    //
                    FileStreamer& fs = mpHub->getFileStreamer();
                    FileStreamer::Request *r = new FileStreamer::Request(this);
                    r->mRequestType = FileStreamer::rtLoadRgbImage;
                    r->mFilenamePath = im->mFilenamePath;
                    fs.postRequest(r);
                }
            }
        }
        
        if(canCreateRepresentation)
        {
            Representations::Model *model = nullptr;
            model = new Representations::Model(iNode, mImageIdToTexture);
            mDefinitionIdToRepresentation.insert( make_pair(def->mId, model) );
        }
    }
    
    
//    if(iNode != nullptr)
//    {
//        deque<IGraphicNode*> q;
//        q.push_back(iNode);
//        while(!q.empty())
//        {
//            IGraphicNode* n = q.front();
//            q.pop_front();
//            
//            for(int i = 0; i < n->mChilds.size(); ++i)
//            { q.push_back(n->mChilds[i]); }
//            
//            switch (n->mNodeType)
//            {
//                case IGraphicNode::ntGroup: break;
//                case IGraphicNode::ntModel:
//                {
//                    ModelNode* modelNode = dynamic_cast<ModelNode*>(n);
//                    checkAndCreateRepresentation(modelNode);
//                } break;
//                default: break;
//            }
//        }
//    }
}

//------------------------------------------------------------------------------
void Scene::filterRenderables(IGraphicNode* iNode)
{
    if(iNode != nullptr)
    {
        deque<IGraphicNode*> q;
        q.push_back(iNode);
        while(!q.empty())
        {
            IGraphicNode* n = q.front();
            q.pop_front();
            
            for(int i = 0; i < n->mChilds.size(); ++i)
            { q.push_back(n->mChilds[i]); }
     
            IRenderable *r = dynamic_cast<IRenderable*>(n);
            if(r)
            {
                IRenderable *p = n->getFirstParent<IRenderable>();
                if(p)
                {
                    Filter *f = mRenderableFilter.find(p);
                    f->addChild(r);
                }
                else
                {
                    mRenderableFilter.addChild( r );
                }
                    
            }
        }
    }
}

//------------------------------------------------------------------------------
Image* Scene::findImage(const std::string &iFilenamePath)
{
    return findImage(iFilenamePath, getRoot());
}

//------------------------------------------------------------------------------
Image* Scene::findImage(const std::string &iFilenamePath, IGraphicNode* ipNode)
{
    realisim::utils::Timer __t;
    Image* imageFound = nullptr;
    
    if(ipNode != nullptr)
    {
        deque<IGraphicNode*> q;
        q.push_back(ipNode);
        while(!q.empty() && imageFound == nullptr)
        {
            IGraphicNode* n = q.front();
            q.pop_front();
            
            for(int i = 0; i < n->mChilds.size(); ++i)
            { q.push_back(n->mChilds[i]); }
            
            switch (n->mNodeType)
            {
                case IGraphicNode::ntLibrary:
                {
                    LibraryNode *lib = dynamic_cast<LibraryNode*>(n);
                    assert(lib);
                    //load all images and create representation
                    for(size_t i = 0; i < lib->mImages.size(); ++i)
                    {
                        Image* im = lib->mImages[i];
                        if(im->mFilenamePath == iFilenamePath)
                        {
                            imageFound = im;
                        }
                    }
                } break;
                default: break;
            }
        }
    }
    
    printf("temps pour Scene::findImage: %.4f(sec)\n", __t.getElapsed());
    return imageFound;
}

//------------------------------------------------------------------------------
IGraphicNode* Scene::getRoot() const
{ return mpRoot; }

//------------------------------------------------------------------------------
void Scene::loadLibraries(IGraphicNode* iNode)
{
    realisim::utils::Timer __t;
    
    if(iNode != nullptr)
    {
        deque<IGraphicNode*> q;
        q.push_back(iNode);
        while(!q.empty())
        {
            IGraphicNode* n = q.front();
            q.pop_front();
            
            for(int i = 0; i < n->mChilds.size(); ++i)
            { q.push_back(n->mChilds[i]); }
            
            switch (n->mNodeType)
            {
                case IGraphicNode::ntLibrary:
                {
                    LibraryNode *lib = dynamic_cast<LibraryNode*>(n);
                    assert(lib);
                    //load all images and create representation
                    for(size_t i = 0; i < lib->mImages.size(); ++i)
                    {
                        Image* im = lib->mImages[i];
                        if( !im->isLoaded() )
                        {
                            //sdfsdfasdf
//                            FileStreamer& fs = mpHub->getFileStreamer();
//                            FileStreamer::Request *r = new FileStreamer::Request(this);
//                            r->mRequestType = FileStreamer::rtLoadRgbImage;
//                            r->mFilenamePath = im->mFilenamePath;
//                            fs.postRequest(r);
                            
//                            im->load();
//                            
//                            //
//                            addToTextureLibrary(im);
//                            
//                            im->unload();
                            
                            //--- save image to disk.
                            //QImage::Format f;
                            //if(im->mNumberOfChannels == 1)
                            //{ continue; }
                            //
                            //assert(im->mNumberOfChannels != 2);
                            //if(im->mNumberOfChannels == 3)
                            //    f = QImage::Format_RGB888;
                            //if(im->mNumberOfChannels == 4)
                            //    f = QImage::Format_RGBA8888;
                            //QImage qim(im->mpPayload,
                            //          im->mWidth,
                            //          im->mHeight,
                            //          f);

                            //QString name = "./testImage_" + QString::number(i) + ".png";
                            //qim.save(name, "PNG");
                        }
                    }
                } break;
                default: break;
            }
        }
    }
    
    printf("temps pour Scene::loadLibraries: %.4f(sec)\n", __t.getElapsed());
}

//----------------------------------------
void Scene::prepareFrame(IGraphicNode* iNode, std::vector<Representations::Representation*> *ipCurrentLayer)
{
    //depth First...
    if(iNode == nullptr) {return;}

    //perform culling

    //fill layers
    GroupNode* gn = dynamic_cast<GroupNode*>(iNode);
    ModelNode* mn = dynamic_cast<ModelNode*>(iNode);
    if (gn)
    {
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
    }

    if (mn)
    {
        auto itModel = mDefinitionIdToRepresentation.find(mn->mId);
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
    }

    // recurse on all childs
    for (size_t i = 0; i < iNode->mChilds.size(); ++i)
    {
        prepareFrame(  iNode->mChilds[i], ipCurrentLayer );
    }
}

//----------------------------------------
void Scene::performCulling(IGraphicNode* iNode)
{
    
}

//----------------------------------------
void Scene::processFileLoadingDoneMessage(MessageQueue::Message* ipMessage)
{
    FileStreamer::DoneRequest *dr = (FileStreamer::DoneRequest *)ipMessage;
    printf("Scene::processFileLoadingDoneMessage %s\n",
           dr->mFilenamePath.c_str() );
    
    RgbImageLoader *loadedIm = (RgbImageLoader*)dr->mpData;
    
    // fin the image that correspond to the filename because
    // ids have already been assigned...
    Image *im = findImage(dr->mFilenamePath);
    
    if(loadedIm)
    {
        im->mpPayload = loadedIm->giveOwnershipOfImageData();
        addToTextureLibrary(im);
        im->unload();
    }
    
    delete loadedIm;
}

//----------------------------------------
void Scene::update()
{
    //process fileStreamingDoneQueue
    mFileLoadingDoneQueue.processMessages();
    
    for(size_t i = 0; i < mNeedsTransformUpdate.size(); ++i)
    {
        updateTransform(&mRenderableFilter);
    }
    mNeedsTransformUpdate.clear();
    
    for(size_t i = 0; i < mNeedsRepresentationCreation.size(); ++i)
    {
//        loadLibraries(mNeedsRepresentationCreation[i]);
        createRepresentations(mNeedsRepresentationCreation[i]);
    }
    mNeedsRepresentationCreation.clear();
    
    //clear all draw list...
    mDefaultLayer.clear();
    mLayers.clear();
    prepareFrame(mpRoot, &mDefaultLayer);
}

//----------------------------------------
void Scene::updateTransform(Filter* iFilterNode)
{
    IRenderable* renderable = iFilterNode->mpData;
    IRenderable* parent = iFilterNode->mpParent ? iFilterNode->mpParent->mpData : nullptr;
    if(renderable && renderable->mIsTransformDirty)
    {
        Matrix4 parentWorldTransform = parent != nullptr ?
            parent->mWorldTransform : Matrix4();
        renderable->mWorldTransform = parentWorldTransform * renderable->mParentTransform;
        
        renderable->mIsTransformDirty = false;
        
        for(size_t i = 0; i < iFilterNode->mChilds.size(); ++i)
        { updateTransform(iFilterNode->mChilds[i]); }
    }
}


//---------------
//--- Filter
//---------------
Scene::Filter::Filter() : mpData(new GroupNode()),
mpParent(nullptr)
{}

//----------------------------------------
Scene::Filter::~Filter()
{
    for(size_t i = 0; i < mChilds.size(); ++i)
    {
        delete mChilds[i];
    }
    mChilds.clear();
}

//----------------------------------------
void Scene::Filter::addChild(IRenderable *iR)
{
    Filter *f = new Filter();
    f->mpData = iR;
    f->mpParent = this;
    mChilds.push_back(f);
}

//----------------------------------------
// try to find iR with a breadth first search
// starting on this.
Scene::Filter* Scene::Filter::find(IRenderable *iR)
{
    Filter* found = nullptr;
    deque<Filter*> q;
    q.push_back(this);
    while(!q.empty() && found == nullptr)
    {
        Filter* n = q.front();
        q.pop_front();
        
        for(int i = 0; i < n->mChilds.size(); ++i)
        { q.push_back(n->mChilds[i]); }
        
        if(n->mpData == iR)
        { found = n; }
    }
    
    assert(found != nullptr);
    return found;
}
