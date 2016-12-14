
#include "VertexListRecord.h"
#include <sstream>
#include "StreamUtilities.h"

using namespace std;
using namespace OpenFlight;

//------------------------------------------------------------------------------
VertexListRecord::VertexListRecord(PrimaryRecord* ipParent) :
PrimaryRecord(ipParent)
{}

//------------------------------------------------------------------------------
VertexListRecord::~VertexListRecord()
{}

//------------------------------------------------------------------------------
const std::vector<int32_t>& VertexListRecord::getByteOffsets() const
{ return mByteOffsets; }

//------------------------------------------------------------------------------
int32_t VertexListRecord::getByteOffsetIntoVertexPalette(int i) const
{
    int r = 0;
    if(i >= 0 && i < (int)mByteOffsets.size())
    { r = mByteOffsets[i]; }
    return r;
}

//------------------------------------------------------------------------------
int VertexListRecord::getNumberOfVertices() const
{ return (int)mByteOffsets.size(); }

//------------------------------------------------------------------------------
bool VertexListRecord::parseRecord(const std::string& iRawRecord, int iVersion)
{
    Record::parseRecord(iRawRecord, iVersion);
    
    stringstream iss(stringstream::in | stringstream::binary);
    iss.str( iRawRecord );

    // Lets move by 4 to skip the opCode and recordLenght... we already know
    // we have a valid record at this point.
    //
    iss.seekg(4);
    bool ok = true;
    
    int numberOfVertices = (getRecordLength() - 4) / 4;
    for(int i = 0; i < numberOfVertices && ok; ++i)
    {
        int32_t offset = 0;
        ok &= readInt32(iss, offset);
        mByteOffsets.push_back(offset);
    }
    
    return ok;
}
