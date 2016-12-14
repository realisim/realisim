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
    Node* getDefinitionRoot() { return mpDefinitionRoot; }
    
protected:
    void digData(OpenFlight::GroupRecord*);
    void digData(OpenFlight::HeaderRecord*);
    void digData(OpenFlight::ObjectRecord*);
    Image* getImageFromTexturePatternIndex(int, LibraryNode* iLibrary);
    OpenFlight::Record* getRecordFromDefinitionId(int);
    
    void parseFltTree(OpenFlight::HeaderRecord*);
    
    OpenFlight::HeaderRecord* mpHeaderRecord; //owned
    
    Node* mpDefinitionRoot; //not owned
    Node* mpCurrentNode;
    
    std::map<int, OpenFlight::Record*> mDefinitionIdToFltRecord;
};