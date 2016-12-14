
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
mpDefinitionRoot(nullptr),
mpCurrentNode(nullptr)
{
    parseFltTree(ipHr);
}

FltImporter::~FltImporter()
{
    if(mpHeaderRecord)
    {delete mpHeaderRecord;}
}

//-----------------------------------------------------------------------------
void FltImporter::digData(OpenFlight::GroupRecord* ipG)
{
    using namespace OpenFlight;
    
    //lets check acros the ancillary record
    for(int i = 0; i < ipG->getNumberOfAncillaryRecords(); ++i)
    {
        AncillaryRecord* a = ipG->getAncillaryRecord(i);
        
        switch (a->getOpCode())
        {
            case OpenFlight::ocMatrix:
            {
                GroupNode *pGroup = new GroupNode();
                pGroup->mpParent = mpCurrentNode;
                pGroup->mParentTransform = toMatrix4( ((MatrixRecord*)a) );
                
                mpCurrentNode->mChilds.push_back(pGroup);
                mpCurrentNode = pGroup;
            }break;
                
            default: break;
        }
    }
}

//-----------------------------------------------------------------------------
void FltImporter::digData(OpenFlight::HeaderRecord* ipH)
{
    using namespace OpenFlight;
    
    // The headerRecord is usually the first node of the openflight
    // data structure. If there is no root yet, then it is the first
    // node of the data structure. HeaderNode can be encountered
    // multiple times in an openFlight tree, but they will have
    // an ExternalRefence parent.
    if(mpDefinitionRoot == nullptr)
    {
        mpDefinitionRoot = new OpenFlightNode();
        mpCurrentNode = mpDefinitionRoot;
        
        //store the relation between OpenFlightNode and headerNode
        //mDefinitionIdToFltRecord.insert( make_pair(mpDefinitionRoot->mId, ipH) );
        
        //add header stuff here
    }
    
    //add library node to store all palette information
    //add libray node
    LibraryNode *library = new LibraryNode();
    library->mpParent = mpCurrentNode;
    mpCurrentNode->mChilds.push_back(library);
    mpCurrentNode = library;

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
    
    //lets check across the ancillary record
    for(int i = 0; i < ipH->getNumberOfAncillaryRecords(); ++i)
    {
        AncillaryRecord* a = ipH->getAncillaryRecord(i);
        
        switch (a->getOpCode())
        {
            case OpenFlight::ocVertexPalette:
            {
                VertexPaletteRecord* vpr = ((VertexPaletteRecord*)a);
                
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
                                                          v.mNormal.mY) );
                    
                    vp->mColors.push_back(QColor(v.mPackedColor.mRed,
                                                 v.mPackedColor.mGreen,
                                                 v.mPackedColor.mBlue,
                                                 v.mPackedColor.mAlpha));
                }
                
                //store the relation between vertexPool and palette
                mDefinitionIdToFltRecord.insert( make_pair(vp->mId, vpr) );
                
                library->mpVertexPool = vp;
            }break;
            
            case OpenFlight::ocTexturePalette:
            {
                TexturePaletteRecord *tpr = (TexturePaletteRecord *)a;
                
                Image *image = new Image();
                
                // the filename of the texture record must be relative, else,
                // it won't work...
                image->mFilenamePath = ipH->getFilePath() + tpr->getFilenamePath();
                
                mDefinitionIdToFltRecord.insert( make_pair(image->mId, tpr));
                library->mImages.push_back(image);
            } break;
            default: break;
        }
    }
}

//-----------------------------------------------------------------------------
void FltImporter::digData(OpenFlight::ObjectRecord* ipR)
{
    using namespace OpenFlight;
    
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
                
                ModelNode *model = new ModelNode();
                model->mpParent = mpCurrentNode;
                mpCurrentNode->mChilds.push_back(model);
             
                // only vertex and morph vertex has childs
                // could have subface (with pushSubface)
                // lets fetch the vertex list
                PrimaryRecord* vertexList = c->getNumberOfChilds() > 0 ? c->getChild(0) : nullptr;
                if(vertexList && vertexList->getOpCode() == ocVertexList)
                {
                    Face *face = new Face();
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
                        face->mVertexIndices.push_back( vertexIndex );
                    }
                    
                    //--- textures (images)
                    Material *material = new Material();
                    if(faceRecord->getTexturePatternIndex() >= 0)
                    {
                        material->mpImage = getImageFromTexturePatternIndex( faceRecord->getTexturePatternIndex(), currentLibrary );
                    }
                    
                    //--- material
                    if(faceRecord->getMaterialIndex() >= 0)
                    {
                        
                    }
                    
                    face->mpMaterial = material;
                    model->mFaces.push_back(face);
                }
            }break;
            case ocLightPoint: break;
            default: break;
        }
    }
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
void FltImporter::parseFltTree(OpenFlight::HeaderRecord* ipRoot)
{
    using namespace OpenFlight;
    
    deque<PrimaryRecord*> q;
    q.push_back(ipRoot);
    
    while(!q.empty())
    {
        PrimaryRecord* n = q.front();
        q.pop_front();
        
        for(int i = 0; i < n->getNumberOfChilds(); ++i)
        {
            q.push_back(n->getChild(i));
        }
        
        //dig for data on current record
        switch (n->getOpCode())
        {
            case OpenFlight::ocHeader: digData( (HeaderRecord*)n ); break;
            case OpenFlight::ocGroup: digData( (GroupRecord*)n ); break;
            case OpenFlight::ocObject: digData( (ObjectRecord*)n ); break;
            default: break;
        }
    }
}