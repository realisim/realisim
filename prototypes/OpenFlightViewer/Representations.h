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
        Model() = delete;
        
        explicit Model(ModelNode*,
                       const std::unordered_map<unsigned int, realisim::treeD::Texture>& iTextureLibrary);
        
        virtual ~Model();
        void draw() override;
        
    protected:
        ModelNode* mpModel; //faut mettre des shared_ptr... mais bon c'est un proto...
        GLuint mDisplayList;
        const std::unordered_map<unsigned int, realisim::treeD::Texture>& mTextureLibrary;
        //std::vector<realisim::treeD::Texture> mTextures; multitexturing?
    };
}