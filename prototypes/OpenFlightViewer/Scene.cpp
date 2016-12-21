#include "Definitions.h"
#include <deque>
#include "math/Matrix4.h"
#include "Representations.h"
#include "Scene.h"

using namespace realisim;
    using namespace math;

Scene::Scene() : mpRoot(nullptr)
{}

//----------------------------------------
Scene::~Scene()
{
    if(mpRoot)
    { delete mpRoot; }
    
    mDefinitionIdToRepresentation.clear();
}

//------------------------------------------------------------------------------
void Scene::addNode(Definition* iNode)
{
    if(mpRoot == nullptr)
    { mpRoot = new Definition; }
    
    mpRoot->mChilds.push_back(iNode);
    
    filterRenderables(iNode);
    
//    appliquer les filtres sur interfaces ici afin de creer les sous
//    arbres...
//    
//    faire un methode getParent<T> qui retourne le premier parent de type T.
    
    mNeedsRepresentationCreation.push_back(iNode);
    mNeedsTransformUpdate.push_back(iNode);
}

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
    
    mNeedsRepresentationCreation.clear();
    mNeedsTransformUpdate.clear();
    
    mRenderableFilter = Filter();
    
    mToDraw.clear();
}

//------------------------------------------------------------------------------
void Scene::createRepresentations(Definition *iNode)
{
    if(iNode != nullptr)
    {
        deque<const Definition*> q;
        q.push_back(iNode);
        while(!q.empty())
        {
            const Definition* n = q.front();
            q.pop_front();
            
            for(int i = 0; i < n->mChilds.size(); ++i)
            { q.push_back(n->mChilds[i]); }
            
            switch (n->mNodeType)
            {
                case Definition::ntGroup: break;
                case Definition::ntModel:
                {
                    auto repIt = mDefinitionIdToRepresentation.find(n->mId);
                    if(repIt == mDefinitionIdToRepresentation.end())
                    {
                        Representations::Model* m = new Representations::Model((ModelNode*)n);
                        repIt = mDefinitionIdToRepresentation.insert( make_pair(n->mId, m) ).first;
                    }
                    
                    //this should be moved in the culling phase
                    mToDraw.push_back( repIt->second );
                } break;
                default: break;
            }
        }
    }
}

//------------------------------------------------------------------------------
void Scene::filterRenderables(Definition* iNode)
{
    if(iNode != nullptr)
    {
        deque<Definition*> q;
        q.push_back(iNode);
        while(!q.empty())
        {
            Definition* n = q.front();
            q.pop_front();
            
            for(int i = 0; i < n->mChilds.size(); ++i)
            { q.push_back(n->mChilds[i]); }
     
            IRenderable *r = dynamic_cast<IRenderable*>(n);
            if(r)
            {
                IRenderable *p = n->getParent<IRenderable>();
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
void Scene::loadLibraries(Definition* iNode)
{
    if(iNode != nullptr)
    {
        deque<const Definition*> q;
        q.push_back(iNode);
        while(!q.empty())
        {
            const Definition* n = q.front();
            q.pop_front();
            
            for(int i = 0; i < n->mChilds.size(); ++i)
            { q.push_back(n->mChilds[i]); }
            
            switch (n->mNodeType)
            {
                case Definition::ntLibrary:
                {
                    LibraryNode *lib = (LibraryNode *)n;
                    //load all images and create representation
                    for(size_t i = 0; i < lib->mImages.size(); ++i)
                    {
                        
                    }
                } break;
                default: break;
            }
        }
    }
}

//----------------------------------------
void Scene::performCulling(Definition* iNode)
{
    
}

//----------------------------------------
void Scene::update()
{
    for(size_t i = 0; i < mNeedsTransformUpdate.size(); ++i)
    {
        updateTransform(&mRenderableFilter);
    }
    mNeedsTransformUpdate.clear();
    
    for(size_t i = 0; i < mNeedsRepresentationCreation.size(); ++i)
    {
        createRepresentations(mNeedsRepresentationCreation[i]);
    }
    mNeedsRepresentationCreation.clear();
    
    
//    performCulling(mpRoot);
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
