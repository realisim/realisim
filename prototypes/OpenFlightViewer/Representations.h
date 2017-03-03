#pragma once

#include "3d/openGlHeaders.h"
#include "3d/Texture.h"
#include "Definitions.h"
#include <unordered_map>

namespace Representations
{
    class Representation
    {
    public:
        Representation();
        virtual ~Representation();
        
        virtual void draw();
    };
    
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