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
    
    enum nodeType{ ntExternalRef, ntGroup, ntLevelOfDetail, ntLibrary, ntLod, 
        ntModel, ntOpenFlight, ntSwitch, ntUndefined};

    struct Path
    {
        Path() = delete;
        explicit Path(IGraphicNode*);
        Path(const Path&) = delete;
        Path& operator=(const Path&) = delete;
        ~Path() = default;

        std::vector<IGraphicNode*> mParents;
    };

    virtual void addChild(IGraphicNode*);
    int decrementUseCount();
    void incrementUseCount();
    const realisim::math::BB3d& getAABB() const {return mAxisAlignedBoundingBox;}
    const realisim::math::BB3d& getPositionnedAABB() const {return mPositionnedAxisAlignedBoundingBox;}
    template<typename T> T* getFirstParent();
    int getUseCount() const;
    bool isBoundingBoxVisible() const {return mIsBoundingBoxVisible;}
    bool isMarkedForDeletion() const {return mMarkedForDeletion;}
    void markedForDeletion(bool iV) {mMarkedForDeletion = iV;}
    void setAABB(const realisim::math::BB3d& iAABB) {mAxisAlignedBoundingBox = iAABB;}
    void setBoundingBoxVisible(bool iS) {mIsBoundingBoxVisible = iS;}
    void updateBoundingBoxes();

    IGraphicNode* mpParent;
    std::vector<IGraphicNode*> mChilds; //owned
    
    nodeType mNodeType;
    std::string mName;

    bool mIsTransformDirty;
    realisim::math::Matrix4 mParentTransform;
    realisim::math::Matrix4 mWorldTransform;    

protected:
    void incrementUseCount(IGraphicNode*, int);

    int mUseCount;
    bool mMarkedForDeletion;
    bool mIsBoundingBoxVisible;
    //    realisim::math::BB3d mOrientedBoundingBox;
    realisim::math::BB3d mAxisAlignedBoundingBox;
    realisim::math::BB3d mPositionnedAxisAlignedBoundingBox;
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
    IRenderable();
    virtual ~IRenderable() = 0;
    
    bool isVisible() const;
    void setAsVisible(bool);    

    protected:
        void setAsVisible(IGraphicNode*, bool);

        bool mIsVisible;    
};


//--------------------------------------------------------
//--- IDefinitions
//--------------------------------------------------------
// Note on instance:
//
//
class IDefinition
{
public:
    IDefinition();
    IDefinition(const IDefinition&) = delete;
    IDefinition& operator=(const IDefinition&) = delete;
    virtual ~IDefinition();

    unsigned int getId() const;
    unsigned int getInstantiatedFromId() const;
    void setAsInstanceOf(unsigned int);
    bool isInstantiated() const;

protected:
    static unsigned int mIdCounter;
    unsigned int mId;
    unsigned int mInstantiatedFromId;
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
    ModelNode();
    virtual ~ModelNode() override;
    
    void addFace(Face*);
    void setAsInstanceOf(ModelNode*);

    std::vector<Mesh*> mMeshes; //owned
    std::vector<Face*> mFaces; //owned

protected:
    ModelNode *mpInstancedFrom;    
};

//--------------------------------------------------------
class GroupNode : public IDefinition, public IGraphicNode, public IRenderable
{
public:
    GroupNode() : IDefinition(), IGraphicNode(), IRenderable(),
        mLayerIndex(0)
        { mNodeType = ntGroup; }
    virtual ~GroupNode() override {};

    bool isLayered() const { return getLayerIndex() != 0; }
    int getLayerIndex() const {return mLayerIndex;}
    void setLayerIndex(int i) {mLayerIndex = i;}

private:
    int mLayerIndex;
};


//--------------------------------------------------------
// Bon... on hérite de IRenderable parce qu'il faut la transform world pour
// faire le calcul du positionnedLodCenter... Techniquement, lod ne devrait pas
// etre un renderable... Decrait séparer Irenderable en deux? une partie pour
// pour le positionnement IPositionnable et IRenderable???
//
class LevelOfDetailNode : public IDefinition, public IGraphicNode, public IRenderable
{
public:
    LevelOfDetailNode();
    virtual ~LevelOfDetailNode() override {};

    double getSwitchInDistance() const {return mSitchInDistance;}
    double getSwitchOutDistance() const {return mSitchOutDistance;}
    const realisim::math::Point3d& getOriginalLodCenter() const {return mOriginalLodCenter;}
    const realisim::math::Point3d& getPositionnedLodCenter() const {return mPositionnedLodCenter;}

    void setSwitchInDistance(double iD) {mSitchInDistance = iD;}
    void setSwitchOutDistance(double iD) {mSitchOutDistance = iD;}
    void setOriginalLodCenter(const realisim::math::Point3d& iC) {mOriginalLodCenter = iC;}
    void setPositionnedLodCenter(const realisim::math::Point3d& iC) {mPositionnedLodCenter = iC;}

private:
    double mSitchInDistance;
    double mSitchOutDistance;
    realisim::math::Point3d mOriginalLodCenter;
    realisim::math::Point3d mPositionnedLodCenter;
};


//--------------------------------------------------------
//--- Utilitaires
//--------------------------------------------------------

LibraryNode* getLibraryFor(IGraphicNode*);