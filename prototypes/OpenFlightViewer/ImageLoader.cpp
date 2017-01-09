#include <cassert>
#include "ImageLoader.h"
#if defined(_OPENMP)
#include <omp.h>
#endif
#include "Utils/StreamUtility.h"
#include <vector>
#include "utils/Timer.h"

using namespace std;

RgbImageLoader::RgbImageLoader() :
mFilenamePath(),
mpImageData(nullptr),
mIsValid(false),
mHasOwnershipOfImageData(true),
mStorage(0),
mBytesPerPixel(0),
mDimension(0),
mPixelSizeX(0),
mPixelSizeY(0),
mNumberOfChannels(0),
mMinumumPixelValue(0),
mMaximumPixelValue(0),
mImageName(),
mColorMapId(0)
{}

//------------------------------------------------------------------------------
RgbImageLoader::~RgbImageLoader()
{
    clear();
}

//------------------------------------------------------------------------------
void RgbImageLoader::clear()
{
    if(mpImageData != nullptr && hasOwnershipOfImageData())
    {
        delete mpImageData;
    }
    
    mIsValid = false;
    mHasOwnershipOfImageData = true;
    mStorage = 0;
    mBytesPerPixel = 0;
    mDimension = 0;
    mPixelSizeX = 0;
    mPixelSizeY = 0;
    mNumberOfChannels = 0;
    mMinumumPixelValue = 0;
    mMaximumPixelValue = 0;
    mImageName = string();
    mColorMapId = 0;
}

//------------------------------------------------------------------------------
void RgbImageLoader::decompress(const std::string &iRleData, unsigned char *iDest)
{
    int rleIndex = 0;
    int count = 0;
    unsigned char pixel;
    for (;;) {
        pixel = iRleData[rleIndex++];
        count = (int)(pixel & 0x7F);
        if (!count) {
            return;
        }
        if (pixel & 0x80) {
            while (count--) {
                *iDest++ = iRleData[rleIndex++];
            }
        } else {
            pixel = iRleData[rleIndex++];
            while (count--) {
                *iDest++ = pixel;
            }
        }
    }
}

//------------------------------------------------------------------------------
int RgbImageLoader::getBytesPerPixel() const
{ return mBytesPerPixel; }

//------------------------------------------------------------------------------
int RgbImageLoader::getColorMapId() const
{ return mColorMapId; }

//------------------------------------------------------------------------------
int RgbImageLoader::getDimension() const
{ return mDimension; }

//------------------------------------------------------------------------------
const std::string& RgbImageLoader::getFilenamePath() const
{ return mFilenamePath; }

//------------------------------------------------------------------------------
unsigned char* RgbImageLoader::getImageData() const
{ return mpImageData; }

//------------------------------------------------------------------------------
std::string RgbImageLoader::getImageName() const
{ return mImageName; }

//------------------------------------------------------------------------------
int RgbImageLoader::getMaximumPixelValue() const
{ return mMaximumPixelValue; }

//------------------------------------------------------------------------------
int RgbImageLoader::getMinumumPixelValue() const
{ return mMinumumPixelValue; }

//------------------------------------------------------------------------------
int RgbImageLoader::getNumberOfChannels() const
{ return mNumberOfChannels; }

//------------------------------------------------------------------------------
int RgbImageLoader::getPixelSizeX() const
{ return mPixelSizeX; }

//------------------------------------------------------------------------------
int RgbImageLoader::getPixelSizeY() const
{ return mPixelSizeY; }

//------------------------------------------------------------------------------
int RgbImageLoader::getStorage() const
{ return mStorage; }

//------------------------------------------------------------------------------
bool RgbImageLoader::hasImageData() const
{ return mpImageData != nullptr; }

//------------------------------------------------------------------------------
bool RgbImageLoader::hasOwnershipOfImageData() const
{ return mHasOwnershipOfImageData; }

//------------------------------------------------------------------------------
bool RgbImageLoader::isRleEncoded() const
{ return getStorage() == 1; }

//------------------------------------------------------------------------------
bool RgbImageLoader::isValid() const
{ return mIsValid; }

//------------------------------------------------------------------------------
void RgbImageLoader::load()
{
    clear();
    // Parse the file
    ifstream ifs;
    ifs.open(getFilenamePath(), ifstream::in | ios_base::binary);
    if(!ifs.fail())
    {
        mIsValid = loadHeader(ifs);
        
        if(isValid())
        {
            if(!isRleEncoded())
            {
                mIsValid = parseAsVerbatim(ifs);
            }
            else
            {
                mIsValid = parseAsRle(ifs);
                if(!isValid())
                {printf("RgbImageLoader::load() - Error while parsing rle data...\n");}
            }
        }
    }
}

//------------------------------------------------------------------------------
void RgbImageLoader::loadHeader()
{
    clear();
    // Parse the file
    ifstream ifs;
    ifs.open(getFilenamePath(), ifstream::in | ios_base::binary);
    if(!ifs.fail())
    {
        mIsValid = loadHeader(ifs);
    }
}

