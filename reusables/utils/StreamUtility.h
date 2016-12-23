
#pragma once

#include <istream>
#include <stdint.h>
#include <string>

namespace Realisim
{
namespace Utils
{
    class StreamUtility
    {
    public:
        StreamUtility() = default;
        StreamUtility(const StreamUtility&) = delete;
        StreamUtility& operator=(const StreamUtility&) = delete;
        ~StreamUtility() = default;
        
        enum endianness{ eBigEndian=0, eLittleEndian };
        
        endianness getStreamFormat() const {return mStreamFormat;}
        bool readBytes(std::istream& iStream, int iNumberOfBytesToRead, std::string *oV);
        bool readChar(std::istream& iStream, int iNumberOfCharToRead, std::string *oV);
        bool readDouble(std::istream& iStream, double *oV);
        bool readFloat32(std::istream& iStream, float *oV);
        bool readInt8(std::istream& iStream, int8_t *oV);
        bool readInt16(std::istream& iStream, int16_t *oV);
        bool readInt32(std::istream& iStream, int32_t *oV);
        bool readUint8(std::istream& iStream, uint8_t *oV);
        bool readUint16(std::istream& iStream, uint16_t *oV);
        bool readUint32(std::istream& iStream, uint32_t *oV);
        void setStreamFormat(endianness);
        
    protected:
        bool needsSwapping() const;
        void swapBytes2(void* iV);
        void swapBytes4(void* iV);
        void swapBytes8(void* iV);
        
        endianness mStreamFormat;
    };
}
}