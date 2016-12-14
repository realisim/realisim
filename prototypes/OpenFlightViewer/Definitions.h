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
class Definition
{
public:
    Definition();
    Definition(const Definition&) = delete;
    Definition& operator=(const Definition&) = delete;
    virtual ~Definition();

    static unsigned int mIdCounter;
    unsigned int mId;
};

//--------------------------------------------------------
class VertexPool : public Definition
{
public:
    VertexPool() : Definition() {}
    //std::vector<OpenFlight::Vertex> mPool;
    std::vector<math::Vector3d> mVertices;
    std::vector<math::Vector3d> mNormals;
    std::vector<math::Vector2d> mTextureCoordinates;
    std::vector<QColor> mColors;
};

//--------------------------------------------------------
class Image : public Definition
{
public:
    Image() : mWidth(0), mHeight(0), mSizeInBytes(0), mpPayload(nullptr) {}
    
    bool isLoaded() const {return mpPayload != nullptr; };
    
    std::string mFilenamePath;
    int mWidth;
    int mHeight;
    int mSizeInBytes;
    unsigned char* mpPayload;
};

//--------------------------------------------------------
class Material : public Definition
{
public:
    Material() : mAmbient(0.25 * 255, 0.25 * 255, 0.25 * 255),
        mDiffuse(0.5 * 255, 0.5 * 255, 0.5 * 255),
        mSpecular(0.6 * 255, 0.6 * 255, 0.6 * 255),
        mEmissive(1.0 * 255, 1.0 * 255, 1.0 * 255),
        mShinniness(80),
        mpImage(nullptr) {}
    
    QColor mAmbient;
    QColor mDiffuse;
    QColor mSpecular;
    QColor mEmissive;
    double mShinniness;

    Image* mpImage;
};

//--------------------------------------------------------
class Face : public Definition
{
public:
    Face() : mpVertexPool(nullptr), mpMaterial(nullptr) {}
    
    VertexPool* mpVertexPool; //not owned
    
    Material* mpMaterial; //owned
    std::vector<uint32_t> mVertexIndices;
};

//--------------------------------------------------------
class Mesh : public Definition
{
public:
    Mesh() : mpVertexPool(nullptr), mpMaterial(nullptr) {}
    
    VertexPool* mpVertexPool; //not owned

    Material* mpMaterial; //owned
    std::vector<uint32_t> mVertexIndices;
};

//--------------------------------------------------------
class Node : public Definition
{
public:
    Node() : mpParent(nullptr),
        mNodeType(ntNode), mIsTransformDirty(true),
        mIsBoundingBoxDirty(true) {}
    virtual ~Node();
    
    enum NodeType{ntGroup, ntLibrary, ntModel, ntNode, ntOpenFlight};
    Node* mpParent;
    std::vector<Node*> mChilds; //owned
    
    NodeType mNodeType;
    
    bool mIsTransformDirty;
    realisim::math::Matrix4 mWorldTransform;
    realisim::math::Matrix4 mParentTransform;
    
    bool mIsBoundingBoxDirty;
    realisim::math::BB3d mBoundingBox;
};

//--------------------------------------------------------
class OpenFlightNode : public Node
{
public:
    OpenFlightNode() : Node() { mNodeType = ntOpenFlight; }
    virtual ~OpenFlightNode();
};

//--------------------------------------------------------
class LibraryNode : public Node
{
public:
    LibraryNode() : Node() { mNodeType = ntLibrary; }
    virtual ~LibraryNode();
    
    VertexPool *mpVertexPool; //owned
//    vector<Material>
    std::vector<Image*> mImages; //owned
};
  

//--------------------------------------------------------
class ModelNode : public Node
{
public:
    ModelNode() : Node() { mNodeType = ntModel; }
    virtual ~ModelNode();

    std::vector<Mesh*> mMeshes; //owned
    std::vector<Face*> mFaces; //owned
};

//--------------------------------------------------------
class GroupNode : public Node
{
public:
    GroupNode() : Node() { mNodeType = ntGroup; }
    virtual ~GroupNode();
};


//--------------------------------------------------------
//--- Utilitaires
//--------------------------------------------------------

LibraryNode* getLibraryFor(Node*);