//------------------------------------------------------------------------------
bool RgbImageLoader::loadHeader(ifstream& ifs)
{
    // see specification
    Realisim::Utils::StreamUtility su;
    su.setStreamFormat(Realisim::Utils::StreamUtility::eBigEndian);
    
    bool ok = true;
    ok &= su.readInt16(ifs, &mMagicNumber);
    ok &= mMagicNumber == 474;
    
    if(ok)
    {
        ok &= su.readInt8(ifs, &mStorage);
        
        ok &= su.readInt8(ifs, &mBytesPerPixel);
        ok &= su.readUint16(ifs, &mDimension);
        ok &= su.readUint16(ifs, &mPixelSizeX);
        ok &= su.readUint16(ifs, &mPixelSizeY);
        ok &= su.readUint16(ifs, &mNumberOfChannels);
        ok &= su.readInt32(ifs, &mMinumumPixelValue);
        ok &= su.readInt32(ifs, &mMaximumPixelValue);
        
        int32_t dummy;
        ok &= su.readInt32(ifs, &dummy);
        
        ok &= su.readChar(ifs, 80, &mImageName);
        ok &= su.readInt32(ifs, &mColorMapId);
    }
    
    ifs.seekg(512); //go to end of header.
    
    // a few assumption and checks
    assert(getBytesPerPixel() == 1 );
    
    if( getBytesPerPixel() != 1 )
    {
        ok = false;
        clear();
        printf("RgbImageLoader::loadHeader - current SGI RGB format is not supported.\n");
    }
    
    return ok;
}

//------------------------------------------------------------------------------
unsigned char* RgbImageLoader::giveOwnershipOfImageData()
{
    mHasOwnershipOfImageData = false;
    return mpImageData;
}

//------------------------------------------------------------------------------
// see documentation (link in h file).
bool RgbImageLoader::parseAsRle(std::ifstream &ifs)
{
    bool ok = true;
    
    Realisim::Utils::StreamUtility su;
    su.setStreamFormat(Realisim::Utils::StreamUtility::eBigEndian);
    
    const int numChannels = getNumberOfChannels();
    const int sx = getPixelSizeX();
    const int sy = getPixelSizeY();
    
    //--- read offset table
    int32_t* startTable = nullptr;
    int32_t* lenghtTable = nullptr;;
    const int tableLenght = sy * numChannels;
    
    startTable = new int32_t[tableLenght];
    for(int i = 0; i < tableLenght; ++i)
    {
        ok &= su.readInt32(ifs, &startTable[i]);
    }
    
    lenghtTable = new int32_t[tableLenght];
    for(int i = 0; i < tableLenght; ++i)
    {
        ok &= su.readInt32(ifs, &lenghtTable[i]);
    }

    // compute remaining size of the stream
    const std::streampos currentStreamPos = ifs.tellg();
    ifs.seekg(0, std::ios::end);
    const size_t remainingSize = ifs.tellg() - currentStreamPos;
    ifs.seekg(currentStreamPos);
    
    // read all image data at once, it is faster and enables openmp usage.
    // it brings in a twist... the rleOffset from startTable are relative to
    // the file, so we will need to adjust the rleOffset to be relative to
    // the rleRawData buffer. (relativeRleOffset = rleOffset - currentStreamPos)
    // 
    string rleRawData;
    ok &= su.readBytes(ifs, remainingSize, &rleRawData);

    //--- create final buffer and recombine channel buffer into a
    // single rgb/rgba buffer.
    const int finalSize = sx * sy * numChannels;
    mpImageData = new unsigned char[finalSize];

    //--- For each color channel, decompress each scanline and repack rgb(a) into final buffer
    // see parseAsVerbatim for explanation on packing into final buffer.
    //vector<unsigned char*> channels(numChannels);
    unsigned char* scanline = nullptr;
    int channelIndex = 0;
    #pragma omp parallel for num_threads(numChannels) private(channelIndex, scanline) shared(ok)
    for(channelIndex = 0; channelIndex < numChannels; ++channelIndex)
    {
        scanline = new unsigned char[sx];
        
        //decompress each scanline of channel n
        for(int row = 0; row < sy && ok; ++row)
        {
            //fetch scanline
            int32_t rleOffset = startTable[row + channelIndex * sy];
            int32_t rleLength = lenghtTable[row + channelIndex * sy];
            
            // grab the rledata for the current scanline
            string rleData = rleRawData.substr(rleOffset - currentStreamPos, rleLength);
            decompress( rleData, &scanline[0] );

            for(int i = 0; i < sx; ++i)
            { mpImageData[ (row * sx * numChannels) + (i * numChannels) + channelIndex] = scanline[i]; }
        }

        delete[] scanline;
    }
    
    //cleanup
    delete[] startTable;
    delete[] lenghtTable;
    
    return ok;
}
//------------------------------------------------------------------------------
bool RgbImageLoader::parseAsVerbatim(std::ifstream &ifs)
{
    bool ok = true;
    
    //each channel is stored one after the other... so lets read each of them
    //separately and repack them in rgb or rgba format on the fly.
    Realisim::Utils::StreamUtility su;
    su.setStreamFormat(Realisim::Utils::StreamUtility::eBigEndian);
        
    //read all data at once, it is faster and enables using openmp
    string rawData;
    ok &= su.readBytes(ifs, getPixelSizeX() * getPixelSizeY() * getNumberOfChannels(), &rawData);

    if (ok)
    {
        const int n = getNumberOfChannels();

        // create final buffer to recombine channel buffer into a
        // single rgb/rgba buffer.
        const int finalSize = getPixelSizeX() * getPixelSizeY() * n;
        mpImageData = new unsigned char[finalSize];

        int i = 0;
        const int sizeOfChannelInBytes = getPixelSizeX() * getPixelSizeY();

        //on channel per thread.
        #pragma omp parallel for num_threads(n) private(i)
        for(i = 0; i < n; ++i)
        {
            for(int j = 0; j < sizeOfChannelInBytes; ++j)
            {
                mpImageData[(j * n) + i] = rawData[ i*sizeOfChannelInBytes + j ];
            }
        }

    }
    return ok;
}


//------------------------------------------------------------------------------
void RgbImageLoader::setFilenamePath(const std::string& iV)
{ mFilenamePath = iV; }


