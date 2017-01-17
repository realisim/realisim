
#include "Definitions.h"
#include <deque>
#include "FltImporter.h"
#include "math/Matrix4.h"

using namespace std;
using namespace realisim;
    using namespace math;

namespace
{
    Matrix4 toMatrix4( OpenFlight::MatrixRecord* ipM )
    {
        double md[4][4];
        for(int row = 0; row < 4; ++row)
            for(int col = 0; col < 4; ++col)
            { md[row][col] = ipM->getMatrix().mData[row][col]; }
        
        bool isRowMajor = ipM->getMatrix().mInternalStorage == OpenFlight::Matrix4f::isRowMajor;
        return Matrix4(md[0], isRowMajor);
    }
}

FltImporter::FltImporter(OpenFlight::HeaderRecord* ipHr) :
mpHeaderRecord(ipHr),
mpGraphicNodeRoot(nullptr)
{
    parseFltTree(ipHr, nullptr);
}

FltImporter::~FltImporter()
{
    if(mpHeaderRecord)
    {delete mpHeaderRecord;}
}

//-----------------------------------------------------------------------------
// In our data tree, we do not care much about external references but they
// can carry a transform matrix which needs to be be brought in. To do so,
// we create a group to hold that transform.
//
// This is somehow crappy, we should store the matrix as a carryover and
// apply it to the first node than can bear it (Group or Object)
//
// This would reduce the lenght of the tree.
//
GroupNode* FltImporter::digData(OpenFlight::ExternalReferenceRecord* ipE,
                                IGraphicNode* ipParent)
{
    using namespace OpenFlight;
    
    GroupNode *pGroup = new GroupNode();
    
    pGroup->mName = "external reference";
    
    pGroup->mParentTransform = fetchTransform(ipE);
    ipParent->addChild(pGroup);
    
    return pGroup;
}

//-----------------------------------------------------------------------------
GroupNode* FltImporter::digData(OpenFlight::GroupRecord* ipG,
                                IGraphicNode* ipParent)
{
    using namespace OpenFlight;

    printf("GroupName: %s, GroupRecord priority: %d, layer code: %d, significance: %d\n", 
        ipG->getAsciiId().c_str(),
        ipG->getRelativePriority(),
        ipG->getLayerCode(),
        ipG->getSignificance() );

    GroupNode *pGroup = new GroupNode();
    pGroup->mName = ipG->hasLongIdRecord() ? ipG->getLongIdRecord()->getAsciiId() : ipG->getAsciiId();
    pGroup->mParentTransform = fetchTransform(ipG);
    
    ipParent->addChild(pGroup);
    
    return pGroup;
}

