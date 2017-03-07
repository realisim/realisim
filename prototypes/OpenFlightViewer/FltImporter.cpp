
#include "Definitions.h"
#include <deque>
#include "FltImporter.h"
#include "math/Matrix4.h"
#include <QFileInfo>
#include <regex>

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

    mDefinitionIdToFltRecord.clear();
    mObjectRecordToGraphicNode.clear();
}

//-----------------------------------------------------------------------------
void FltImporter::applyLayersLogicOnChilds(OpenFlight::PrimaryRecord* ipRecord, IGraphicNode* ipNode)
{
    using namespace OpenFlight;

    if(ipNode == nullptr) return;

    switch (ipNode->mNodeType)
    {
    case IGraphicNode::ntGroup: 
    {
        GroupNode* gn = dynamic_cast<GroupNode*>(ipNode);
        GroupRecord* gr = dynamic_cast<GroupRecord*>(ipRecord);
        if (gn && gr)
        {
            // this is a deprecated way of doing layering... Ideally it would be done
            // at the group level with LayerCode. In the following case, when a group has
            // relativePriority at 1, childs under it are layered from left to right. The
            // first child being the base
            //
            if (gr->getRelativePriority() == 1)
            {
                for (int i = 0; i < gn->mChilds.size(); ++i)
                {
                    IGraphicNode* child = gn->mChilds[i];
                    GroupNode* castedChild = dynamic_cast<GroupNode*>(child);
                    if (castedChild)
                    {
                        //layers starts at 1.
                        castedChild->setLayerIndex(i+1);
                    }
                }
            }
        }

    }break;
    default: break;
    }
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
    
    // note: faudrait il faire une classe spéciale pour les node external reference??
    // genre ExternalReferenceNode...

    GroupNode *pGroup = new GroupNode();
    pGroup->mName = ipE->getFilenamePath();
    pGroup->mParentTransform = fetchTransform(ipE);
    ipParent->addChild(pGroup);

    // mark as instance if needed
    markAsInstance(ipE, pGroup); 

    return pGroup;
}

//-----------------------------------------------------------------------------
GroupNode* FltImporter::digData(OpenFlight::GroupRecord* ipG,
                                IGraphicNode* ipParent,
                                bool *ipDigIntoChilds )
{
    using namespace OpenFlight;

    /*printf("GroupName: %s, GroupRecord priority: %d, layer code: %d, significance: %d\n", 
        ipG->getAsciiId().c_str(),
        ipG->getRelativePriority(),
        ipG->getLayerCode(),
        ipG->getSignificance() );*/

    GroupNode *pGroup = new GroupNode();
    ipParent->addChild(pGroup);
    pGroup->mName = ipG->hasLongIdRecord() ? ipG->getLongIdRecord()->getAsciiId() : ipG->getAsciiId();
    pGroup->mParentTransform = fetchTransform(ipG);
    pGroup->setLayerIndex( ipG->getLayerCode() );
    
    // mark as instance if needed
    markAsInstance(ipG, pGroup);    

    // --- autoroutes
    // autoroutes are tagged by name with the following structure AR:xxxx where xxxx is the ICAO
    std::regex rg("AR:") ; //match a single drive letter followed by a colon and whatever else...
    std::smatch m;
    //skip autoroutes for now...
    if (regex_search(pGroup->mName, m, rg))
    {
        *ipDigIntoChilds = false;
    }

    return pGroup;
}

//-----------------------------------------------------------------------------
LibraryNode* FltImporter::digData(OpenFlight::HeaderRecord* ipH,
                                  IGraphicNode* ipParent)
{
    using namespace OpenFlight;
    
    // The headerRecord is usually the first node of the openflight
    // data structure. If there is no root yet, it will become the root:
    // first node of the data structure. HeaderNode can be encountered
    // multiple times in an openFlight tree, but they will have
    // an ExternalRefence as parent.

    OpenFlightNode* opfNode = new OpenFlightNode();
    opfNode->mName = ipH->hasLongIdRecord() ? ipH->getLongIdRecord()->getAsciiId() : ipH->getAsciiId();

    if( ipParent == nullptr )
    {
        mpGraphicNodeRoot = opfNode;
    }
    else
    { ipParent->addChild(opfNode); }
    
    markAsInstance(ipH, opfNode);

    //store the relation between OpenFlightNode and headerNode
    //mDefinitionIdToFltRecord.insert( make_pair(mpGraphicNodeRoot->getId(), ipH) );
    
    //add header stuff here
    
    //add library node to store all palette information
    LibraryNode *library = new LibraryNode();
    library->mName = "Library";
    opfNode->addChild(library);

    if (!opfNode->isInstantiated())
    {
        // check external reference flags, to properly populate
        // the library node with appropriate palette since external
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
            mDefinitionIdToFltRecord.insert( make_pair(vp->getId(), vpr) );

            library->mpVertexPool = vp;
        }

        //--- texture palettes
        for (int i = 0; i < ipH->getNumberOfTexturePalettes(); ++i)
        {
            TexturePaletteRecord* tpr = ipH->getTexturePalette(i);
            if (tpr != nullptr)
            {
                // the filename of the texture record must be relative, else,
                // it won't work...
                // also, lets make it cannonical...
                //
                // Note: We can assum that images inside the texture palette are never duplicated...
                //
                QFileInfo fi( QString::fromStdString(ipH->getFilePath() + tpr->getFilenamePath()) );
                QString cannonical = fi.canonicalFilePath();
                if (!cannonical.isEmpty())
                {
                    // check if a texture with the same path has already been added to the 

                    Image *image = new Image();
                    image->mFilenamePath = cannonical.toStdString();
                    image->loadMetaData();

                    library->mImages.push_back(image);
                    mDefinitionIdToFltRecord.insert( make_pair(image->getId(), tpr));
                }
                else
                {
                    printf("Image with unreachable path found: %s add warning...\n", fi.filePath().toStdString().c_str());
                }
            }
        }
    }    
    
    return library;
}

