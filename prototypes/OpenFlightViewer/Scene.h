
#pragma once

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
    Scene();
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    ~Scene();

    void addNode(IGraphicNode*);
    void clear();
    void update();
    IGraphicNode* getRoot() const;

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
    void createRepresentations(IGraphicNode*);
    Representations::Model* checkAndCreateRepresentation(ModelNode*);
    void loadLibraries(IGraphicNode*);
    void filterRenderables(IGraphicNode*);
    void performCulling(IGraphicNode*);
    void prepareFrame(IGraphicNode*, std::vector<Representations::Representation*> *ipCurrentLayer );
    void updateTransform(Filter*);
    
    //data
    IGraphicNode* mpRoot;
    
    std::unordered_map<unsigned int, Representations::Representation*> mDefinitionIdToRepresentation;
    std::unordered_map<unsigned int, realisim::treeD::Texture> mImageIdToTexture;
    
    std::vector<IGraphicNode*> mNeedsRepresentationCreation;
    std::vector<IGraphicNode*> mNeedsTransformUpdate;
    
    Filter mRenderableFilter;
};