//-----------------------------------------------------------------------------
LibraryNode* FltImporter::digData(OpenFlight::HeaderRecord* ipH,
                                  IGraphicNode* ipParent)
{
    using namespace OpenFlight;
    
    // The headerRecord is usually the first node of the openflight
    // data structure. If there is no root yet, then it is the first
    // node of the data structure. HeaderNode can be encountered
    // multiple times in an openFlight tree, but they will have
    // an ExternalRefence parent.

    OpenFlightNode* opfNode = new OpenFlightNode();
    opfNode->mName = ipH->hasLongIdRecord() ? ipH->getLongIdRecord()->getAsciiId() : ipH->getAsciiId();

    if( ipParent == nullptr )
    {
        mpGraphicNodeRoot = opfNode;
    }
    else
    { ipParent->addChild(opfNode); }
        
    //store the relation between OpenFlightNode and headerNode
    //mDefinitionIdToFltRecord.insert( make_pair(mpGraphicNodeRoot->mId, ipH) );
    
    //add header stuff here
    
    //add library node to store all palette information
    //add libray node
    LibraryNode *library = new LibraryNode();
    library->mName = "Library";
    opfNode->addChild(library);

    // check external reference flags, to properly populate
    // the library node with approate palette since, external
    // reference can override (or not) palettes.
    bool useParentDatabaseColorPalette = false;
    bool useParentDatabaseMaterialPalette = false;
    bool useParentDatabaseTexturePalette = false;
    if(ipH->isExternalReference())
    {
        //get external reference parent to check override flags
        ExternalReferenceRecord *extRef = (ExternalReferenceRecord *)ipH->getParent();
        useParentDatabaseColorPalette = extRef->hasFlag(ExternalReferenceRecord::fColorPaletteOverride);
        useParentDatabaseMaterialPalette = extRef->hasFlag(ExternalReferenceRecord::fMaterialPaletteOverride);
        useParentDatabaseTexturePalette = extRef->hasFlag(ExternalReferenceRecord::fTextureAndMappingOverride);
    }
    
    // collect from palettes
    //--- Vertex palette
    VertexPaletteRecord* vpr = ipH->getVertexPalette();
    if (vpr != nullptr)
    {
        VertexPool *vp = new VertexPool();
        for(int i = 0; i < vpr->getNumberOfVertices(); ++i)
        {
            const OpenFlight::Vertex& v = vpr->getVertex(i);
            vp->mVertices.push_back(math::Vector3d(v.mCoordinate.mX,
                                                    v.mCoordinate.mY,
                                                    v.mCoordinate.mZ ) );
                        
            vp->mNormals.push_back(math::Vector3d(v.mNormal.mX,
                                                    v.mNormal.mY,
                                                    v.mNormal.mZ ) );
                        
            vp->mTextureCoordinates.push_back(math::Vector2d(v.mTextureCoordinate.mX,
                                                    v.mTextureCoordinate.mY) );
                        
            vp->mColors.push_back(QColor(v.mPackedColor.mRed,
                                            v.mPackedColor.mGreen,
                                            v.mPackedColor.mBlue,
                                            v.mPackedColor.mAlpha));
        }
                    
        //store the relation between vertexPool and palette
        mDefinitionIdToFltRecord.insert( make_pair(vp->mId, vpr) );
                    
        library->mpVertexPool = vp;
    }

    //--- texture palettes
    for (int i = 0; i < ipH->getNumberOfTexturePalettes(); ++i)
    {
        TexturePaletteRecord* tpr = ipH->getTexturePalette(i);
        if (tpr != nullptr)
        {
            Image *image = new Image();
                        
            // the filename of the texture record must be relative, else,
            // it won't work...
            image->mFilenamePath = ipH->getFilePath() + tpr->getFilenamePath();
            image->loadMetaData();
                        
            mDefinitionIdToFltRecord.insert( make_pair(image->mId, tpr));
            library->mImages.push_back(image);
        }
    }
    
    return library;
}

//-----------------------------------------------------------------------------
ModelNode* FltImporter::digData(OpenFlight::ObjectRecord* ipR,
                                IGraphicNode* ipParent)
{
    using namespace OpenFlight;

    printf("Object Name: %s, ObjectRecord priority: %d\n", 
        ipR->getAsciiId().c_str(),
        ipR->getRelativePriority());

    ModelNode *model = new ModelNode();
    ipParent->addChild(model);
    
    model->mName = ipR->hasLongIdRecord() ? ipR->getLongIdRecord()->getAsciiId() : ipR->getAsciiId();
    model->mParentTransform = fetchTransform(ipR);
    
    //object record can only have 2 type of childs...
    // 1- Face
    // 2- light points.
    
    for(int i = 0; i < ipR->getNumberOfChilds(); ++i)
    {
        PrimaryRecord* c = ipR->getChild(i);
        
        switch (c->getOpCode())
        {
            case ocFace:
            {
                FaceRecord *faceRecord = (FaceRecord*)c;

                // only vertex and morph vertex has childs
                // could have subface (with pushSubface)
                // lets fetch the vertex list
                PrimaryRecord* vertexList = c->getNumberOfChilds() > 0 ? c->getChild(0) : nullptr;
                if(vertexList && vertexList->getOpCode() == ocVertexList)
                {
                    Face *face = new Face();
                    model->mFaces.push_back(face);
                    
                    LibraryNode *currentLibrary = getLibraryFor(model);
                    face->mpVertexPool = currentLibrary->mpVertexPool;
                    
                    //--- vertices
                    // fetch the VertexPalette from openFlight and resolve
                    // the byte offset into actual index into the vertex array.
                    VertexPaletteRecord *vpr =
                    (VertexPaletteRecord*)getRecordFromDefinitionId(face->mpVertexPool->mId);
                    
                    VertexListRecord *vlr = (VertexListRecord *)vertexList;
                    for(int i = 0; i < vlr->getNumberOfVertices(); ++i)
                    {
                        int vertexIndex = vpr->getIndexFromByteOffset( vlr->getByteOffsetIntoVertexPalette(i) );
                        assert( vertexIndex != -1 );
                        face->mVertexIndices.push_back( vertexIndex );
                    }
                    
                    //--- textures (images)
                    Material *material = new Material();
                    face->mpMaterial = material;
                    if(faceRecord->getTexturePatternIndex() >= 0)
                    {
                        material->mpImage = getImageFromTexturePatternIndex( faceRecord->getTexturePatternIndex(), currentLibrary );
                    }
                    
                    //--- material
                    if(faceRecord->getMaterialIndex() >= 0)
                    {
                        
                    }
                    
                }
            }break;
            case ocLightPoint: break;
            default: break;
        }
    }
    
    return model;
}

