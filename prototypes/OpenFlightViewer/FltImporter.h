#pragma once

#include "openFlight/Records.h"
#include <map>
#include <math/BoundingBox.h>

class FltImporter
{
public:
    FltImporter() = delete;
    FltImporter(OpenFlight::HeaderRecord*);
    FltImporter(const FltImporter&) = delete;
    FltImporter& operator=(const FltImporter&) = delete;
    ~FltImporter();
    
    OpenFlight::HeaderRecord* getOpenFlightRoot() { return mpHeaderRecord; }
    IGraphicNode* getGraphicNodeRoot() { return mpGraphicNodeRoot; }
    
protected:
void applyTransformCarryover(IRenderable*);
    void applyLayersLogicOnChilds(OpenFlight::PrimaryRecord* ipRecord, IGraphicNode* ipNode);
    GroupNode* digData(OpenFlight::ExternalReferenceRecord*, IGraphicNode*);
    GroupNode* digData(OpenFlight::GroupRecord*, IGraphicNode*, bool* ipDigIntoChilds);
    LibraryNode* digData(OpenFlight::HeaderRecord*, IGraphicNode*);
    ModelNode* digData(OpenFlight::ObjectRecord*, IGraphicNode*);
    LevelOfDetailNode* digData(OpenFlight::LevelOfDetailRecord*, IGraphicNode*);
    realisim::math::Matrix4 fetchTransform(OpenFlight::PrimaryRecord*, bool* = nullptr);
    void finalize();
    Image* getImageFromTexturePatternIndex(int, LibraryNode* iLibrary);
    OpenFlight::Record* getRecordFromDefinitionId(int);
    void markAsInstance(OpenFlight::PrimaryRecord*, IGraphicNode*);
    void markEmptyNodeForDeletion(IGraphicNode*);
    void parseFltTree(OpenFlight::PrimaryRecord*, IGraphicNode* ipCurrentParent);
    void removeMarkedForDeletionNode(IGraphicNode*);
    
    
    //--- data
    OpenFlight::HeaderRecord* mpHeaderRecord; //owned
    
    IGraphicNode* mpGraphicNodeRoot; //not owned
    
    std::map<int, OpenFlight::Record*> mDefinitionIdToFltRecord;
    std::map<OpenFlight::PrimaryRecord*, IGraphicNode*> mObjectRecordToGraphicNode;
};