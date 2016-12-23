#include <cassert>
#include "ImageLoader.h"
#include "Utils/StreamUtility.h"
#include <vector>

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
                {printf("RgbImageLoader::load() - Error while parsing rle data...");}
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
    const int numBytes = getBytesPerPixel();
    
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
    
    //--- create each color channel
    vector<unsigned char*> channels(numChannels);
    for(int channelIndex = 0; channelIndex < numChannels && ok; ++channelIndex)
    {
        const int sizeOfChannelInBytes = sx * sy * numBytes;
        channels[channelIndex] = new unsigned char[sizeOfChannelInBytes];
        
        //decompress each scanline of channel n
        for(int row = 0; row < sy && ok; ++row)
        {
            //fetch scanline
            int32_t rleOffset = startTable[row + channelIndex * sy];
            int32_t rleLength = lenghtTable[row + channelIndex * sy];
            
            string rleData;
            ifs.seekg(rleOffset);
            ok &= su.readBytes(ifs, rleLength, &rleData);
            
            //decompress rle scanline into color channel
            decompress( rleData, &channels[channelIndex][row * sx] );
        }
    }

    if(ok)
    {
        // create final buffer and recombine channel buffer into a
        // single rgb/rgba buffer.
        const int finalSize = sx * sy * numBytes * numChannels;
        mpImageData = new unsigned char[finalSize];
        for(int i = 0; i < finalSize / numChannels; ++i)
        {
            for(int j = 0; j < numChannels; ++j)
            {
                mpImageData[(i * numChannels) + j] = channels[j][i];
            }
        }
    }
    
    //cleanup
    delete[] startTable;
    delete[] lenghtTable;
    
    for(int i = 0; i < numChannels; ++i)
    { delete[] channels[i]; }
    
    return ok;
}
//------------------------------------------------------------------------------
bool RgbImageLoader::parseAsVerbatim(std::ifstream &ifs)
{
    bool ok = true;
    
    //each channel is stored one after the other... so lets read each of them
    //separately and repack them in rgb or rgba format at the end.
    Realisim::Utils::StreamUtility su;
    su.setStreamFormat(Realisim::Utils::StreamUtility::eBigEndian);
    
    const int n = getNumberOfChannels();
    const int b = getBytesPerPixel();
    
    vector<unsigned char*> channels(n);
    for(int i = 0; i < n && ok; ++i)
    {
        const int sizeOfChannelInBytes = getPixelSizeX() * getPixelSizeY() * b;
        channels[i] = new unsigned char[sizeOfChannelInBytes];
        
        for(int j = 0; j < sizeOfChannelInBytes && ok; ++j)
        {
            ok &= su.readUint8(ifs, &channels[i][j]);
        }
    }
    
    // create final buffer and recombine channel buffer into a
    // single rgb/rgba buffer.
    const int finalSize = getPixelSizeX() * getPixelSizeY() * b * n;
    mpImageData = new unsigned char[finalSize];
    for(int i = 0; i < finalSize / n; ++i)
    {
        for(int j = 0; j < n; ++j)
        {
            mpImageData[(i * n) + j] = channels[j][i];
        }
    }
    
    // cleanup
    for(int i = 0; i < n; ++i)
    { delete[] channels[i]; }
    
    return ok;
}


//------------------------------------------------------------------------------
void RgbImageLoader::setFilenamePath(const std::string& iV)
{ mFilenamePath = iV; }


