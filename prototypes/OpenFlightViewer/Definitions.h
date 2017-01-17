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
class IGraphicNode
{
public:
    IGraphicNode();
    virtual ~IGraphicNode() = 0;
    
    virtual void addChild(IGraphicNode*);
    template<typename T> T* getFirstParent();
    
    enum nodeType{ ntUndefined, ntGroup, ntLibrary, ntLod, ntModel, ntOpenFlight, ntSwitch};
    
    IGraphicNode* mpParent;
    std::vector<IGraphicNode*> mChilds; //owned
    
    nodeType mNodeType;
    std::string mName;
};

//--------------------------------------------------------
template<typename T>
T* IGraphicNode::getFirstParent()
{
    IGraphicNode* p = mpParent;
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
//--- IDefinitions
//--------------------------------------------------------
class IDefinition
{
public:
    IDefinition();
    IDefinition(const IDefinition&) = delete;
    IDefinition& operator=(const IDefinition&) = delete;
    virtual ~IDefinition();

    static unsigned int mIdCounter;
    unsigned int mId;
};

//--------------------------------------------------------
//--- VertexPool
//--------------------------------------------------------
class VertexPool : public IDefinition
{
public:
    VertexPool() : IDefinition() {}
    //std::vector<OpenFlight::Vertex> mPool;
    std::vector<math::Vector3d> mVertices;
    std::vector<math::Vector3d> mNormals;
    std::vector<math::Vector2d> mTextureCoordinates;
    std::vector<QColor> mColors;
};

//--------------------------------------------------------
class Image : public IDefinition
{
public:
    Image() : IDefinition(), mWidth(0), mHeight(0),
    mNumberOfChannels(0),
    mBitsPerChannel(0),
    mSizeInBytes(0),
    mpPayload(nullptr)
    {}
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
    virtual ~Image() override;
    
//void clear();
    bool isLoaded() const {return mpPayload != nullptr; };
    void loadMetaData();
    void load();
    void unload();
    
    std::string mFilenamePath;
    int mWidth;
    int mHeight;
    int mNumberOfChannels;
    int mBitsPerChannel;
    int mSizeInBytes;
    unsigned char* mpPayload;
};

//--------------------------------------------------------
class Material : public IDefinition
{
public:
    Material() : IDefinition(), mAmbient(0.25 * 255, 0.25 * 255, 0.25 * 255),
        mDiffuse(0.5 * 255, 0.5 * 255, 0.5 * 255),
        mSpecular(0.6 * 255, 0.6 * 255, 0.6 * 255),
        mEmissive(1.0 * 255, 1.0 * 255, 1.0 * 255),
        mShinniness(80),
        mpImage(nullptr)
       {}
    
    QColor mAmbient;
    QColor mDiffuse;
    QColor mSpecular;
    QColor mEmissive;
    double mShinniness;

    Image* mpImage;
};

//--------------------------------------------------------
class Face
{
public:
    Face() : mpVertexPool(nullptr), mpMaterial(nullptr) {}
    
    VertexPool* mpVertexPool; //not owned
    
    Material* mpMaterial; //owned
    std::vector<uint32_t> mVertexIndices;
};

//--------------------------------------------------------
class Mesh
{
public:
    Mesh() : mpVertexPool(nullptr), mpMaterial(nullptr) {}
    
    VertexPool* mpVertexPool; //not owned

    Material* mpMaterial; //owned
    std::vector<uint32_t> mVertexIndices;
};

//--------------------------------------------------------
class OpenFlightNode : public IDefinition,
public IGraphicNode
{
public:
    OpenFlightNode() : IDefinition(), IGraphicNode() { mNodeType = ntOpenFlight; }
    virtual ~OpenFlightNode() override;
};

//--------------------------------------------------------
class LibraryNode : public IDefinition,
public IGraphicNode
{
public:
    LibraryNode() : IDefinition(), IGraphicNode() { mNodeType = ntLibrary; }
    virtual ~LibraryNode() override;
    
    VertexPool *mpVertexPool; //owned
//    vector<Material>
    std::vector<Image*> mImages; //owned
    
    //shaders...
};
  

//--------------------------------------------------------
class ModelNode : public IDefinition,
public IGraphicNode,
public IRenderable
{
public:
    ModelNode() : IDefinition(), IGraphicNode(), IRenderable() { mNodeType = ntModel; }
    virtual ~ModelNode() override;
    
    std::vector<Mesh*> mMeshes; //owned
    std::vector<Face*> mFaces; //owned
};

//--------------------------------------------------------
class GroupNode : public IDefinition, public IGraphicNode, public IRenderable
{
public:
    GroupNode() : IDefinition(), IGraphicNode(), IRenderable() { mNodeType = ntGroup; }
    virtual ~GroupNode() override {};
};


//--------------------------------------------------------
//--- Utilitaires
//--------------------------------------------------------

LibraryNode* getLibraryFor(IGraphicNode*);