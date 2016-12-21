
#pragma once

#include <unordered_map>
#include <vector>

class Definition;
class IRenderable;
namespace Representations { class Representation; }

class Scene
{
public:
    Scene();
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    ~Scene();

    void addNode(Definition*);
    void clear();
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

    std::vector<Representations::Representation*> mToDraw;
    
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

    
    void createRepresentations(Definition*);
    void loadLibraries(Definition*);
    void filterRenderables(Definition*);
    void performCulling(Definition*);
    void updateTransform(Filter*);
    
    //data
    Definition* mpRoot;
    
    std::unordered_map<unsigned int, Representations::Representation*> mDefinitionIdToRepresentation;
    
    std::vector<Definition*> mNeedsRepresentationCreation;
    std::vector<Definition*> mNeedsTransformUpdate;
    
    Filter mRenderableFilter;
};