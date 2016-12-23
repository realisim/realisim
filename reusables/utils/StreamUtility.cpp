
#include "StreamUtility.h"


using namespace std;
using namespace Realisim;
using namespace Utils;

namespace
{
    const StreamUtility::endianness __localFormat = StreamUtility::eLittleEndian; //Windows machine are little endian...
}

//-------------------------------------------------------------------------
bool StreamUtility::needsSwapping() const
{
    return __localFormat != getStreamFormat();
}

//-------------------------------------------------------------------------
bool StreamUtility::readBytes(std::istream& iStream, int iNumberOfBytesToRead, std::string *oV)
{
    char *c = new char[iNumberOfBytesToRead];
    iStream.read(c, iNumberOfBytesToRead);
    
    //no swap involved in reading chars...
    
    *oV = string(c, iNumberOfBytesToRead);
    delete[] c;
    
    return iStream.good();
}

//-------------------------------------------------------------------------
bool StreamUtility::readChar(std::istream& iStream, int iNumberOfCharToRead, std::string *oV)
{
    char *c = new char[iNumberOfCharToRead];
    iStream.read(c, iNumberOfCharToRead);

    //no swap involved in reading chars...

    *oV = string(c);
    delete[] c;

    return iStream.good();
}

//-------------------------------------------------------------------------
bool StreamUtility::readDouble(std::istream& iStream, double *oV)
{
    iStream.read( (char*)oV, sizeof(double) );

    if(needsSwapping()){ swapBytes8((void*)oV); }

    return iStream.good();
}

//-------------------------------------------------------------------------
bool StreamUtility::readFloat32(std::istream& iStream, float *oV)
{
    iStream.read( (char*)oV, sizeof(float) );
    
    if(needsSwapping()){ swapBytes4((void*)oV); }
    
    return iStream.good();
}

//-------------------------------------------------------------------------
bool StreamUtility::readInt8(istream& iStream, int8_t *oV)
{
    iStream.read( (char*)oV, sizeof(int8_t) );
    return iStream.good();
}

//-------------------------------------------------------------------------
bool StreamUtility::readInt16(istream& iStream, int16_t *oV)
{
    iStream.read( (char*)oV, sizeof(int16_t) );

    if(needsSwapping()){ swapBytes2((void*)oV); }

    return iStream.good();
}

//-------------------------------------------------------------------------
bool StreamUtility::readInt32(std::istream& iStream, int32_t *oV)
{
    iStream.read( (char*)oV, sizeof(int32_t) );

    if(needsSwapping()){ swapBytes4((void*)oV); }

    return iStream.good();
}

//-------------------------------------------------------------------------
bool StreamUtility::readUint8(std::istream& iStream, uint8_t *oV)
{
    iStream.read( (char*)oV, sizeof(uint8_t) );
    return iStream.good();
}

//-------------------------------------------------------------------------
bool StreamUtility::readUint16(istream& iStream, uint16_t *oV)
{
    iStream.read( (char*)oV, sizeof(uint16_t) );

    if(needsSwapping()){ swapBytes2((void*)oV); }

    return iStream.good();
}

//-------------------------------------------------------------------------
bool StreamUtility::readUint32(istream& iStream, uint32_t *oV)
{
    iStream.read( (char*)oV, sizeof(uint32_t) );
    
    if(needsSwapping()){ swapBytes4((void*)oV); }
    
    return iStream.good();
}

//-------------------------------------------------------------------------
void StreamUtility::setStreamFormat(endianness iV)
{ mStreamFormat = iV; }

//-------------------------------------------------------------------------
void StreamUtility::swapBytes2(void* iV)
{ 
    char in[2], out[2];
    memcpy(in, iV, 2);
    out[0]  = in[1];
    out[1]  = in[0];
    memcpy(iV, out, 2);
}

//-------------------------------------------------------------------------
void StreamUtility::swapBytes4(void* iV)
{ 
    char in[4], out[4];
    memcpy(in, iV, 4);
    out[0]  = in[3];
    out[1]  = in[2];
    out[2]  = in[1];
    out[3]  = in[0];
    memcpy(iV, out, 4);
}

//-------------------------------------------------------------------------
void StreamUtility::swapBytes8(void* iV)
{
    char in[8], out[8];
    memcpy(in, iV, 8);
    out[0]  = in[7];
    out[1]  = in[6];
    out[2]  = in[5];
    out[3]  = in[4];
    out[4]  = in[3];
    out[5]  = in[2];
    out[6]  = in[1];
    out[7]  = in[0];
    memcpy(iV, out, 8);
}
