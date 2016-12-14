#pragma once

#include "3d/openGlHeaders.h"
#include "Definitions.h"
//using namespace realisim;

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
        
        explicit Model(ModelNode*);
        
        virtual ~Model();
        void draw() override;
        
    protected:
        ModelNode* mpModel; //faut mettre des shared_ptr... mais bon c'est un proto...
        GLuint mDisplayList;
    };
}