//-----------------------------------------------------------------------------
ModelNode* FltImporter::digData(OpenFlight::ObjectRecord* ipR,
                                IGraphicNode* ipParent)
{
    using namespace OpenFlight;

    // here we will create a new model. In order to limit the number of models
    // we will bundle all models under a group in the same model. This means that
    // all faces, vertices of all ObjectRecord will be bundle in the same modelNode.

    // fetch parent to find out if it  already has a model node. If not, create a new
    // one, else use the existing one.
    //
    ModelNode *model = nullptr;
    for (size_t i = 0; i < ipParent->mChilds.size() && model == nullptr; ++i)
    {
        IGraphicNode *child = ipParent->mChilds[i];
        if(child->mNodeType == IGraphicNode::ntModel)
        { model = dynamic_cast<ModelNode*>(child); }
    }
    
    // if we found no model under the parent, lets create one!
    //
    if(model == nullptr)
    {
        model = new ModelNode();
        ipParent->addChild(model);

        model->mName = ipR->hasLongIdRecord() ? ipR->getLongIdRecord()->getAsciiId() : ipR->getAsciiId();
        model->mParentTransform = fetchTransform(ipR);

        markAsInstance(ipR, model);
    }    

    //this is an instance...
    if(model->isInstantiated())
    {
        assert(ipR->getUseCount() > 1);

        auto it = mObjectRecordToGraphicNode.find(ipR);
        ModelNode *originalModel = dynamic_cast<ModelNode *>( it->second );
        model->setAsInstanceOf( originalModel );
    }
    else
    {
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

                    LibraryNode *currentLibrary = getLibraryFor(model);
                    face->mpVertexPool = currentLibrary->mpVertexPool;

                    //--- vertices
                    // fetch the VertexPalette from openFlight and resolve
                    // the byte offset into actual index into the vertex array.
                    VertexPaletteRecord *vpr =
                        (VertexPaletteRecord*)getRecordFromDefinitionId(face->mpVertexPool->getId());

                    VertexListRecord *vlr = (VertexListRecord *)vertexList;
                    for(int i = 0; i < vlr->getNumberOfVertices(); ++i)
                    {
                        int vertexIndex = vpr->getIndexFromByteOffset( vlr->getByteOffsetIntoVertexPalette(i) );
                        assert( vertexIndex != -1 );
                        face->mVertexIndices.push_back( vertexIndex );
                    }
                    model->addFace(face);

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
    }    
    

    return model;
}

