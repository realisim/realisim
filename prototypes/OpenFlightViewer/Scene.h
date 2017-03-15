
#pragma once

#include "Definitions.h"
#include "Hub.h"
#include "MessageQueue.h"
#include <unordered_map>
#include <vector>

class IDefinition;
class IGraphicNode;
class IRenderable;
class Image;
class ModelNode;
namespace Representations
{
    class BoundingBox;
    class Model;
    class Representation;
}

class Scene
{
public:
    explicit Scene(Hub* ipHub);
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    ~Scene();

    void addNode(IGraphicNode*);
    void clear();
    IGraphicNode* getRoot() const;
    void update();


    // ca serait plaisant d'avoir un genre d'iterateur
    // sur les nodes... au lieu de faire un parcour de
    //
    // graph a chaque fois...
    //
    // pour faire
    // for(it = begin(); it != end; ++it)
    // {
    //    blablabla
    // }

    std::vector<Representations::Representation*> mDefaultLayer;
    std::map<int, std::vector<Representations::Representation*> > mLayers;
    
protected:

    void addToDefinitionMap(IGraphicNode*);
void addToTextureLibrary(Image*); //meuh! que faire avec ça!!!
    int countChilds(const IGraphicNode*) const;
    void countChilds(const IGraphicNode*, int& iCurrentCount) const;
    void createBoundingBoxRepresentation(IGraphicNode*);
    void createModelRepresentation(ModelNode*);
    Hub& getHub() {return *mpHub;}
//    void checkAndCreateRepresentation(ModelNode*)
    IDefinition* findDefinition(unsigned int);
    //void filterRenderables(IGraphicNode*);
    void performCulling(IGraphicNode*);
    void prepareFrame(IGraphicNode*, std::vector<Representations::Representation*> *ipCurrentLayer );
    void processFileLoadingDoneMessage(MessageQueue::Message*);
    void processGpuStreammingDoneMessage(MessageQueue::Message*);
    void updateBoundingBoxes(IGraphicNode*);
    void updateLod(IGraphicNode*);
    void updateTransform(IGraphicNode*);
    void updateTransform(IGraphicNode*, math::Matrix4 iParentTransform);
    
    //data
    Hub *mpHub;
    IGraphicNode *mpRoot;
    MessageQueue mFileLoadingDoneQueue;
    MessageQueue mGpuStreamingDoneQueue;
    
    std::unordered_map<unsigned int, IDefinition*> mIdToDefinition;
    std::unordered_map<unsigned int, Representations::BoundingBox*> mDefinitionIdToBoundingBox;
    std::unordered_map<unsigned int, Representations::Representation*> mDefinitionIdToRepresentation;
    std::unordered_map<unsigned int, realisim::treeD::Texture> mImageIdToTexture;
    std::set<unsigned int> mLoadTextureRequests;
    std::set<unsigned int> mLoadModelRequests;
    
    std::vector<ModelNode*> mNeedsRepresentationCreation;
    std::vector<IGraphicNode*> mNeedsBBoxRepresentationCreation;
    std::vector<IGraphicNode*> mNeedsTransformUpdate;
};