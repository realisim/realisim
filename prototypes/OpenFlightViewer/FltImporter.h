#pragma once

#include "openFlight/Records.h"
#include <map>

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
    GroupNode* digData(OpenFlight::ExternalReferenceRecord*, IGraphicNode*);
    GroupNode* digData(OpenFlight::GroupRecord*, IGraphicNode*);
    LibraryNode* digData(OpenFlight::HeaderRecord*, IGraphicNode*);
    ModelNode* digData(OpenFlight::ObjectRecord*, IGraphicNode*);
    realisim::math::Matrix4 fetchTransform(OpenFlight::PrimaryRecord*, bool* = nullptr);
    Image* getImageFromTexturePatternIndex(int, LibraryNode* iLibrary);
    OpenFlight::Record* getRecordFromDefinitionId(int);
    void parseFltTree(OpenFlight::PrimaryRecord*, IGraphicNode* ipCurrentParent);
    
    //--- data
    OpenFlight::HeaderRecord* mpHeaderRecord; //owned
    
    IGraphicNode* mpGraphicNodeRoot; //not owned
    
    std::map<int, OpenFlight::Record*> mDefinitionIdToFltRecord;
};