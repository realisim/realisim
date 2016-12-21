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
    Definition* getDefinitionRoot() { return mpDefinitionRoot; }
    
protected:
void applyTransformCarryover(IRenderable*);
    GroupNode* digData(OpenFlight::ExternalReferenceRecord*, Definition*);
    GroupNode* digData(OpenFlight::GroupRecord*, Definition*);
    LibraryNode* digData(OpenFlight::HeaderRecord*, Definition*);
    ModelNode* digData(OpenFlight::ObjectRecord*, Definition*);
    realisim::math::Matrix4 fetchTransform(OpenFlight::PrimaryRecord*, bool* = nullptr);
    ImageNode* getImageFromTexturePatternIndex(int, LibraryNode* iLibrary);
    OpenFlight::Record* getRecordFromDefinitionId(int);
    void parseFltTree(OpenFlight::PrimaryRecord*, Definition* ipCurrentParent);
    
    //--- data
    OpenFlight::HeaderRecord* mpHeaderRecord; //owned
    
    Definition* mpDefinitionRoot; //not owned
Definition* mpCurrentNode;
    
    std::map<int, OpenFlight::Record*> mDefinitionIdToFltRecord;
};