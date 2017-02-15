
#pragma once

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
    class Representation;
    class Model;
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
    Image* findImage(const std::string& iFilenamePath);
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
    class Filter
    {
    public:
        Filter();
        ~Filter();
        
        void addChild(IRenderable*);
        Filter* find(IRenderable*);
        
        IRenderable *mpData;
        Filter *mpParent;
        std::vector<Filter*> mChilds;
    };

    
void addToTextureLibrary(Image*); //meuh! que faire avec ça!!!
    void createRepresentations(ModelNode*);
//    void checkAndCreateRepresentation(ModelNode*);
    Image* findImage(const std::string& iFilenamePath, IGraphicNode* ipNode);
    void filterRenderables(IGraphicNode*);
    void performCulling(IGraphicNode*);
    void prepareFrame(IGraphicNode*, std::vector<Representations::Representation*> *ipCurrentLayer );
    void processFileLoadingDoneMessage(MessageQueue::Message*);
    void updateTransform(Filter*);
    
    //data
    Hub *mpHub;
    IGraphicNode *mpRoot;
    MessageQueue mFileLoadingDoneQueue;
    
    std::unordered_map<unsigned int, Representations::Representation*> mDefinitionIdToRepresentation;
    std::unordered_map<unsigned int, realisim::treeD::Texture> mImageIdToTexture;
    
    std::vector<ModelNode*> mNeedsRepresentationCreation;
    std::vector<IGraphicNode*> mNeedsTransformUpdate;
    
    Filter mRenderableFilter;
};