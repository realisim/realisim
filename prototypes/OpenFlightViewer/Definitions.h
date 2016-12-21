#pragma once

#include "math/BoundingBox.h"
#include "math/Matrix4.h"
#include "math/Vect.h"
#include "openFlight/Types.h"
#include <QColor>
#include <vector>
#include <map>
#include <string>

using namespace realisim;

//--------------------------------------------------------
//--- Interfaces
//--------------------------------------------------------
//class IGraphicNode
//{
//public:
//    IGraphicNode();
//    virtual ~IGraphicNode() = 0;
//}


//--------------------------------------------------------
//--- Interfaces
//--------------------------------------------------------
class IRenderable
{
public:
    IRenderable() : mIsTransformDirty(true) {}
    virtual ~IRenderable() = 0;
    
    bool mIsTransformDirty;
    realisim::math::Matrix4 mParentTransform;
    realisim::math::Matrix4 mWorldTransform;
    
//    realisim::math::BB3d mOriginalBoundingBox;
//    realisim::math::BB3d mOrientedBoundingBox;
//    realisim::math::BB3d mAxisAlignedBoundingBox;
};


//--------------------------------------------------------
//--- Definitions
//--------------------------------------------------------
class Definition
{
public:
    Definition();
    Definition(const Definition&) = delete;
    Definition& operator=(const Definition&) = delete;
    virtual ~Definition();

    static unsigned int mIdCounter;
    unsigned int mId;
    
    virtual void addChild(Definition*);
    template<typename T> T* getParent();
    
    enum nodeType{ntDefinition, ntFace, ntGroup, ntImage, ntLibrary, ntMesh,
        ntMaterial,
        ntModel,
        ntOpenFlight,
        ntVertexPool};
    
    Definition* mpParent;
    std::vector<Definition*> mChilds; //owned
    
    nodeType mNodeType;
};

//--------------------------------------------------------
template<typename T>
T* Definition::getParent()
{
    Definition* p = mpParent;
    T* found = nullptr;
    while(p != nullptr && found == nullptr)
    {
        T* cast = dynamic_cast<T*>(p);
        if(cast){ found = cast; }
        p = p->mpParent;
    }
    return found;
}

//--------------------------------------------------------
//--- VertexPoolNode
//--------------------------------------------------------
class VertexPoolNode : public Definition
{
public:
    VertexPoolNode() : Definition() { mNodeType = ntVertexPool; }
    //std::vector<OpenFlight::Vertex> mPool;
    std::vector<math::Vector3d> mVertices;
    std::vector<math::Vector3d> mNormals;
    std::vector<math::Vector2d> mTextureCoordinates;
    std::vector<QColor> mColors;
};

//--------------------------------------------------------
class ImageNode : public Definition
{
public:
    ImageNode() : Definition(), mWidth(0), mHeight(0), mSizeInBytes(0), mpPayload(nullptr)
    { mNodeType = ntImage; }
    
    bool isLoaded() const {return mpPayload != nullptr; };
    
    std::string mFilenamePath;
    int mWidth;
    int mHeight;
    int mSizeInBytes;
    unsigned char* mpPayload;
};

//--------------------------------------------------------
class MaterialNode : public Definition
{
public:
    MaterialNode() : mAmbient(0.25 * 255, 0.25 * 255, 0.25 * 255),
        mDiffuse(0.5 * 255, 0.5 * 255, 0.5 * 255),
        mSpecular(0.6 * 255, 0.6 * 255, 0.6 * 255),
        mEmissive(1.0 * 255, 1.0 * 255, 1.0 * 255),
        mShinniness(80),
        mpImage(nullptr)
       { mNodeType = ntMaterial; }
    
    virtual void addChild(Definition*) override;
    
    QColor mAmbient;
    QColor mDiffuse;
    QColor mSpecular;
    QColor mEmissive;
    double mShinniness;

    ImageNode* mpImage;
};

//--------------------------------------------------------
class FaceNode : public Definition
{
public:
    FaceNode() : mpVertexPool(nullptr), mpMaterial(nullptr) { mNodeType = ntFace; }
    
    virtual void addChild(Definition*) override;
    
    VertexPoolNode* mpVertexPool; //not owned
    
    MaterialNode* mpMaterial; //owned
    std::vector<uint32_t> mVertexIndices;
};

//--------------------------------------------------------
class MeshNode : public Definition
{
public:
    MeshNode() : mpVertexPool(nullptr), mpMaterial(nullptr) { mNodeType = ntMesh; }

    virtual void addChild(Definition*) override;
    
    VertexPoolNode* mpVertexPool; //not owned

    MaterialNode* mpMaterial; //owned
    std::vector<uint32_t> mVertexIndices;
};

//--------------------------------------------------------
class OpenFlightNode : public Definition
{
public:
    OpenFlightNode() : Definition() { mNodeType = ntOpenFlight; }
    virtual ~OpenFlightNode();
};

//--------------------------------------------------------
class LibraryNode : public Definition
{
public:
    LibraryNode() : Definition() { mNodeType = ntLibrary; }
    virtual ~LibraryNode();
    
    virtual void addChild(Definition*) override;
    
    VertexPoolNode *mpVertexPool; //owned
//    vector<Material>
    std::vector<ImageNode*> mImages; //owned
    
    //shaders...
};
  

//--------------------------------------------------------
class ModelNode : public Definition, public IRenderable
{
public:
    ModelNode() : Definition(), IRenderable() { mNodeType = ntModel; }
    virtual ~ModelNode() override;
    
    virtual void addChild(Definition*) override;
    
    std::vector<MeshNode*> mMeshes; //owned
    std::vector<FaceNode*> mFaces; //owned
};

//--------------------------------------------------------
class GroupNode : public Definition, public IRenderable
{
public:
    GroupNode() : Definition() { mNodeType = ntGroup; }
    virtual ~GroupNode() override;
};


//--------------------------------------------------------
//--- Utilitaires
//--------------------------------------------------------

LibraryNode* getLibraryFor(Definition*);