#include "Definitions.h"
#include "Scene.h"
#include "math/Matrix4.h"

using namespace realisim;
    using namespace math;

Scene::Scene() : mpRoot(new Node())
{
    
}

//----------------------------------------
Scene::~Scene()
{
    if(mpRoot)
    { delete mpRoot; }
}

//----------------------------------------
void Scene::update()
{
    updateTransform(mpRoot);
}

//----------------------------------------
void Scene::updateTransform(Node* iNode)
{
    if(iNode && iNode->mIsTransformDirty)
    {
        Matrix4 parentWorldTransform = iNode->mpParent != nullptr ?
            iNode->mpParent->mWorldTransform : Matrix4();
        iNode->mWorldTransform = parentWorldTransform * iNode->mParentTransform;
        
        iNode->mIsTransformDirty = false;
        
        for(size_t i = 0; i < iNode->mChilds.size(); ++i)
        { updateTransform(iNode->mChilds[i]); }
    }
}