//-----------------------------------------------------------------------------
LevelOfDetailNode* FltImporter::digData(OpenFlight::LevelOfDetailRecord* ipR, IGraphicNode* ipParent)
{
    LevelOfDetailNode *pLOD = new LevelOfDetailNode();
    ipParent->addChild(pLOD);
    pLOD->mName = ipR->hasLongIdRecord() ? ipR->getLongIdRecord()->getAsciiId() : ipR->getAsciiId();
    pLOD->mName += " (LOD)";
    pLOD->setSwitchInDistance( ipR->getSwitchInDistance() );
    pLOD->setSwitchOutDistance( ipR->getSwitchOutDistance() );
    pLOD->setOriginalLodCenter( Point3d( ipR->getLodX(), ipR->getLodY(), ipR->getLodZ() ) );

    // mark as instance if needed
    markAsInstance(ipR, pLOD);    


    return pLOD;
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
            auto it = mDefinitionIdToFltRecord.find(iLibrary->mImages[i]->getId());
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
void FltImporter::markAsInstance(OpenFlight::PrimaryRecord* ipR, IGraphicNode* ipNode)
{
    if (ipR->getUseCount() > 0)
    {
        auto it = mObjectRecordToGraphicNode.find(ipR);
        if (it != mObjectRecordToGraphicNode.end())
        {
            IDefinition *nodeDef = dynamic_cast<IDefinition*>(ipNode);
            IDefinition *instancedFrom = dynamic_cast<IDefinition*>(it->second);
            if(nodeDef && instancedFrom)
            { nodeDef->setAsInstanceOf( instancedFrom->getId() ); }
        }
    }
}

//-----------------------------------------------------------------------------
void FltImporter::parseFltTree(OpenFlight::PrimaryRecord* ipRecord, IGraphicNode* ipCurrentParent)
{
    using namespace OpenFlight;

    //depth first parsing
    
    bool digIntoChild = true;
    IGraphicNode *currentNode = nullptr;
    //dig for data on current record
    switch (ipRecord->getOpCode())
    {
        case OpenFlight::ocHeader:
            currentNode = digData( (HeaderRecord*)ipRecord, ipCurrentParent );
            break;
        case OpenFlight::ocExternalReference:
            currentNode = digData( (ExternalReferenceRecord*)ipRecord, ipCurrentParent );

            // dig into child only if current node has ref count 1, else
            // it means it is already a complete subtree
            //
            digIntoChild = currentNode->getUseCount() == 1; 
            break;
        case OpenFlight::ocGroup:
            currentNode = digData( (GroupRecord*)ipRecord, ipCurrentParent, &digIntoChild );
            break;
        case OpenFlight::ocObject:
            currentNode = digData( (ObjectRecord*)ipRecord, ipCurrentParent );
            break;
        case OpenFlight::ocLevelOfDetail:
            currentNode = digData( (LevelOfDetailRecord*)ipRecord, ipCurrentParent );
            break;
        case OpenFlight::ocUnsupported:
        {
            // these explicitely ignored are temporary... Since they are not yet supported
            // the appear here and creating a node for each of them is a bit overkill.
            // Nonetheless, those which are not explicitly ignored needs to have a node create
            // to preserve the tree structure.
            //
            OpenFlight::opCode originalOpCode = ((UnsupportedRecord*)ipRecord)->getOriginalOpCode();
            const bool explicitlyIgnored = (
                originalOpCode == OpenFlight::ocIndexedLightPoint ||
                originalOpCode == OpenFlight::ocReserved103 ||
                originalOpCode == OpenFlight::ocReserved104 ||
                originalOpCode == OpenFlight::ocReserved110 ||
                originalOpCode == OpenFlight::ocReserved117 ||
                originalOpCode == OpenFlight::ocReserved118 ||
                originalOpCode == OpenFlight::ocReserved120 ||
                originalOpCode == OpenFlight::ocReserved121 ||
                originalOpCode == OpenFlight::ocReserved124 ||
                originalOpCode == OpenFlight::ocReserved125 ||
                originalOpCode == OpenFlight::ocReserved134 ||
                originalOpCode == OpenFlight::ocReserved144 ||
                originalOpCode == OpenFlight::ocReserved146 );

                if (!explicitlyIgnored)
                {
                    GroupNode *pGroup = new GroupNode();
                    pGroup->mName = string("Unsupported openfligNode: ") + OpenFlight::toString( originalOpCode );
                    ipCurrentParent->addChild(pGroup);
                    currentNode = pGroup;
                }
        }break;
        default: 
        {
            // this catches records that have been parsed and are supported by openFlightReader
            // but are not handled by flt importer...
            // since a missing node can cause a crash, we will assert when a node that is
            // not explicitly ignored is catched...
            
            const bool explicitlyIgnored = 
                ipRecord->getOpCode() == OpenFlight::ocFace ||
                ipRecord->getOpCode() == OpenFlight::ocVertexList;
            assert(explicitlyIgnored);

            //// we still create a group so that the tree wont be broken...
            //if (!explicitlyIgnored)
            //{
            //    GroupNode *pGroup = new GroupNode();
            //    pGroup->mName = string("Supported openfligNode not implemented in FltImporter: ") + OpenFlight::toString( ipRecord->getOpCode() );
            //    ipCurrentParent->addChild(pGroup);
            //    currentNode = pGroup;
            //}

        } break;
    }
    
    // add all nodes to the map
    if (currentNode != nullptr)
    {
        mObjectRecordToGraphicNode.insert( make_pair(ipRecord, currentNode) );
    }
    
    if (digIntoChild)
    {
        for(int i = 0; i < ipRecord->getNumberOfChilds(); ++i)
        { parseFltTree(ipRecord->getChild(i), currentNode); }
    }
    
    // we just ended processing all childs, at this point, we might need to apply
    // some logic are calculations that required the childs to be present.
    //
    // For example, layering and boundingbox calculations requires all childs...
    //

    // apply parent logic onto childs...
    // Layering is set at the group node for it's childrens and there are many way
    // to do it, so when we are done with the childrens, we will check if there
    // are actions to be taken
    applyLayersLogicOnChilds(ipRecord, currentNode);
}