//-----------------------------------------------------------------------------
realisim::math::Matrix4 FltImporter::fetchTransform(OpenFlight::PrimaryRecord* ipR, bool *oFound)
{
    using namespace OpenFlight;
    
    bool found = false;
    realisim::math::Matrix4 m;
    //lets check acros the ancillary record
    for(int i = 0; i < ipR->getNumberOfAncillaryRecords() && !found; ++i)
    {
        AncillaryRecord* a = ipR->getAncillaryRecord(i);
        
        switch (a->getOpCode())
        {
            case OpenFlight::ocMatrix:
            {
                found = true;
                m = toMatrix4( ((MatrixRecord*)a) );
            }break;
            default: break;
        }
    }
    
    if(oFound != nullptr)
    { *oFound = found; }
    return m;
}

//-----------------------------------------------------------------------------
Image* FltImporter::getImageFromTexturePatternIndex(int iIndex, LibraryNode* iLibrary)
{
    using namespace OpenFlight;
    
    Image *r = nullptr;
    if( iLibrary != nullptr )
    {
        for( size_t i = 0; i < iLibrary->mImages.size() && r == nullptr; ++i)
        {
            auto it = mDefinitionIdToFltRecord.find(iLibrary->mImages[i]->mId);
            if(it != mDefinitionIdToFltRecord.end() &&
               ((TexturePaletteRecord*)it->second)->getTexturePatternIndex() == iIndex )
            {
                r = iLibrary->mImages[i];
            }
        }
    }
    return r;
}


//-----------------------------------------------------------------------------
OpenFlight::Record* FltImporter::getRecordFromDefinitionId(int iId)
{
    OpenFlight::Record* r = nullptr;
    auto it = mDefinitionIdToFltRecord.find(iId);
    if( it != mDefinitionIdToFltRecord.end() )
    {
        r = it->second;
    }
    return r;
}

//-----------------------------------------------------------------------------
void FltImporter::parseFltTree(OpenFlight::PrimaryRecord* ipRecord, IGraphicNode* ipCurrentParent)
{
    using namespace OpenFlight;

    //depth first parsing
    
    IGraphicNode *currentNode = nullptr;
    //dig for data on current record
    switch (ipRecord->getOpCode())
    {
        case OpenFlight::ocHeader:
            currentNode = digData( (HeaderRecord*)ipRecord, ipCurrentParent );
            break;
        case OpenFlight::ocExternalReference:
            currentNode = digData( (ExternalReferenceRecord*)ipRecord, ipCurrentParent );
            break;
        case OpenFlight::ocGroup:
            currentNode = digData( (GroupRecord*)ipRecord, ipCurrentParent );
            break;
        case OpenFlight::ocObject:
            currentNode = digData( (ObjectRecord*)ipRecord, ipCurrentParent );
            break;
        case OpenFlight::ocUnsupported:
        {
            //explanation...
            GroupNode *pGroup = new GroupNode();
            pGroup->mName = string("Unsupported openfligNode: ") + OpenFlight::toString(  ((UnsupportedRecord*)ipRecord)->getOriginalOpCode() );
            ipCurrentParent->addChild(pGroup);
            currentNode = pGroup;
        }break;
        default: break;
    }
    
    //dig into child
    for(int i = 0; i < ipRecord->getNumberOfChilds(); ++i)
    { parseFltTree(ipRecord->getChild(i), currentNode); }
}