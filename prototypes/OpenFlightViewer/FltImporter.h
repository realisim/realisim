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
    void digData(OpenFlight::GroupRecord*);
    void digData(OpenFlight::HeaderRecord*);
    void digData(OpenFlight::ObjectRecord*);
    ImageNode* getImageFromTexturePatternIndex(int, LibraryNode* iLibrary);
    OpenFlight::Record* getRecordFromDefinitionId(int);
    
    void parseFltTree(OpenFlight::HeaderRecord*);
    
    OpenFlight::HeaderRecord* mpHeaderRecord; //owned
    
    Definition* mpDefinitionRoot; //not owned
    Definition* mpCurrentNode;
    
    std::map<int, OpenFlight::Record*> mDefinitionIdToFltRecord;
};