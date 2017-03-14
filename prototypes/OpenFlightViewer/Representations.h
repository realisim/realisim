#pragma once

#include "3d/openGlHeaders.h"
#include "3d/Texture.h"
#include "Definitions.h"
#include <unordered_map>

namespace Representations
{
    //-------------------------------------------------------------------------
    class Representation
    {
    public:
        Representation();
        Representation(const Representation&) = delete;
        Representation& operator=(const Representation&) = delete;
        virtual ~Representation();
        
        void deleteFenceSync();
        virtual void draw();
        GLsync fenceSync() const;
        bool hasFenceSync() const;
        bool isFenceSignaled() const;
        void setFenceSync(GLsync);


    protected:
        GLsync mFenceSync;
    };
    
    //-------------------------------------------------------------------------
    class BoundingBox : public Representation
    {
    public:
        BoundingBox() = default;
        virtual ~BoundingBox() = default;

        void create(IGraphicNode*);
        virtual void draw() override;

    protected:
        IGraphicNode* mpGraphicNode;
    };

    //-------------------------------------------------------------------------
    class Model : public Representation
    {
    public:
        Model();               
        virtual ~Model();

        void create(ModelNode*,
            const std::unordered_map<unsigned int, realisim::treeD::Texture>& iTextureLibrary);
        void createInstance(ModelNode*, Model*);
        void draw() override;
        
        

    protected:
        ModelNode* mpModelNode; //faut mettre des shared_ptr... mais bon c'est un proto...
        GLuint mDisplayList;
         
        //std::vector<realisim::treeD::Texture> mTextures; multitexturing?
    };

}